#include "animation.hpp"
#include "gui.hpp"
#include "init.hpp"
#include "keyboard.h"
#include "errtbl.hpp"
#include "cia.hpp"
#include "download.hpp"
#include "thread.h"
#include "cyvar.hpp"
#include "screen.hpp"
#include <assert.h>
#include <stdarg.h>
#include <unistd.h>
#include <fstream>
#include <dirent.h>
#include <cmath>
static C2D_SpriteSheet sprites;
static C2D_SpriteSheet meta_img;
char errorstr[2048];
int errorcode=0;
int touchpt, touchpx, touchpy;
int touchot, touchox, touchoy;
int waitictimer;
int waitposx, waitposy, waitalpha=15;
int maincnt, waitmc;
float millisec;
std::string keyboardIn;
char stringtochar_buf[8192];
C2D_TextBuf sizeBuf;
C2D_Font systemFont;
int CFGLang;
extern u32 topNotificationColor;
extern std::string topNotificationText;
extern std::string top_screen_title;
extern bool changeTopTitle;
extern bool disableTopTitle;
extern bool disableTopTxtBG;
extern bool exiting;
extern bool waiting;
extern bool showProgressBar;
extern curl_off_t downloadTotal;
extern curl_off_t downloadNow;
extern u16 progressBarType;
extern int pageid;
extern float scx;
extern float scy;
extern float pagex;
extern float pagey;
extern std::string meta_prjn[];
extern std::string meta_title[];
extern std::string meta_desc[];
extern std::string downloadList[];
extern u32 meta_ver[];
extern u32 meta_total;
extern u64 topTextID;

bool ExtDataFolderDeleteDialog;
int retry;
u16 downloadNo=1; /* Initialising the file count with 1, to prevent div by 0 */
u16 downloadedFiles;

int down_y;
size_t down_s;
float down_vy=-240.0f;
int down_max;
bool down_show_props;

bool fadein = true;
bool fadeout;
u8 fadealpha;
u8 fadecolor;
C3D_RenderTarget *top;
C3D_RenderTarget *bottom;
int buttonpressed=-1;

float fadea;
u8 fader;
u8 fadeg;
u8 fadeb;

void Gui::clearTextBufs(void) {
	C2D_TextBufClear(sizeBuf);
}
Result Gui::init(void) {
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	C2D_Prepare();
	
	u32 transFlags=	GX_TRANSFER_FLIP_VERT(false)|
					GX_TRANSFER_OUT_TILED(false)|
					GX_TRANSFER_RAW_COPY(false)|
					GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8)|
					GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8)|
					GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO);
	
	top = C3D_RenderTargetCreate(240, 400, GPU_RB_RGBA8, GPU_RB_DEPTH16);
	C3D_RenderTargetClear(top, C3D_CLEAR_ALL, 0, 0);
	C3D_RenderTargetSetOutput(top, GFX_TOP, GFX_LEFT, transFlags);
	
	bottom = C3D_RenderTargetCreate(240, 320, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
	C3D_RenderTargetClear(bottom, C3D_CLEAR_ALL, 0, 0);
	C3D_RenderTargetSetOutput(bottom, GFX_BOTTOM, GFX_LEFT, transFlags);
	
	sizeBuf = C2D_TextBufNew(8192);
	sprites   = C2D_SpriteSheetLoad("romfs:/gfx/sprites.t3x");
	systemFont= C2D_FontLoad("romfs:/font/0.bcfnt");
	return 0;
}
void Gui::exit(void) {
	if (sprites) {
		C2D_SpriteSheetFree(sprites);
	}

	if (meta_img){
		C2D_SpriteSheetFree(meta_img);
	}

	C2D_TextBufDelete(sizeBuf);
	C2D_Fini();
	C3D_Fini();
}

void set_screen(C3D_RenderTarget * screen) {
	C2D_SceneBegin(screen);
}

void Gui::sprite(int key, int x, int y, float sx, float sy) {
	if (key == sprites_res_null_idx) {
		return;
	} else { // standard case
		C2D_DrawImageAt(C2D_SpriteSheetGetImage(sprites, key), x, y, 0.5f, nullptr, sx, sy);
	}
}
void Gui::spriteTinted(int key, int x, int y, float sx, float sy, u32 color, float tintstrength) {
	C2D_ImageTint tint;
	C2D_PlainImageTint(&tint, color, tintstrength);
	if (key == sprites_res_null_idx) {
		return;
	} else { // standard case
		C2D_DrawImageAt(C2D_SpriteSheetGetImage(sprites, key), x, y, 0.5f, &tint, sx, sy);
	}
}
void Gui::gcls(C3D_RenderTarget * screen, u32 color){
	C2D_TargetClear(screen,color);
}

void Draw_Text(float x, float y, float size, u32 color, const char *text) {
	C2D_Text c2d_text;
	C2D_TextFontParse(&c2d_text, systemFont, sizeBuf, text);
	C2D_TextOptimize(&c2d_text);
	C2D_DrawText(&c2d_text, C2D_WithColor | C2D_AlignLeft, x, y, 0.5f, size, size, color);
}

void DrawStrBox(float x, float y, float size, u32 color, const char *text, float width, float maxwidth = 1) {
	C2D_Text c2d_text;
	C2D_TextFontParse(&c2d_text, systemFont, sizeBuf, text);
	C2D_TextOptimize(&c2d_text);
	C2D_DrawText(&c2d_text, C2D_WithColor, x, y, 0.5f, clamp(size*(width/Draw_GetTextWidth(size, text)),0.001, size * maxwidth), size, color);
}

void Draw_Text_Center(float x, float y, float size, u32 color, const char *text) {
	C2D_Text c2d_text;
	C2D_TextFontParse(&c2d_text, systemFont, sizeBuf, text);
	C2D_TextOptimize(&c2d_text);
	C2D_DrawText(&c2d_text, C2D_WithColor | C2D_AlignCenter, x, y, 0.5f, size, size, color);
}

void DrawStrBoxC(float x, float y, float size, u32 color, const char *text, float width, float maxwidth = 1) {
	C2D_Text c2d_text;
	C2D_TextFontParse(&c2d_text, systemFont, sizeBuf, text);
	C2D_TextOptimize(&c2d_text);
	float temp=clamp(size*(width/Draw_GetTextWidth(size, text)),0.001, size * maxwidth);
	C2D_DrawText(&c2d_text, C2D_WithColor | C2D_AlignCenter, x, y, 0.5f, temp, size, color);
}

void DrawStrBoxCC(float x, float y, float size, u32 color, const char *text, float width, float height) {
	C2D_Text c2d_text;
	C2D_TextFontParse(&c2d_text, systemFont, sizeBuf, text);
	C2D_TextOptimize(&c2d_text);
	float tempx=clamp(size * (width / Draw_GetTextWidth(size, text)), 0.001, size);
	float tempy=clamp(size * (height / Draw_GetTextHeight(size, text)), 0.001, size);
	C2D_DrawText(&c2d_text, C2D_WithColor | C2D_AlignCenter, x, y - Draw_GetTextHeight(tempy, text) / 2, 0.5f, tempx, tempy, color);
}

void Draw_Text_Right(float x, float y, float size, u32 color, const char *text) {
	C2D_Text c2d_text;
	C2D_TextFontParse(&c2d_text, systemFont, sizeBuf, text);
	C2D_TextOptimize(&c2d_text);
	C2D_DrawText(&c2d_text, C2D_WithColor | C2D_AlignRight, x, y, 0.5f, size, size, color);
}

void Draw_GetTextSize(float size, float *width, float *height, const char *text) {
	C2D_Text c2d_text;
	C2D_TextFontParse(&c2d_text, systemFont, sizeBuf, text);
	C2D_TextGetDimensions(&c2d_text, size, size, width, height);
}
float Draw_GetTextWidth(float size, const char *text) {
	float width = 0;
	Draw_GetTextSize(size, &width, NULL, text);
	return width;
}
float Draw_GetTextHeight(float size, const char *text) {
	float height = 0;
	Draw_GetTextSize(size, NULL, &height, text);
	return height;
}
bool Draw_Rect(float x, float y, float w, float h, u32 color) {
	return C2D_DrawRectSolid(x, y, 0.5f, w, h, color);
}
char* stringtochar(std::string str){
	char *buf = &(str[0]);
	sprintf(stringtochar_buf,"%s",buf);
	return stringtochar_buf;
}
float math_abs(float val){
	float res = val;
	if (res<0){res -= val*2;}
	return res;
}
float percvalf(float at0, float at100, float perc){
	return at0*(1-perc)+at100*(perc);
}
int percval(int at0, int at100, float perc){
	return at0*(1-perc)+at100*(perc);
}
float math_pow2(float val){
	return val*val;
}
float _1ddist(float x0,float x1){
	return math_abs(x0-x1);
}
float _2ddist(float x0,float y0,float x1, float y1){
	return std::sqrt(math_pow2(math_abs(x0-x1))+math_pow2(math_abs(y0-y1)));
}
float clamp(float num, float low, float high){
	if (num < low)
		num=low;
	if (num > high)
		num=high;
	return num;
}

void Gui::ScreenLogic(u32 hDown, u32 hHeld, touchPosition touch){
	maincnt++;
	millisec += 1.0f/60.0f;
	touchox = touchpx;
	touchoy = touchpy;
	touchot = touchpt;
	touchpx = touch.px;
	touchpy = touch.py;
	
	if (hDown & KEY_Y){
		top_screen_title="The \ue003 button was pressed!";
		changeTopTitle=true;
	}
	if (hDown & KEY_B){
		top_screen_title="Wait, I can't go back! Just press START instead.";
		changeTopTitle=true;
	}
	if (hDown & KEY_A){
		top_screen_title="There is nothing to select here!";
		changeTopTitle=true;
	}
	if (hDown & KEY_X){
		top_screen_title="Visit new.smilebasicsource.com for more content!!!";
		changeTopTitle=true;
	}
	
	if (hDown & KEY_DOWN){
		disableTopTitle=true;
	}
	
	if (hDown & KEY_UP){
		disableTopTitle=false;
	}
	
	if (hDown & KEY_LEFT){
		disableTopTxtBG=true;
	}
	
	if (hDown & KEY_RIGHT){
		disableTopTxtBG=false;
	}
	
	if (hDown & KEY_L){
		topTextID--;
	}
	
	if (hDown & KEY_R){
		topTextID++;
	}
	
	if (hDown & KEY_START){
		exiting=true;
	}
}
void ScreenLogic_bak(u32 hDown, u32 hHeld, touchPosition touch){
	maincnt++;
	millisec += 1/60;
	touchox = touchpx;
	touchoy = touchpy;
	touchot = touchpt;
	touchpx = touch.px;
	touchpy = touch.py;

	if (touchpx+touchpy!=0){
		touchpt++;
	} else {
		touchpt=0;
	}
	if (hDown & KEY_START){
		exiting=true;
	}

	if (pageid==0 && touchpt && buttonpressed==-1 && touchpx > 60 && touchpx < 220 && touchpy > 24 && touchpy < 88){
		buttonpressed=0; Init::SelSound();
	}

	if (pageid==0 && touchpt && buttonpressed==-1 && touchpx > 60 && touchpx < 220 && touchpy > 94 && touchpy < 158){
		buttonpressed=1; Init::SelSound();
	}

	if (pageid==0 && touchpt && buttonpressed==-1 && touchpx > 60 && touchpx < 220 && touchpy > 164 && touchpy < 228){
		buttonpressed=2; Init::SelSound();
	}

	if (pageid==0 && touchot && !touchpt && buttonpressed==0 && !exiting){
		Init::LaunchSound();
		updateApp("");
		if (errorcode==0){
			exiting=true;
			fadeout=true;
		}
	}

	if (pageid==0 && touchot && !touchpt && buttonpressed==1 && !exiting){
		Init::LaunchSound();
		pageid=1; pagex=500;
		if (meta_total == 0){
			Meta::prepareArrays(getProjectList());
			getImageList();
			if(errorcode){
				showError("prjimg.t3x or prjlist.txt couldn't be downloaded.\n\nRefusing to open download menu.");
				meta_total=0; pageid=0; pagex=0;
			} else {
				meta_img = C2D_SpriteSheetLoad("sdmc:/SB-Download-Station/cache/prjic.t3x");
			}
			deleteFile("sdmc:/SB-Download-Station/cache/prjic.t3x");
		}
		down_max=meta_total*64;
		Init::SwitchSound();
		waiting=false;
	}

	if (pageid==0 && touchot && !touchpt && buttonpressed==2 && !exiting){
		Init::LaunchSound();
	}

	if (pageid==1 && !down_show_props && (hDown & KEY_B) && !exiting){
		Init::BackSound();
		pageid=0; pagex=0;
	}

	if (pageid==1 && down_show_props && buttonpressed==-1 && (touchpx > 172 && touchpy > 450-scy && touchpx < 305 && touchpy < 474 - scy) && !exiting){
		Init::SelSound();
		buttonpressed=3;
	}

	if (pageid==1 && !down_show_props && (hDown & KEY_UP) && down_s && !exiting){
		Init::SelSound();
		down_s--;
	}

	if (pageid==1 && !down_show_props && (hDown & KEY_DOWN) && (down_s < meta_total -1) && !exiting){
		Init::SelSound();
		down_s++;
	}

	if (pageid==1 && down_show_props && (hDown & KEY_B) && !exiting){
		Init::BackSound();
		down_show_props=false;
	}

	if (pageid==1 && down_show_props && buttonpressed!=4 && (hDown & KEY_A)){
		buttonpressed=4;
	}

	if (pageid==1 && !down_show_props && (hDown & KEY_A) && !exiting){
		Init::LaunchSound();
		setMessageText(meta_desc[down_s]);
		down_show_props=true;
	}

	if (pageid==1 && ((touchot && !touchpt && buttonpressed==3) || (!(hHeld & KEY_A) && buttonpressed==4)) && !exiting){
		buttonpressed=-1;
		Init::LaunchSound();
		down_show_props=false; retry=1; ExtDataFolderDeleteDialog=true;
		do {
			performProjectDownload(meta_prjn[down_s]);
		} while (retry);
	}

	if (pageid==1){
		pagey=down_show_props * 240;
	}

	if (buttonpressed!=-1 && touchot && !touchpt){
		buttonpressed=-1;
	}

	if (errorcode){
		sprintf(errorstr,AppErrTbl(errorcode,CFGLang),errorcode);
		Init::WrongSound();
		showError(errorstr,errorcode);
		errorcode=0;
	}

	while((down_s*72 - down_vy) < 32){down_vy--;}
	while((down_s*72 - down_vy) > 100){down_vy++;}
	down_y = (down_y * 3.0f + down_vy)/4.0f;

}

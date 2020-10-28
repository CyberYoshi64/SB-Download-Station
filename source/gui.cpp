#include "animation.hpp"
#include "gui.hpp"
#include "init.hpp"
#include "keyboard.h"
#include "errtbl.hpp"
#include "cia.hpp"
#include "download.hpp"
#include "thread.h"
#include "cyvar.hpp"
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
int touchpt;
int touchpx;
int touchpy;
int touchot;
int touchox;
int touchoy;
int waitictimer;
int waitposx;
int waitposy;
int waitalpha=15;
int maincnt;
int waitmc;
float millisec;
std::string keyboardIn;
char stringtochar_buf[8192];
C2D_TextBuf sizeBuf;
C2D_Font systemFont;
int CFGLang;
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
extern int meta_ver[];
extern int meta_total;
extern u64 jumptarg;

u16 downloadNo=1; /* Initialising the file count with 1, to prevent div by 0 */
u16 downloadedFiles;

int down_y;
size_t down_s;
float down_vy=-240.0f;
int down_max;
bool down_show_props;

bool fadein = true;
bool fadeout;
u8 fadealpha = 250;
u8 fadecolor;
C3D_RenderTarget *top;
C3D_RenderTarget *bottom;
int buttonpressed=-1;

void Gui::clearTextBufs(void) {
	C2D_TextBufClear(sizeBuf);
}
Result Gui::init(void) {
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	C2D_Prepare();

	CYvar::init(1.0f);
	CYvar::aliasing(false);
	CYvar::updateCanvas();

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

void Gui::fadeEffects(void){
	if (fadein && fadealpha>0){
		fadealpha+=-10;
	} else {
		fadein=false;
	}
	if (fadeout && fadealpha<250){
		fadealpha+=10; fadein=false;
	} else {
		fadeout=false;
	}
}

void Gui::sprite(int key, int x, int y, float sx, float sy) {
	if (key == sprites_res_null_idx) {
		return;
	} else { // standard case
		C2D_DrawImageAt(C2D_SpriteSheetGetImage(sprites, key), x * gnrscl, y * gnrscl, 0.5f, nullptr, sx * gnrscl, sy * gnrscl);
	}
}
void Gui::gcls(C3D_RenderTarget * screen, u32 color){
	C2D_TargetClear(screen,color);
}
void Draw_ImageBlend(int key, int x, int y, u32 color, float sx, float sy) {
	C2D_ImageTint tint;
	C2D_SetImageTint(&tint, C2D_TopLeft, color, 1);
	C2D_SetImageTint(&tint, C2D_TopRight, color, 1);
	C2D_SetImageTint(&tint, C2D_BotLeft, color, 1);
	C2D_SetImageTint(&tint, C2D_BotRight, color, 1);
	C2D_DrawImageAt(C2D_SpriteSheetGetImage(sprites, key), x * gnrscl, y * gnrscl, 0.5f, &tint, sx * gnrscl, sy * gnrscl);
}
void displayMsg(const char* text) {
	Gui::clearTextBufs();
	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
	C2D_TargetClear(bottom, TRANSPARENT);
	set_screen(bottom);
	Draw_Rect(16,16,320-32,208,C2D_Color32(255,255,255,192));
	Draw_Text(24, 32, 0.45f, BLACK, text);
	Draw_EndFrame();
}
void displayBottomMsg(const char* text) {
	Gui::clearTextBufs();
	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
	C2D_TargetClear(bottom, TRANSPARENT);
	set_screen(bottom);
	Draw_Rect(16,16,320-32,208,C2D_Color32(255,255,255,192));
	Draw_Text(24, 32, 0.45f, BLACK, text);
	Draw_EndFrame();
}
void Draw_EndFrame(void) {
	C2D_TextBufClear(sizeBuf);
	C3D_FrameEnd(0);
}

void Draw_Text(float x, float y, float size, u32 color, const char *text) {
	x = x * gnrscl;
	y = y * gnrscl;
	size = size * gnrscl;
	C2D_Text c2d_text;
	C2D_TextFontParse(&c2d_text, systemFont, sizeBuf, text);
	C2D_TextOptimize(&c2d_text);
	C2D_DrawText(&c2d_text, C2D_WithColor | C2D_AlignLeft, x, y, 0.5f, size, size, color);
}

void DrawStrBox(float x, float y, float size, u32 color, const char *text, float width, float maxwidth = 1) {
	x = x * gnrscl;
	y = y * gnrscl;
	size = size * gnrscl;
	width = width * gnrscl;
	C2D_Text c2d_text;
	C2D_TextFontParse(&c2d_text, systemFont, sizeBuf, text);
	C2D_TextOptimize(&c2d_text);
	C2D_DrawText(&c2d_text, C2D_WithColor, x, y, 0.5f, clamp(size*(width/Draw_GetTextWidth(size, text)),0.001, size * maxwidth), size, color);
}

void Draw_Text_Center(float x, float y, float size, u32 color, const char *text) {
	x = x * gnrscl;
	y = y * gnrscl;
	size = size * gnrscl;
	C2D_Text c2d_text;
	C2D_TextFontParse(&c2d_text, systemFont, sizeBuf, text);
	C2D_TextOptimize(&c2d_text);
	C2D_DrawText(&c2d_text, C2D_WithColor | C2D_AlignCenter, x, y, 0.5f, size, size, color);
}

void DrawStrBoxC(float x, float y, float size, u32 color, const char *text, float width, float maxwidth = 1) {
	x = x * gnrscl;
	y = y * gnrscl;
	size = size * gnrscl;
	width = width * gnrscl;
	C2D_Text c2d_text;
	C2D_TextFontParse(&c2d_text, systemFont, sizeBuf, text);
	C2D_TextOptimize(&c2d_text);
	float temp=clamp(size*(width/Draw_GetTextWidth(size, text)),0.001, size * maxwidth);
	C2D_DrawText(&c2d_text, C2D_WithColor | C2D_AlignCenter, x, y, 0.5f, temp, size, color);
}

void Draw_Text_Right(float x, float y, float size, u32 color, const char *text) {
	x = x * gnrscl;
	y = y * gnrscl;
	size = size * gnrscl;
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
	return C2D_DrawRectSolid(x * gnrscl, y * gnrscl, 0.5f, w * gnrscl, h * gnrscl, color);
}
void Gui::Waitz(){
	waiting=true; waitmc=0;
	aptSetSleepAllowed(false);
	aptSetHomeAllowed(false);
	while (aptMainLoop()){
		if (!waiting)
			break;
		Gui::clearTextBufs();
		maincnt++;
		waitmc++;
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		set_screen(bottom);
		if (aptCheckHomePressRejected()) waitmc=clamp(waitmc,600,3600);
		if (waitmc > 600 && waitmc < 630){
			Draw_Rect(0,0,320,240,((waitmc-600)*2)<<24);
			Draw_Text_Center(160,176,FONT_SIZE_12,(((waitmc-600)*4)<<24)+16777215,"Please wait...");
		}
		if (waitmc > 1800 && waitmc < 1830){
			Draw_Rect(0,0,320,240,((waitmc-1800)*2)<<24);
			Draw_Text_Center(160,176,FONT_SIZE_12,(((waitmc-1800)*4)<<24)+16777215,"This is taking longer than expected. Sorry.");
		}
		if (waitmc > 3600 && waitmc < 3630){
			Draw_Rect(0,0,320,240,((waitmc-3600)*2)<<24);
			Draw_Text_Center(160,176,FONT_SIZE_12,(((waitmc-3600)*4)<<24)+0xC0D8FF,"Please restart your Nintendo 3DS.");
		}
		if (waitmc > 5400 && waitmc < 5430){
			Draw_Rect(0,0,320,240,((waitmc-5400)*2)<<24);
			Draw_Text_Center(160,176,FONT_SIZE_12,(((waitmc-5400)*4)<<24)+0x80C0FF,"Why are you still here?");
		}
		if (waitmc > 7200 && waitmc < 7230){
			Draw_Rect(0,0,320,240,((waitmc-7200)*2)<<24);
			Draw_Text_Center(160,176,FONT_SIZE_12,(((waitmc-7200)*4)<<24)+0xFFC040,"Niek hat Stern gesagt!");
		}
		if (waitmc > 7560 && waitmc < 7590){
			Draw_Rect(0,0,320,240,((waitmc-7560)*2)<<24);
			if(waitmc==7589) waitmc=600;
		}
		if (waitmc == 599)
			Init::WaitSound();
		sprite(sprites_ic_btn_generic_small_n_idx, 145, 105, 1, 1);

		float rot=(waitictimer/(3.14159265359f*4.0f));
		C2D_DrawImageAtRotated(C2D_SpriteSheetGetImage(sprites, sprites_waiticon_idx), 160.0f * gnrscl, 120.0f * gnrscl, 0.5f, rot);
		waiticon(160,120);
		C3D_FrameEnd(0);
	}
	aptSetSleepAllowed(true);
	aptSetHomeAllowed(true);
	Init::StopWaitSound();
	if (!aptMainLoop()){destroyThreads();}
}
void waiticon(int posx, int posy){
	waitictimer=(waitictimer+1)%2147000000;
}
void waiticonStart(){
	Init::WaitSound();
}
void waiticonEnd(){
	Init::StopWaitSound();
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

void ProjectDownloadThread(){
	animProgBarTimer=0.0f;
	char str[256]; int threadcnt=0; bool shademode=false;
	while(showProgressBar) {
		if (downloadNo < 1) {
			downloadNo = 1;
		}
		snprintf(str, sizeof(str), "%d / %d",downloadedFiles,downloadNo);
		
		if (threadcnt < 160 && !shademode) {
			threadcnt++;
		} else {
			shademode=true;
		}
		
		if (threadcnt > 80 && shademode) {
			threadcnt--;
		} else {
			shademode=false;
		}
		
		Gui::clearTextBufs();
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		set_screen(bottom);
		Draw_Rect(0, 0, 320, 240, C2D_Color32(0, threadcnt/3, threadcnt, 128));
		Draw_Rect(60, 169, 202, 30, 0xff004040);
		Animation::DrawProgressBar(downloadedFiles, downloadNo);
		switch(progressBarType){
			case 0: Draw_Text_Center(160, 40, 0.7f, 0xffffffff, "Downloading project file(s)..."); break;
			case 1: Draw_Text_Center(160, 40, 0.7f, 0xffffffff, "Transferring file to SmileBASIC..."); break;
		}
		Draw_Text_Center(160, 100, 0.65f, 0x40ffffff, str);
		Draw_Text_Center(160, 140, 0.625f, 0x80ffffff, downloadList[downloadedFiles].c_str());
		C3D_FrameEnd(0);
	}
}

void Dialog_DrawContent(float jiff, std::string text, bool buttonno, std::string leftbtn, std::string rightbtn){
	Draw_Rect(32,32+jiff,320-64,240-64,0xE8202830);
	DrawStrBoxC(160, 64+jiff, 0.5f, -1, text.c_str(), 320-80, 1);
	Draw_Rect(32,240-64+jiff,320-64,32,0xFF208090);
}

bool Gui::Dialog(std::string text, u8 flag, bool buttonno, std::string leftbtn, std::string rightbtn){
	float offset=-240.0f; float vel=0.0f; bool hasChosen=false; bool res=true;
	while (!hasChosen or offset < -0.05f){
		Dialog_DrawContent(offset, text, buttonno, leftbtn, rightbtn);
		offset=offset*0.5;
	}
	vel=1/512;
	while (offset < 240.0f){
		vel=vel*2.0f;
		offset += vel;
		Dialog_DrawContent(offset, text, buttonno, leftbtn, rightbtn);
	}
}

void Gui::DrawScreen(){
	bool mainbtn_upd = (buttonpressed == 0);
	bool mainbtn_dwn = (buttonpressed == 1);
	bool mainbtn_sto = (buttonpressed == 2);
	bool instbtn = (buttonpressed == 3 || buttonpressed == 4);
	float startx = 0;
	float tagx = 0;
	set_screen(top);
	gcls(top, 0xFF302018);
	if (scx > -400 && scx < 400){
		Draw_Text_Center(200-scx, 32-scy, FONT_SIZE_14, -1, LngpackStr(LNGTXT_TOPMENU,CFGLang));
	}
	startx += 500;
	if (scx > 100 && scx < 900){
		Draw_Text_Center(startx+200-scx, 32-scy, FONT_SIZE_14, -1, LngpackStr(LNGTXT_DOWNLOAD_PRJ,CFGLang));
	}
	Draw_Rect(0,0,400,240,C2D_Color32(fadecolor,fadecolor,fadecolor,fadealpha));
	startx = 0;
	set_screen(bottom);
	gcls(bottom, 0xFF302018);
	if (scx > -320 && scx < 320){
		Draw_ImageBlend(sprites_ic_update_btn_idx, 61-scx, 26-scy, 0x80000000, 1, 1);
		sprite(sprites_ic_update_btn_idx, 59-scx + mainbtn_upd * 2, 24-scy + mainbtn_upd * 2, 1, 1);
		Draw_Text_Center(160-scx + mainbtn_upd * 2, 50-scy + mainbtn_upd * 2, FONT_SIZE_11,BLACK,LngpackStr(LNGTXT_UPDATE_APP,CFGLang));

		Draw_ImageBlend(sprites_ic_download_btn_idx, 61-scx, 96-scy, 0x80000000, 1, 1);
		sprite(sprites_ic_download_btn_idx, 59-scx + mainbtn_dwn * 2, 94-scy + mainbtn_dwn * 2, 1, 1);
		Draw_Text_Center(160-scx + mainbtn_dwn * 2, 120-scy + mainbtn_dwn * 2, FONT_SIZE_11,BLACK,LngpackStr(LNGTXT_DOWNLOAD_PRJ,CFGLang));

		Draw_ImageBlend(sprites_ic_launch_btn_idx, 61-scx, 166-scy, 0x80000000, 1, 1);
		sprite(sprites_ic_launch_btn_idx, 59-scx + mainbtn_sto * 2, 164-scy + mainbtn_sto * 2, 1, 1);
		Draw_Text_Center(160-scx + mainbtn_sto * 2, 190-scy + mainbtn_sto * 2, FONT_SIZE_11,BLACK,LngpackStr(LNGTXT_STORE_PRJ,CFGLang));

		Draw_Text_Center(160,212+math_abs(scx/8),FONT_SIZE_12,-1,LngpackStr(LNGTXT_EXIT,CFGLang));
	}
	startx += 500;
	if (scx > 180 && scx < 820){
		Draw_Rect(startx-scx,0,320-16,208,0x80000000);
		Draw_Rect(startx+304-scx,0,16,208,0xC0000000);

		if (down_max > 240)
			Draw_Rect(startx+304-scx,32+clamp((down_y/(down_max-240))*144.0f,0.0f,144.0f),16,32,0x60FFEDCB);

		for(size_t down_i = 0; down_i < meta_total; down_i++){
			while(((down_i * 72.0f) - down_y) < -64.0f){down_i++;}
			if(((down_i * 72.0f) - down_y) > 240.0f){break;}
			tagx = 0;
			if (down_s == down_i){
				Draw_ImageBlend(sprites_ic_dwn_button_idx, startx + 8 - scx, 8 + (down_i * 72) - down_y, 0xFF806030, 1, 1);
				if ((down_i * 72 - down_y) > 0.0f && C2D_SpriteSheetCount(meta_img) > down_i) C2D_DrawImageAt(C2D_SpriteSheetGetImage(meta_img, down_i), startx + 16 - scx, 16 + (down_i * 72) - down_y, 0.5f);
				DrawStrBox(startx + 80 - scx, 16 + (down_i * 72) - down_y, FONT_SIZE_12, -1, meta_title[down_i].c_str(), 200);
				DrawStrBox(startx + 80 - scx, 48 + (down_i * 72) - down_y, FONT_SIZE_9, -1, meta_desc[down_i].c_str(), 200);
			} else {
				if ((down_i * 72 - down_y) > 0.0f && C2D_SpriteSheetCount(meta_img) > down_i) C2D_DrawImageAt(C2D_SpriteSheetGetImage(meta_img, down_i), startx + 16 - scx, 16 + (down_i * 72) - down_y, 0.5f);
				Draw_ImageBlend(sprites_ic_dwn_button_idx, startx + 8 - scx, 8 + (down_i * 72) - down_y, 0x80806030, 1, 1);
				DrawStrBox(startx + 80 - scx, 16 + (down_i * 72) - down_y, FONT_SIZE_12, 0x88FFFFFF, meta_title[down_i].c_str(), 200);
				DrawStrBox(startx + 80 - scx, 48 + (down_i * 72) - down_y, FONT_SIZE_9, 0x88FFFFFF, meta_desc[down_i].c_str(), 200);
			}
		}

		sprite(sprites_ic_title_bar_idx,startx-scx,0,1,1);
		DrawStrBox(startx-scx+8,4,0.75f,-1,"Download menu", 304, 1.0625);

		if (scy > 1){
			Draw_Rect(startx-scx, 240-scy, 320, 240, 0xF0402010);
			DrawStrBox(startx+12-scx, 252 - scy, 0.75, -1, meta_title[down_s].c_str(), 296);
			drawMessageText(startx+12-scx, 280 - scy, 0);
		}

		Draw_Rect(startx-scx,204,320,36,0xE0887654);
		sprite(sprites_ic_btn_blue_n_idx + instbtn, startx + 160 - scx, 268 - scy/4, 1, 1);
		DrawStrBoxC(startx + 240 - scx, 272 - scy/4 + instbtn * 4, 0.5f, instbtn ? GRAY : -1, " Download", 140, 1);
	}
	Draw_Rect(0,0,320,240,C2D_Color32(fadecolor,fadecolor,fadecolor,fadealpha));
}

void Gui::ScreenLogic(u32 hDown, u32 hHeld, touchPosition touch){
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
	if ((hDown & KEY_START) && fadealpha < 128){
		exiting=true;
		fadeout=true;		archiveCommitSaveData("extdata"); archiveUnmount("extdata");

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
		createThread((ThreadFunc)Waitz);
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
		buttonpressed=0;
		Init::LaunchSound();
		down_show_props=false;
		performProjectDownload(meta_prjn[down_s]);
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

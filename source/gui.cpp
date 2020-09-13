#include "gui.hpp"
#include "init.hpp"
#include "keyboard.h"
#include "errtbl.hpp"
#include "download.hpp"
#include <assert.h>
#include <stdarg.h>
#include <unistd.h>
#include <cmath>
static C2D_SpriteSheet sprites;
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
float millisec;
std::string keyboardIn;
char stringtochar_buf[8192];
C2D_TextBuf sizeBuf;
C2D_Font systemFont;
	int CFGLang;
	extern bool exiting;
	extern bool waiting;
	extern int pageid;
bool downloaded;
bool downloading;
bool fadein = true;
bool fadeout;
u8 fadealpha = 250;
u8 fadecolor;
C3D_RenderTarget *top;
C3D_RenderTarget *bottom;
int buttonpressed=-1;
int page;

void Gui::clearTextBufs(void) {
	C2D_TextBufClear(sizeBuf);
}
Result Gui::init(void) {
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	C2D_Prepare();
	top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
	bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
	sizeBuf = C2D_TextBufNew(8192);
	sprites   = C2D_SpriteSheetLoad("romfs:/gfx/sprites.t3x");
	systemFont= C2D_FontLoadSystem(CFG_REGION_EUR);
	return 0;
}
void Gui::exit(void) {
	if (sprites) {
		C2D_SpriteSheetFree(sprites);
	}
	C2D_TextBufDelete(sizeBuf);
	C2D_Fini();
	C3D_Fini();
}
void set_screen(C3D_RenderTarget * screen) {
	C2D_SceneBegin(screen);
}
void Gui::DrawScreen(float scx, float scy, int pagex){
	page = pagex;
	bool mainbtn_upd = (buttonpressed == 0);
	bool mainbtn_dwn = (buttonpressed == 1);
	bool mainbtn_sto = (buttonpressed == 2);
	set_screen(top);
	gcls(top, 0xff403018);
	if (scx > -500 && scx < 500){
		Draw_Text_Center(160-scx, 32-scy, FONT_SIZE_14, -1, LngpackStr(LNGTXT_TOPMENU,CFGLang));
	}
	Draw_Rect(0,0,400,240,C2D_Color32(fadecolor,fadecolor,fadecolor,fadealpha));
	set_screen(bottom);
	gcls(bottom, 0xff403018);
	if (scx > -500 && scx < 500){
		Draw_ImageBlend(sprites_ic_update_btn_idx, 61-scx, 26-scy, 0x80000000, 1, 1);
		sprite(sprites_ic_update_btn_idx, 59-scx + mainbtn_upd * 2, 24-scy + mainbtn_upd * 2, 1, 1);
		Draw_Text_Center(160-scx + mainbtn_upd * 2, 50-scy + mainbtn_upd * 2, FONT_SIZE_11,BLACK,LngpackStr(LNGTXT_UPDATE_APP,CFGLang));
		
		Draw_ImageBlend(sprites_ic_download_btn_idx, 61-scx, 96-scy, 0x80000000, 1, 1);
		sprite(sprites_ic_download_btn_idx, 59-scx + mainbtn_dwn * 2, 94-scy + mainbtn_dwn * 2, 1, 1);
		Draw_Text_Center(160-scx + mainbtn_dwn * 2, 120-scy + mainbtn_dwn * 2, FONT_SIZE_11,BLACK,LngpackStr(LNGTXT_DOWNLOAD_PRJ,CFGLang));
		
		Draw_ImageBlend(sprites_ic_launch_btn_idx, 61-scx, 166-scy, 0x80000000, 1, 1);
		sprite(sprites_ic_launch_btn_idx, 59-scx + mainbtn_sto * 2, 164-scy + mainbtn_sto * 2, 1, 1);
		Draw_Text_Center(160-scx + mainbtn_sto * 2, 190-scy + mainbtn_sto * 2, FONT_SIZE_11,BLACK,LngpackStr(LNGTXT_STORE_PRJ,CFGLang));
		
		Draw_Text_Center(160-scx,212-scy,FONT_SIZE_12,-1,LngpackStr(LNGTXT_EXIT,CFGLang));
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
		fadeout=true;
	}
	
	if (page==0 && touchpt && buttonpressed==-1 && touchpx > 60 && touchpx < 220 && touchpy > 24 && touchpy < 88){
		buttonpressed=0; Init::SelSound();
	}
	
	if (page==0 && touchpt && buttonpressed==-1 && touchpx > 60 && touchpx < 220 && touchpy > 94 && touchpy < 158){
		buttonpressed=1; Init::SelSound();
	}
	
	if (page==0 && touchpt && buttonpressed==-1 && touchpx > 60 && touchpx < 220 && touchpy > 164 && touchpy < 228){
		buttonpressed=2; Init::SelSound();
	}
	
	if (page==0 && touchot && !touchpt && buttonpressed==0){
		Init::LaunchSound();
		updateApp("");
	}
	
	if (page==0 && touchot && !touchpt && buttonpressed==1){
		Init::LaunchSound();
		showError("This feature is not yet implemented.");
	}
	
	if (page==0 && touchot && !touchpt && buttonpressed==2){
		Init::LaunchSound();
		showError("This feature is not yet implemented.");
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
		C2D_DrawImageAt(C2D_SpriteSheetGetImage(sprites, key), x, y, 0.5f, nullptr, sx, sy);
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
	C2D_DrawImageAt(C2D_SpriteSheetGetImage(sprites, key), x, y, 0.5f, &tint, sx, sy);
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
	C2D_Text c2d_text;
	C2D_TextFontParse(&c2d_text, systemFont, sizeBuf, text);
	C2D_TextOptimize(&c2d_text);
	C2D_DrawText(&c2d_text, C2D_WithColor, x, y, 0.5f, size, size, color);
}
void Draw_Textf(float x, float y, float size, u32 color, const char* text, ...) {
	char buffer[256];
	va_list args;
	va_start(args, text);
	vsnprintf(buffer, 256, text, args);
	Draw_Text(x, y, size, color, buffer);
	va_end(args);
}
void Draw_Text_Center(float x, float y, float size, u32 color, const char *text) {
	C2D_Text c2d_text;
	C2D_TextFontParse(&c2d_text, systemFont, sizeBuf, text);
	C2D_TextOptimize(&c2d_text);
	C2D_DrawText(&c2d_text, C2D_WithColor, x-Draw_GetTextWidth(size, text)/2, y, 0.5f, size, size, color);
}
void Draw_Textf_Center(float x, float y, float size, u32 color, const char* text, ...) {
	char buffer[256];
	va_list args;
	va_start(args, text);
	vsnprintf(buffer, 256, text, args);
	Draw_Text(x-Draw_GetTextWidth(size, buffer)/2, y, size, color, buffer);
	va_end(args);
}
void Draw_Text_Right(float x, float y, float size, u32 color, const char *text) {
	C2D_Text c2d_text;
	C2D_TextFontParse(&c2d_text, systemFont, sizeBuf, text);
	C2D_TextOptimize(&c2d_text);
	C2D_DrawText(&c2d_text, C2D_WithColor, x-Draw_GetTextWidth(size, text), y, 0.5f, size, size, color);
}
void Draw_Textf_Right(float x, float y, float size, u32 color, const char* text, ...) {
	char buffer[256];
	va_list args;
	va_start(args, text);
	vsnprintf(buffer, 256, text, args);
	Draw_Text(x-Draw_GetTextWidth(size, buffer), y, size, color, buffer);
	va_end(args);
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
void waiticon(int posx, int posy){
	waitposx=posx-8;
	waitposy=posy-8;
	if ((maincnt % 4)==0){
		waitictimer=(waitictimer+1)%8;
	}
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

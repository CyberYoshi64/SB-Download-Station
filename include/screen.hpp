#ifndef SCREEN_TOP_HPP
#define SCREEN_TOP_HPP

#define SCRTOPNOTIF_SFX_NONE	0
#define SCRTOPNOTIF_SFX_ERROR	1
#define SCRTOPNOTIF_SFX_WARNING	2
#define SCRTOPNOTIF_SFX_MENU	3
#define SCRTOPNOTIF_SFX_WRONG	4

#include "common.hpp"

extern C2D_SpriteSheet gfx_scrbg;
extern C2D_SpriteSheet gfx_button;
extern C2D_SpriteSheet gfx_common;
extern C2D_SpriteSheet gfx_cy64;
extern C2D_SpriteSheet gfx_ghud;
extern C2D_SpriteSheet gfx_appdlg;
extern C2D_SpriteSheet meta_img;

extern bool drawingScreen;
extern bool stopScreenUpdate;
extern bool dspfirmfound;
extern char errorstr[];
extern int errorcode;
extern int touchpt, touchpx, touchpy;
extern int touchot, touchox, touchoy;
extern int waitictimer;
extern int waitposx, waitposy, waitalpha;
extern int maincnt, waitmc;
extern float millisec;
extern std::string keyboardIn;
extern char stringtochar_buf[];
extern C2D_TextBuf sizeBuf;
extern C2D_Font systemFont;
extern int CFGLang;
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
extern u64 jumptarg;
extern bool ExtDataFolderDeleteDialog;
extern int retry;
extern u16 downloadNo;
extern u16 downloadedFiles;
extern int down_y;
extern size_t down_s;
extern float down_vy;
extern int down_max;
extern bool down_show_props;
extern bool fadein;
extern bool fadeout;
extern u8 fadealpha;
extern u8 fadecolor;
extern C3D_RenderTarget *top;
extern C3D_RenderTarget *bottom;
extern int buttonpressed;
extern float fadea;
extern u8 fader;
extern u8 fadeg;
extern u8 fadeb;

namespace Screen {
	void Thread(void);
}

#endif

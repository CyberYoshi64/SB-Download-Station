#include "common.hpp"
#include "download.hpp"
#include "inifile.h"
#include "init.hpp"
#include "keyboard.h"
#include "sound.h"
#include "thread.h"
#include <3ds.h>
#include <dirent.h>
#include <unistd.h>
#include "dialog.hpp"

u8 batteryLevel;
u8 soundSliderLvl;
bool wifiConnected;
u32 wifiConnectedVal;
u8 wifiStrength;
bool headSetUsed;
u8 mcureg0F;
char tmpstr[4096];
std::string top_screen_title = "";
bool changeTopTitle = true;
bool disableTopTitle;
bool disableTopTxtBG;
bool wifiStrengthLow;
bool batBelow0;
bool batBelow1;
bool batBelow0c;
bool batBelow1c;
bool batCharge;
bool batChargeS;
bool batChargeE;
bool dspfirmfoundF;
u64 topTextID = 1;
std::string hot_potato_toptitle = "";
float topTitleAnimTime = 1.0f;
float topTextBGAnimTime = 1.0f;
float topNotifAnimTime = 0.0f;
u64 topNotificationTimer = 0;
u32 topNotificationColor = 0;
u8 topNotificationSfx = 0;
std::string topNotificationText = "";
u8 topNotif_red = 0;
u8 topNotif_grn = 0;
u8 topNotif_blu = 0;
u8 topNotif_alp = 0;
u64 topNotifTimer = 0;
std::string hot_potato_topnotif = "";

void deviceMkEvent(){
	if (batBelow0 && !batCharge){
		topNotificationTimer = 300;
		topNotificationColor = 0xFFA000FF;
		topNotificationText = "Warning! The battery is below 10%!";
		topNotificationSfx = SCRTOPNOTIF_SFX_WARNING;
	}
	if (batBelow1 && !batCharge){
		topNotificationTimer = 600;
		topNotificationColor = 0xFF4000FF;
		topNotificationText = "Please charge your console now.";
		topNotificationSfx = SCRTOPNOTIF_SFX_ERROR;
	}
	if (batChargeS){
		topNotificationTimer = 180;
		topNotificationColor = 0x4060FFC0;
		topNotificationText = "Charging battery... "+std::to_string(batteryLevel)+"%";
		topNotificationSfx = SCRTOPNOTIF_SFX_MENU;
	}
	if (batChargeE && (batteryLevel < 100)){
		topNotificationTimer = 300;
		topNotificationColor = 0xFF8040FF;
		topNotificationText = "The charging cable was unexpectedly unplugged.";
		topNotificationSfx = SCRTOPNOTIF_SFX_WARNING;
	}
	if (wifiStrengthLow){
		topNotificationTimer = 300;
		topNotificationColor = 0xFF8000FF;
		topNotificationText = "Wi-Fi strength is weak. Please get closer to the access point.";
		topNotificationSfx = SCRTOPNOTIF_SFX_WARNING;
	}
	if (!dspfirmfound && !dspfirmfoundF) {
		topNotificationTimer = 900;
		topNotificationColor = 0xFF0000FF;
		topNotificationText = "The sound output has been disabled.";
		dspfirmfoundF = true;
	}
}

void drawTextInBodyWithID(u64 id){
	switch (id){
		case 0:
			sprintf(tmpstr, \
			"Battery voltage: %d\n" \
			"Sound slider: %d\n" \
			"Wifi Connected? %d\n"\
			"Strength: %d\n"\
			"Headset plugged in? %d\n"\
			"Charging!?: %s",\
			batteryLevel, soundSliderLvl, wifiConnected, wifiStrength, headSetUsed, batCharge ? "true" : "false");
			DrawStrBoxCC(200, 121, FONT_SIZE_12, C2D_Color32f(1,1,1,topTextBGAnimTime), tmpstr, 360, 136);
			break;
		case 1:
			DrawStrBoxCC(200, 121, FONT_SIZE_12, C2D_Color32f(1,1,1,topTextBGAnimTime), LngpackStr(LNGTXT_MAIN_MENU_TXT, CFGLang), 360, 136);
			break;
		default:
			sprintf(tmpstr, "Warning!\n\nAn invalid ID was specified:\n%016llx", topTextID);
			DrawStrBoxCC(200, 121, FONT_SIZE_14, C2D_Color32f(1,0.5f,0,topTextBGAnimTime), tmpstr, 360, 136);
			break;
	}
}

void topNotificationPlaySfx(){
	switch(topNotificationSfx){
		case SCRTOPNOTIF_SFX_ERROR: Init::ErrorSound(); break;
		case SCRTOPNOTIF_SFX_WARNING: Init::WarningSound(); break;
		case SCRTOPNOTIF_SFX_MENU: Init::MenuSound(); break;
		case SCRTOPNOTIF_SFX_WRONG: Init::WrongSound(); break;
	}
}

void TopScr(){
	Gui::sprite(&gfx_scrbg, scrbg_top_idx, 0, 0, 1, 1);
	if (topNotifAnimTime < 0.001f){
		if (topNotificationText != ""){
			topNotifTimer = topNotificationTimer - 1;
			hot_potato_topnotif = topNotificationText;
			topNotif_red = (topNotificationColor >> 24) & 255;
			topNotif_grn = (topNotificationColor >> 16) & 255;
			topNotif_blu = (topNotificationColor >>  8) & 255;
			topNotif_alp = (topNotificationColor >>  0) & 255;
			topNotificationTimer= 0;
			topNotificationText = "";
			topNotificationPlaySfx();
			topNotificationSfx  = 0;
		}
	} else if (topNotifTimer != 0) {
		topNotifTimer--;
	}
	
	if (changeTopTitle && topTitleAnimTime < 0.001f){
		hot_potato_toptitle=top_screen_title;
		changeTopTitle=false;
		Init::SwitchSound();
	}
	
	if (topTitleAnimTime > 0.0009f){
		Gui::sprite(&gfx_common,common_ic_div_l_idx, 200.0f - topTitleAnimTime * 200.0f, 56.0f - topTitleAnimTime * 8.0f, topTitleAnimTime, topTitleAnimTime);
		DrawStrBoxC(200, 32.0f - topTitleAnimTime * 8.0f, FONT_SIZE_18, C2D_Color32f(1,1,1,topTitleAnimTime), hot_potato_toptitle.c_str(), 360, 1);
	}
	
	if (topNotifAnimTime > 0.0009f){
		Gui::spriteTinted(&gfx_ghud,ghud_ic_bg_gui_top_msg_idx, 0, 200, 1, 1, C2D_Color32(topNotif_red, topNotif_grn, topNotif_blu, topNotifAnimTime * topNotif_alp), 1);
		DrawStrBox(8, 224.0f - topNotifAnimTime * 8.0f, 0.5f, C2D_Color32f(1,1,1,topNotifAnimTime), hot_potato_topnotif.c_str(), 320, 1);
	}

	if (topTextBGAnimTime > 0.005f){
		Gui::spriteTinted(&gfx_common,common_ic_textbody_idx, 24, 60, 1, 1, C2D_Color32f(1,1,1,topTextBGAnimTime),0.0f);
		drawTextInBodyWithID(topTextID);
	}
	
	topTitleAnimTime=(topTitleAnimTime * 2.0f + (float)(!changeTopTitle && !disableTopTitle)) / 3.0f;
	switch(!!topNotificationTimer){
		case 0: topNotifAnimTime=(topNotifAnimTime * 5.0f + (float)(!!topNotifTimer)) / 6.0f; break;
		case 1: topNotifAnimTime=topNotifAnimTime * 0.5f; break;
	}
	topTextBGAnimTime=(topTextBGAnimTime * 2.0f + (float)(!disableTopTxtBG)) / 3.0f;
}

void BotScr(){
	Gui::sprite(&gfx_scrbg,scrbg_bottom_idx, 0, 0, 1, 1);
	Gui::sprite(&gfx_common,common_ic_div_m_idx, 0, 48, 1, 1);
	Gui::sprite(&gfx_common,common_ic_textbody_idx, 24, 60, 0.8, 1);
	DrawStrBoxC(160, 24, FONT_SIZE_11, -1, "Warning!\n\nThis version of SmileBASIC Download Station\nis currently just a dummy and will not do\nanything. lmao.\n\n\uE000: Dialog test\u3000\u3000\uE001/\uE002: Notification test\n\uE006: Top screen heading & body test\n\nSorry for the inconvenience.", 240, 1.2f);
}

void Screen::Thread(){
	fadea=1.0f; gspWaitForVBlank();
	if (errorcode){
		showError(AppErrTbl(errorcode, 1),errorcode);
	}
	while (((!exiting && !aptShouldJumpToHome()) || (exiting && fadea<0.99f)) && !aptShouldClose()){
		MCUHWC_ReadRegister(0xf,&mcureg0F,1);
		batChargeS = ((mcureg0F >> 4) & 1) && !batCharge;
		batChargeE = !((mcureg0F >> 4) & 1) && batCharge;
		batCharge = (mcureg0F >> 4) & 1;
		MCUHWC_GetBatteryLevel(&batteryLevel);
		MCUHWC_GetSoundSliderLevel(&soundSliderLvl);
		ACU_GetStatus(&wifiConnectedVal);
		wifiConnected=(bool)(wifiConnectedVal==3);
		wifiStrengthLow = (osGetWifiStrength() < 2 && wifiStrength >= 3);
		wifiStrength=osGetWifiStrength();
		headSetUsed=osIsHeadsetConnected();
		batBelow0 = ((batteryLevel <= 10) && !batBelow0c);
		batBelow1 = ((batteryLevel <= 5) && !batBelow1c);
		batBelow0c = batteryLevel <= 10;
		batBelow1c = batteryLevel <= 5;
		
		deviceMkEvent();
		
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		Gui::clearTextBufs();
		
		// top screen
		set_screen(top);
		TopScr();
		
		if (exiting){
			Draw_Rect(0,0,400,240,C2D_Color32(fader,fadeg,fadeb,fadea * 255.0f));
		}
		
		// bottom screen
		set_screen(bottom);
		BotScr();
		
		if (Dialog::IsDialogShown()) {
			Dialog::Display();
		}

		if (exiting){
			Draw_Rect(0,0,320,240,C2D_Color32(fader,fadeg,fadeb,fadea * 255.0f));
		}
		
		
		C3D_FrameEnd(0);
		if (errorcode){
			showError(AppErrTbl(errorcode, CFGLang),errorcode);
			errorcode = 0;
		}
		Gui::_3dsAppletEvent();
		fadea=(fadea * 3.0f + ((exiting || aptShouldJumpToHome()) * 1.0f)) / 4.0f;
	}
	stopScreenUpdate=aptShouldJumpToHome();
}

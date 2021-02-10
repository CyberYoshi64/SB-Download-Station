#include "common.hpp"
#include "errtbl.hpp"

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
bool disableTopTitle = false;
bool disableTopTxtBG = false;
bool batBelow15 = false;
bool batBelow5 = false;
bool batBelow15f = false;
bool batBelow5f = false;
bool batCharge = false;
bool batChargef = false;
u64 topTextID = 1;
std::string hot_potato_toptitle = "";
float topTitleAnimTime = 1.0f;
float topTextBGAnimTime = 1.0f;
float topNotifAnimTime = 0.0f;

u64 topNotificationTimer = 0;
u32 topNotificationColor = 0;
std::string topNotificationText = "";

u8 topNotif_red = 0;
u8 topNotif_grn = 0;
u8 topNotif_blu = 0;
u8 topNotif_alp = 0;
u64 topNotifTimer = 0;
std::string hot_potato_topnotif = "";

void deviceMkEvent(){
	if (batBelow15 && !batCharge){
		topNotificationTimer = 180;
		topNotificationColor = 0xFFA000FF;
		topNotificationText = "Warning! The battery is below 15%!";
	}
	if (batBelow5 && !batCharge){
		topNotificationTimer = 300;
		topNotificationColor = 0xFF4000FF;
		topNotificationText = "Please charge your console now.";
	}
}

void drawTextInBodyWithID(u64 id){
	switch (id){
		case 0:
			sprintf(tmpstr, "Battery voltage: %d\nSound slider: %d\nWifi Connected? %d\nStrength: %d\nHeadset plugged in? %d\n\nCharging!?: %s", batteryLevel, soundSliderLvl, wifiConnected, wifiStrength, headSetUsed, batCharge ? "true" : "false");
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

void TopScr(){
	
	Gui::sprite(sprites_ic_bg_scr_top_idx, 0, 0, 1, 1);
	
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
		Gui::sprite(sprites_ic_div_l_idx, 200.0f - topTitleAnimTime * 200.0f, 56.0f - topTitleAnimTime * 8.0f, topTitleAnimTime, topTitleAnimTime);
		DrawStrBoxC(200, 32.0f - topTitleAnimTime * 8.0f, FONT_SIZE_18, C2D_Color32f(1,1,1,topTitleAnimTime), hot_potato_toptitle.c_str(), 360, 1);
	}
	
	if (topTextBGAnimTime > 0.005f){
		Gui::spriteTinted(sprites_ic_textbody_idx, 24, 60, 1, 1, C2D_Color32f(1,1,1,topTextBGAnimTime),0.0f);
		drawTextInBodyWithID(topTextID);
	}
	
	if (topNotifAnimTime > 0.0009f){
		Gui::spriteTinted(sprites_ic_bg_gui_top_msg_idx, 0, 192, 1, 1, C2D_Color32(topNotif_red, topNotif_grn, topNotif_blu, topNotifAnimTime * topNotif_alp), 1);
		DrawStrBox(8, 224.0f - topNotifAnimTime * 8.0f, 0.5f, C2D_Color32f(1,1,1,topNotifAnimTime), hot_potato_topnotif.c_str(), 320, 1);
	}
	
	topTitleAnimTime=(topTitleAnimTime * 2.0f + (float)(!changeTopTitle && !disableTopTitle)) / 3.0f;
	topNotifAnimTime=(topNotifAnimTime * 5.0f + (float)(!!topNotifTimer && !topNotificationTimer)) / 6.0f;
	topTextBGAnimTime=(topTextBGAnimTime * 2.0f + (float)(!disableTopTxtBG)) / 3.0f;
}

void BotScr(){
	Gui::sprite(sprites_ic_bg_scr_bot_idx, 0, 0, 1, 1);
	Gui::sprite(sprites_ic_div_m_idx, 0, 48, 1, 1);
	Gui::sprite(sprites_ic_textbody_idx, 24, 60, 0.8, 1);
	DrawStrBoxC(160, 24, FONT_SIZE_11, -1, "Warning!\n\nThis version of SmileBASIC Download Station\nis currently just a dummy and will not do\nanything.\n\nlmao.\n\n\n\nSorry for the inconvenience.", 240, 1.2f);
}

void Screen::Thread(){
	fadea=1.0f;
	if (errorcode){
		showError(AppErrTbl(errorcode, 1),errorcode);
	}
	while ((!exiting && !aptShouldJumpToHome()) || ((exiting || aptShouldJumpToHome()) && fadea<0.99f)){
		MCUHWC_ReadRegister(0xf,&mcureg0F,1);
		batCharge = (mcureg0F >> 4) & 1;
		MCUHWC_GetBatteryLevel(&batteryLevel);
		MCUHWC_GetSoundSliderLevel(&soundSliderLvl);
		ACU_GetStatus(&wifiConnectedVal);
		wifiConnected=(bool)(wifiConnectedVal==3);
		wifiStrength=osGetWifiStrength();
		headSetUsed=osIsHeadsetConnected();
		batBelow15f = batteryLevel <= 15;
		batBelow5f = batteryLevel <= 5;
		batBelow15 = (!(batteryLevel <= 15) != !batBelow15f);
		batBelow5 = (!(batteryLevel <= 5) != !batBelow5f);
		
		deviceMkEvent();
		
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		Gui::clearTextBufs();
		
		// top screen
		set_screen(top);
		TopScr();
		
		if (exiting){
			Draw_Rect(0,0,400,240,C2D_Color32(fader,fadeg,fadeb,fadea * 255.0f));
		} else {
			Draw_Rect(0,0,400,240,C2D_Color32(fader,fadeg,fadeb,fadea * 128.0f));
			Draw_Rect(0,100,400,40,C2D_Color32(fader,fadeg,fadeb,fadea * 200.0f));
			DrawStrBoxC(200, 104, FONT_SIZE_12, C2D_Color32f(1,1,1,fadea * 1.0f), "Please don't disconnect from the access point.", 360, 1);
			DrawStrBoxC(200, 120, FONT_SIZE_12, C2D_Color32f(1,1,1,fadea * 0.6f), "Doing so could corrupt all of your projects.", 360, 1);
		}
		
		// bottom screen
		set_screen(bottom);
		BotScr();
		
		if (exiting){
			Draw_Rect(0,0,320,240,C2D_Color32(fader,fadeg,fadeb,fadea * 255.0f));
		} else {
			Draw_Rect(0,0,320,240,C2D_Color32(fader,fadeg,fadeb,fadea * 128.0f));
		}
		
		
		C3D_FrameEnd(0);
		if (errorcode){
			showError(AppErrTbl(errorcode, 1),errorcode);
		}
		fadea=(fadea * 3.0f + ((exiting || aptShouldJumpToHome()) * 1.0f)) / 4.0f;
	}
	stopScreenUpdate=aptShouldJumpToHome();
}

#include "dialog.hpp"

extern bool stopScreenUpdate;
bool dialogShown;
float dialogVisualScale;
float dialogVisualOpacity;
float dialogWaitSpin=0.0f;
u8 dialogRes=255;
std::string dialogMainString;
std::vector<std::string> dialogBodyText;
std::string dialogAbtn; bool dialogAbtnf;
std::string dialogBbtn; bool dialogBbtnf;
bool dialogAbtnPress, dialogBbtnPress;
C2D_ImageTint dialogdeftint;
u32 dialogBtnHeld,dialogBtnDown;
bool* dialogWaitForThisBool;
bool dialogWaitForBoolFlag, dialogWaitingForBool;
char dialogDmyStr[1024];

void Dialog::Clear(void){
	dialogVisualScale=0.0f; dialogVisualOpacity=0.0f;
	dialogShown=false; dialogWaitingForBool=false;
	dialogAbtnPress=false; dialogBbtnPress=false;
	dialogAbtn=""; dialogAbtnf=false;
	dialogBbtn=""; dialogBbtnf=false;
	dialogMainString=""; dialogBodyText.clear();
}
void Dialog::WaitForBool(bool* pnt, bool flg){
	dialogWaitForThisBool=pnt; dialogWaitForBoolFlag=flg;
	dialogWaitingForBool=true;
}
void Dialog::MainWGiven(std::string maintxt){
	dialogMainString=maintxt;
}
void Dialog::MainWithID(u32 id){
	dialogMainString="ID not found, lmao";
}
void Dialog::UseAButtonWGiven(std::string btnstr){
	dialogAbtn=btnstr; dialogAbtnf=true;
}
void Dialog::UseBButtonWGiven(std::string btnstr){
	dialogBbtn=btnstr; dialogBbtnf=true;
}
void Dialog::UseAButtonWithID(u32 id){
	dialogAbtn="ID not found"; dialogAbtnf=true;
}
void Dialog::UseBButtonWithID(u32 id){
	dialogBbtn="ID not found"; dialogBbtnf=true;
}
void Dialog::PrepareDisplay(){
	u8 vecidx=0; bool forceLB = false;
	std::string subs="",osbs=""; size_t i=0,j=0;
	dialogAbtn="\uE000 "+dialogAbtn; dialogBbtn="\uE001 "+dialogBbtn;
	dialogBodyText.push_back(""); dialogMainString+=' ';
	for (i=0; i < dialogMainString.length(); i++){
		osbs=subs; if (subs != "") subs += ' ';
		for (j=i; j < dialogMainString.length(); j++){
			if (dialogMainString[j] == ' '){
				break;
			} else if (dialogMainString[j] > 0 && dialogMainString[j] < 32){
				forceLB=true; break;
			} else {
				subs += dialogMainString[j];
			}
		}
		if(Draw_GetTextWidth(0.6f, subs.c_str()) > 240.0f && !forceLB) {subs=osbs; forceLB=1; i--;} else {i=j;if (j >= dialogMainString.length()) break;}
		if (forceLB){
			dialogBodyText[vecidx]=subs;
			vecidx++; subs=""; forceLB=false;
			dialogBodyText.push_back("");
		}
		if (j >= dialogMainString.length()) break;
	}
	if (subs != ""){
		dialogBodyText[vecidx]=subs;
	}
}
bool Dialog::IsDialogShown(){
	return dialogShown;
}

u8 Dialog::Show(){
	dialogRes=255; dialogShown=true;
	touchPosition touch;
	bool exitConfirmed = false;
	bool A_btn_acknowledge = false;
	bool B_btn_acknowledge = false;
	u16 tx=0,ty=0; bool tt=0;
	u16 tox=0,toy=0; bool tot=0;
	if (dialogWaitingForBool) Init::DlgWaitSound();
	while (dialogShown && aptMainLoop() && !aptShouldClose()) {
		if (dialogAbtnf || dialogBbtnf){
			hidScanInput();
			dialogBtnHeld = hidKeysHeld();
			dialogBtnDown = hidKeysDown();
			hidTouchRead(&touch); tot=tt; tox=tx; toy=ty;
			tx=touch.px; ty=touch.py; tt=!(tx == 0 && ty == 0);
			if (dialogRes != 255) {
				dialogAbtnPress=false; dialogBbtnPress=false;
			}
			if (dialogRes == 255 && dialogVisualOpacity > 0.95f){
				if (A_btn_acknowledge){dialogRes=1;}
				if (B_btn_acknowledge){dialogRes=0;}
				if (!tt&&!tot){
					if ((dialogBtnDown & KEY_A) && dialogAbtnf){A_btn_acknowledge = true; Init::SelSound();}
					if ((dialogBtnDown & KEY_B) && dialogBbtnf){B_btn_acknowledge = true; Init::SelSound();}
				}
				if (!exitConfirmed && dialogRes != 255){
					if (dialogRes == 0) Init::BackSound();
					if (dialogRes == 1) Init::OkSound();
					exitConfirmed=true;
				}
				dialogAbtnPress = A_btn_acknowledge;
				dialogBbtnPress = B_btn_acknowledge;
				if(tt && dialogBbtnf && TouchedHitbox(tt,tx,ty,30,170,130 + 130 * (!dialogAbtnf),45)){
					if (!tot || !TouchedHitbox(tot,tox,toy,30,170,130 + 130 * (!dialogAbtnf),45))
						Init::SelSound();
					dialogBbtnPress=true;
				} else if (!tt&&TouchedHitbox(tot,tox,toy,30,170,130 + 130 * (!dialogAbtnf),45)){
					B_btn_acknowledge = true;
				}
				if(tt && dialogAbtnf && TouchedHitbox(tt,tx,ty,160 - 130 * (!dialogBbtnf),170,130 + 130 * (!dialogBbtnf),45)){
					if (!tot || !TouchedHitbox(tot,tox,toy,160 - 130 * (!dialogBbtnf),170,130 + 130 * (!dialogBbtnf),45))
						Init::SelSound();
					dialogAbtnPress=true;
				} else if (!tt&&TouchedHitbox(tot,tox,toy,160 - 130 * (!dialogBbtnf),170,130 + 130 * (!dialogBbtnf),45)){
					A_btn_acknowledge = true;
				}
			}
		}
		if (dialogWaitingForBool && (*dialogWaitForThisBool == dialogWaitForBoolFlag)) {dialogRes=2; Init::StopDlgWaitSound();}
		if (stopScreenUpdate && !aptShouldJumpToHome()){
			stopScreenUpdate=false;
			createThread((ThreadFunc)Screen::Thread);
		}
		dialogVisualOpacity=(dialogVisualOpacity * 3.0f + (float)((dialogRes == 255) * 1.0f)) / 4.0f;
		dialogVisualScale = (dialogRes != 1) ? (0.925f + dialogVisualOpacity * 0.075f) : (1.075f - dialogVisualOpacity * 0.075f);
		if (dialogVisualOpacity < 0.075f && dialogRes != 255)
			break;
		gspWaitForVBlank();
	}
	dialogShown=false; Init::StopDlgWaitSound();
	return dialogRes;
}
void Dialog::Display(){
	float genericvar0; bool genbool;
	Draw_Rect(0,0,320,240,C2D_Color32f(0,0,0,dialogVisualOpacity/2));
	C2D_DrawParams param;
	C2D_PlainImageTint(&dialogdeftint, C2D_Color32f(1,1,1,dialogVisualOpacity), 0.0f);
	param.pos.x = 160.0f; param.pos.y = 120.0f;
	param.pos.w = 320*dialogVisualScale; param.pos.h = 240*dialogVisualScale;
	param.center.x = 160.0f*dialogVisualScale; param.center.y = 120.0f*dialogVisualScale;
	param.angle = 0; param.depth = 0.5f;
	C2D_DrawImage(C2D_SpriteSheetGetImage(gfx_appdlg, appdlg_ic_dlg_body_idx), &param, &dialogdeftint);
	genericvar0 = ((120.0f - (20.0f + 10.0f * ((dialogAbtnf||dialogBbtnf)&&dialogWaitingForBool)) * dialogVisualScale) - ((10.0f * dialogBodyText.size()) * dialogVisualScale));
	for(u32 i=0; i < dialogBodyText.size(); i++){
		Draw_Text_Center(160.0f, genericvar0 + (i * (20.0f * dialogVisualScale)), 0.6f*dialogVisualScale, C2D_Color32f(1,1,1,dialogVisualOpacity), dialogBodyText[i].c_str());
	}
	if (dialogAbtnf != dialogBbtnf) {
		if (dialogAbtnf){genbool=dialogAbtnPress;}else{genbool=dialogBbtnPress;}
		param.pos.x = 160.0f; param.pos.y = 120.0f + 52.0f * dialogVisualScale;
		param.pos.w = 256*dialogVisualScale; param.pos.h = 44*dialogVisualScale;
		param.center.x = 128.0f*dialogVisualScale; param.center.y = 0.0f;
		param.angle = 0; param.depth = 0.5f; genericvar0=(1.0f - 0.75f * genbool);
		C2D_PlainImageTint(&dialogdeftint, C2D_Color32f(0.4f,0.4f,0.4f,dialogVisualOpacity), 1.0f);
		C2D_TopImageTint(&dialogdeftint, C2D_Color32f(genericvar0,genericvar0,genericvar0,dialogVisualOpacity), 1.0f);
		C2D_DrawImage(C2D_SpriteSheetGetImage(gfx_appdlg, appdlg_ic_dlg_btn_idx), &param, &dialogdeftint);
		if (dialogAbtnf){
			DrawStrBoxC(160, 120.0f + 62.5f * dialogVisualScale, 0.7f*dialogVisualScale, C2D_Color32f(!genbool,!genbool,!genbool,dialogVisualOpacity * 0.75f),dialogAbtn.c_str(),232.0f*dialogVisualScale,dialogVisualScale);
			DrawStrBoxC(160, 120.0f + 61.0f * dialogVisualScale, 0.7f*dialogVisualScale, C2D_Color32f(genbool,genbool,genbool,dialogVisualOpacity * 0.75f),dialogAbtn.c_str(),232.0f*dialogVisualScale,dialogVisualScale);
		} else {
			DrawStrBoxC(160, 120.0f + 62.5f * dialogVisualScale, 0.7f*dialogVisualScale, C2D_Color32f(!genbool,!genbool,!genbool,dialogVisualOpacity * 0.75f),dialogBbtn.c_str(),232.0f*dialogVisualScale,dialogVisualScale);
			DrawStrBoxC(160, 120.0f + 61.0f * dialogVisualScale, 0.7f*dialogVisualScale, C2D_Color32f(genbool,genbool,genbool,dialogVisualOpacity * 0.75f),dialogBbtn.c_str(),232.0f*dialogVisualScale,dialogVisualScale);
		}
	} else if (dialogAbtnf == 1) {
		param.pos.x = 160.0f; param.pos.y = 120.0f + 52.0f * dialogVisualScale;
		param.pos.w = 128*dialogVisualScale; param.pos.h = 44*dialogVisualScale;
		param.center.x = 128.0f*dialogVisualScale; param.center.y = 0.0f;
		param.angle = 0; param.depth = 0.5f; genericvar0=(1.0f - 0.75f * dialogBbtnPress);
		C2D_PlainImageTint(&dialogdeftint, C2D_Color32f(0.4f,0.4f,0.4f,dialogVisualOpacity), 1.0f);
		C2D_TopImageTint(&dialogdeftint, C2D_Color32f(genericvar0,genericvar0,genericvar0,dialogVisualOpacity), 1.0f);
		C2D_DrawImage(C2D_SpriteSheetGetImage(gfx_appdlg, appdlg_ic_dlg_btn_l_idx), &param, &dialogdeftint);
		DrawStrBoxC(160.0f - 64.0f * dialogVisualScale, 120.0f + 62.5f * dialogVisualScale, 0.7f*dialogVisualScale, C2D_Color32f(!dialogBbtnPress,!dialogBbtnPress,!dialogBbtnPress,dialogVisualOpacity * 0.75f),dialogBbtn.c_str(),112.0f*dialogVisualScale,dialogVisualScale);
		DrawStrBoxC(160.0f - 64.0f * dialogVisualScale, 120.0f + 61.0f * dialogVisualScale, 0.7f*dialogVisualScale, C2D_Color32f(dialogBbtnPress,dialogBbtnPress,dialogBbtnPress,dialogVisualOpacity * 0.75f),dialogBbtn.c_str(),112.0f*dialogVisualScale,dialogVisualScale);
		param.pos.x = 160.0f; param.pos.y = 120.0f + 52.0f * dialogVisualScale;
		param.pos.w = 128*dialogVisualScale; param.pos.h = 44*dialogVisualScale;
		param.center.x = 0.0f; param.center.y = 0.0f;
		param.angle = 0; param.depth = 0.5f; genericvar0=(1.0f - 0.75f * dialogAbtnPress);
		C2D_PlainImageTint(&dialogdeftint, C2D_Color32f(0.4f,0.4f,0.4f,dialogVisualOpacity), 1.0f);
		C2D_TopImageTint(&dialogdeftint, C2D_Color32f(genericvar0,genericvar0,genericvar0,dialogVisualOpacity), 1.0f);
		C2D_DrawImage(C2D_SpriteSheetGetImage(gfx_appdlg, appdlg_ic_dlg_btn_r_idx), &param, &dialogdeftint);
		DrawStrBoxC(160.0f + 64.0f * dialogVisualScale, 120.0f + 62.5f * dialogVisualScale, 0.7f*dialogVisualScale, C2D_Color32f(!dialogAbtnPress,!dialogAbtnPress,!dialogAbtnPress,dialogVisualOpacity * 0.75f),dialogAbtn.c_str(),112.0f*dialogVisualScale,dialogVisualScale);
		DrawStrBoxC(160.0f + 64.0f * dialogVisualScale, 120.0f + 61.0f * dialogVisualScale, 0.7f*dialogVisualScale, C2D_Color32f(dialogAbtnPress,dialogAbtnPress,dialogAbtnPress,dialogVisualOpacity * 0.75f),dialogAbtn.c_str(),112.0f*dialogVisualScale,dialogVisualScale);
		param.pos.x = 160.0f; param.pos.y = 120.0f + 52.0f * dialogVisualScale;
		param.pos.w = 4*dialogVisualScale; param.pos.h = 44*dialogVisualScale;
		param.center.x = 2.0f; param.center.y = 0.0f; param.angle = 0; param.depth = 0.5f;
		C2D_PlainImageTint(&dialogdeftint, C2D_Color32f(1,1,1,dialogVisualOpacity), 0.0f);
		C2D_DrawImage(C2D_SpriteSheetGetImage(gfx_appdlg, appdlg_ic_dlg_btn_div_idx), &param, &dialogdeftint);
	}
	if (dialogWaitingForBool){
		dialogWaitSpin += NUMBER_PI/16.0f; genericvar0=(dialogAbtnf||dialogBbtnf)*16.0f;
		if(dialogWaitSpin > NUMBER_PI*2.0f) dialogWaitSpin -= NUMBER_PI*2.0f;
		param.pos.x = 160.0f; param.pos.y = 120.0f + (52.0f - genericvar0) * dialogVisualScale;
		param.pos.w = 24*dialogVisualScale; param.pos.h = 24*dialogVisualScale;
		param.center.x = 12.0f; param.center.y = 12.0f; param.angle = 0; param.depth = 0.5f;
		C2D_PlainImageTint(&dialogdeftint, C2D_Color32f(0,0.5f,0.35f,dialogVisualOpacity), 1.0f);
		C2D_DrawImage(C2D_SpriteSheetGetImage(gfx_common, common_waiticon_bg_idx), &param, &dialogdeftint);
		param.pos.x = 160.0f; param.pos.y = 120.0f + (52.0f - genericvar0) * dialogVisualScale;
		param.pos.w = 24*dialogVisualScale; param.pos.h = 24*dialogVisualScale;
		param.center.x = 12.0f; param.center.y = 12.0f; param.depth = 0.5f; param.angle = dialogWaitSpin;
		C2D_PlainImageTint(&dialogdeftint, C2D_Color32f(0,1,0.7f,dialogVisualOpacity), 1.0f);
		C2D_DrawImage(C2D_SpriteSheetGetImage(gfx_common, common_waiticon_idx), &param, &dialogdeftint);
	}
}

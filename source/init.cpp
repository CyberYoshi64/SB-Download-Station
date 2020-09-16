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

#define CONFIG_3D_SLIDERSTATE (*(float *)0x1FF81080)

static touchPosition touch;
	extern bool fadein;
	extern bool fadeout;
	extern u8 fadealpha;
	extern u8 fadecolor;
	extern C3D_RenderTarget *top;
	extern C3D_RenderTarget *bottom;
	extern int errorcode;
	extern char errorstr;
	extern int meta_total;
	extern std::string meta_prjn[];
	extern std::string meta_title[];
	extern std::string meta_desc[];
	extern int meta_ver[];

Handle* FShandle;
FS_Path FSpath;
FS_Archive FSarc;
FS_Archive extdata_archive;

char* string0;
bool doFSoperations;
bool dspfirmfound = false;
bool updateAvailable = false;
static bool musicPlaying = false;
u8 consoleModel = 0;
float scx = 0;
float scy = -100.0f;
float pagex = 0;
float pagey = 0;
int pageid = 0;
Result init_res;
Result appres;
u64 sb3freespace;
bool isInit = true;
int delay = 0;
bool exiting = false;
bool waiting = false;

// Music and sound effects.
sound *mus_bgm = NULL;
sound *sfx_launch = NULL;
sound *sfx_select = NULL;
sound *sfx_stop = NULL;
sound *sfx_switch = NULL;
sound *sfx_wrong = NULL;
sound *sfx_back = NULL;
sound *sfx_wait = NULL;

C2D_SpriteSheet sprites;

void Play_Music(void) {
	if (!musicPlaying && dspfirmfound) {
		mus_bgm->play();
		musicPlaying = true;
	}
}

Result Init::Initialize() {
	romfsInit();
	gfxInitDefault();
	Gui::init();
	init_res = cfguInit();
	if (R_SUCCEEDED(init_res)) {
		CFGU_GetSystemModel(&consoleModel);
	} else {
		errorcode=10000002;
	}
	init_res = acInit(); if(!R_SUCCEEDED(init_res)){errorcode=10000000;}
	init_res = amInit(); if(!R_SUCCEEDED(init_res)){errorcode=10000001;}
	init_res = srvInit(); if(!R_SUCCEEDED(init_res)){errorcode=10000004;}
	init_res = fsInit();
	
	osSetSpeedupEnable(true);	// Enable speed-up for New 3DS users
	
	// make folders if they don't exist
	mkdir("sdmc:/3ds", 0777);	                        // For DSP dump
	mkdir("sdmc:/SB-Download-Station",0777);            // Working root directory on the SD card
	mkdir("sdmc:/SB-Download-Station/cache",0777);      // Working cache directory on the SD card
	mkdir("sdmc:/SB-Download-Station/data",0777);       // Working data directory on the SD card

 	if (access("sdmc:/3ds/dspfirm.cdc", F_OK ) != -1 ) { // Was DSP firm dumped before?
		ndspInit(); // If so, then initialise the service
		dspfirmfound = true;
	} else{ // If not, then attempt dumping it.
		showError("The DSP firm was not found in \"sdmc:/3ds/dspfirm.cdc\".\n\nBecause of this inconvenience, no sounds can be played.\n\nPress \"Okay\" to proceed without sound.");
	}
	// Load the sound effects if DSP is available.
	if (dspfirmfound){
		mus_bgm = new sound("sdmc:/SB-Download-Station/data/bgm.wav", 0, true);
		sfx_launch = new sound("romfs:/sound/launch.wav", 1, false);
		sfx_select = new sound("romfs:/sound/select.wav", 2, false);
		sfx_stop = new sound("romfs:/sound/stop.wav", 2, false);
		sfx_switch = new sound("romfs:/sound/switch.wav", 2, false);
		sfx_wrong = new sound("romfs:/sound/wrong.wav", 2, false);
		sfx_back = new sound("romfs:/sound/back.wav", 2, false);
		sfx_wait = new sound("romfs:/sound/wait.wav", 3, true);
	}
	
	C2D_TargetClear(top,RED);
	
	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
	Draw_Text_Center(160,116,FONT_SIZE_14,-1,"Checking for updates...");
	C3D_FrameEnd(0);

	checkForUpdates();
	
	Meta::init();
	
	if(!R_SUCCEEDED(init_res)){
		errorcode=10000003;
	} else {
		u32 extdata_archive_lowpathdata[3] = {1,0x00001a1c,0};
		FShandle=fsGetSessionHandle();
		appres=FSUSER_OpenArchive(&extdata_archive, ARCHIVE_EXTDATA, {PATH_BINARY, 12, &extdata_archive_lowpathdata});
	}
	
	return 0;
}

Result Init::MainLoop() {
	// Initialize everything.
	Initialize();

	// Loop as long as the status is not exiting.
	while (aptMainLoop())
	{
		hidScanInput();
		u32 hHeld = hidKeysHeld();
		u32 hDown = hidKeysDown();
		hidTouchRead(&touch);
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		Gui::clearTextBufs();
		Gui::DrawScreen();
		Gui::ScreenLogic(hDown, hHeld, touch); // Call the logic of the current screen here.
		Gui::fadeEffects();
		C3D_FrameEnd(0);
		if (exiting) {
			StopWaitSound();
			if (!fadeout)	break;
		}
		if (isInit) {
			delay++;
				Play_Music();
				isInit = false;
		}
		if (exiting){
			scy += -4;
		}
		scx=(scx * 3.0f + pagex) / 4.0f;
		scy=(scy * 3.0f + pagey) / 4.0f;
	}
	// Exit all services and exit the app.
	Exit();
	return 0;
}

void Init::SelSound(){
	if (dspfirmfound){
		sfx_select->stop();
		sfx_select->play();
	}
}
void Init::BackSound(){
	if (dspfirmfound){
		sfx_back->stop();
		sfx_back->play();
	}
}
void Init::WrongSound(){
	if (dspfirmfound){
		sfx_wrong->stop();
		sfx_wrong->play();
	}
}
void Init::LaunchSound(){
	if (dspfirmfound){
		sfx_launch->stop();
		sfx_launch->play();
	}
}
void Init::StopSound(){
	if (dspfirmfound){
		sfx_stop->stop();
		sfx_stop->play();
	}
}
void Init::SwitchSound(){
	if (dspfirmfound){
		sfx_switch->stop();
		sfx_switch->play();
	}
}
void Init::WaitSound(){
	if (dspfirmfound){
		sfx_wait->play();
		waiting=true;
	}
}
void Init::StopWaitSound(){
	if (dspfirmfound){
		sfx_wait->stop();
		waiting=false;
	}
}

Result Init::Exit() {
	delete mus_bgm;
	delete sfx_launch;
	delete sfx_select;
	delete sfx_stop;
	delete sfx_switch;
	delete sfx_wrong;
	delete sfx_back;
	if (dspfirmfound) {
		ndspExit();
	}
	Meta::exit();
	Gui::exit();
	FSUSER_CloseArchive(extdata_archive);
	romfsExit();
	srvExit();
	fsExit();
	cfguExit();
	amExit();
	acExit();
	return 0;
}

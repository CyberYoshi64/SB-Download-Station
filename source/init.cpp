#include "common.hpp"
#include "errtbl.hpp"

#define CONFIG_3D_SLIDERSTATE (*(float *)0x1FF81080)

bool drawingScreen;
bool stopScreenUpdate;
static touchPosition touch;
extern std::string top_screen_title;
extern bool fadein;
extern bool fadeout;
extern u8 fadealpha;
extern u8 fadecolor;
extern C3D_RenderTarget *top;
extern C3D_RenderTarget *bottom;
extern int errorcode;
extern char errorstr[];
extern u32 meta_total;
extern std::string meta_prjn[];
extern std::string meta_title[];
extern std::string meta_desc[];
extern u32 meta_ver[];
extern int CFGLang;

Handle FShandle;
FS_Path FSpath;
FS_Archive FSarc;
FS_Archive extarc;

char* string0;
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
u64 jumptarg;
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
	gfxInitDefault();
	mcuHwcInit();
	romfsInit();
	Gui::init();
	init_res = cfguInit();
	if (R_SUCCEEDED(init_res)) {
		u8 tmplng=0;
		CFGU_GetSystemLanguage(&tmplng);
		CFGLang=(int)tmplng;
		CFGU_GetSystemModel(&consoleModel);
	} else {
		errorcode=10000002; exiting=true;
	}
	init_res = acInit(); if(!R_SUCCEEDED(init_res)){errorcode=10000000; exiting=true;}
	init_res = amInit(); if(!R_SUCCEEDED(init_res)){errorcode=10000001; exiting=true;}
	init_res = srvInit(); if(!R_SUCCEEDED(init_res)){errorcode=10000004; exiting=true;}
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
	} else{ // If not, then run without sound.
		errorcode=10000009;
	}
	// Load the sound effects if DSP is available.
	if (dspfirmfound){
		mus_bgm = new sound("romfs:/sound/bgm.wav", 15, true);
		sfx_launch = new sound("romfs:/sound/launch.wav", 0, false);
		sfx_select = new sound("romfs:/sound/select.wav", 1, false);
		sfx_stop = new sound("romfs:/sound/stop.wav", 2, false);
		sfx_switch = new sound("romfs:/sound/switch.wav", 3, false);
		sfx_wrong = new sound("romfs:/sound/wrong.wav", 4, false);
		sfx_back = new sound("romfs:/sound/back.wav", 5, false);
		sfx_wait = new sound("romfs:/sound/wait.wav", 6, true);
	}
	
	Meta::init();

	if(!R_SUCCEEDED(init_res)){
		errorcode=10000003; exiting=true;
	} else {
		u32 extdata_archive_lowpathdata[3] = {1,0x00001a1c,0};
		appres=FSUSER_OpenArchive(&extarc, ARCHIVE_EXTDATA, (FS_Path){PATH_BINARY, 12, &extdata_archive_lowpathdata});
	}
	drawingScreen = false;
	if (osGetKernelVersion() < SYSTEM_VERSION(2,57,0)){errorcode=99999999; exiting=true;}
	return 0;
}

Result Init::MainLoop() {
	// Initialize everything.
	Initialize();
	createThread((ThreadFunc)Screen::Thread);
	top_screen_title = LngpackStr(LNGTXT_APPNAME, CFGLang);
	// Loop as long as the status is not exiting.
	while (aptMainLoop()) {
		hidScanInput();
		u32 hHeld = hidKeysHeld();
		u32 hDown = hidKeysDown();
		hidTouchRead(&touch);
		Gui::ScreenLogic(hDown, hHeld, touch); // Call the logic of the current screen here.
		if (exiting) {
			break;
		}
		if (isInit) {
			delay++;
				Play_Music();
				isInit = false;
		}
		gspWaitForVBlank();
		if (stopScreenUpdate && !aptShouldJumpToHome()){
			stopScreenUpdate=false;
			createThread((ThreadFunc)Screen::Thread);
		}
	}
	// Exit all services and exit the app.
	destroyThreads();
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

	if (access("sdmc:/SB-Download-Station/cache/app.cia", F_OK ) != -1){
		deleteFile("sdmc:/SB-Download-Station/cache/app.cia");
	}

	if (access("sdmc:/SB-Download-Station/cache/ic.t3x", F_OK ) != -1){
		deleteFile("sdmc:/SB-Download-Station/cache/ic.t3x");
	}

	if (access("sdmc:/SB-Download-Station/cache/prjlist.txt", F_OK ) != -1){
		deleteFile("sdmc:/SB-Download-Station/cache/prjlist.txt");
	}

	delete mus_bgm;
	delete sfx_launch;
	delete sfx_select;
	delete sfx_stop;
	delete sfx_switch;
	delete sfx_wrong;
	delete sfx_back;
	delete sfx_wait;
	if (dspfirmfound) {
		ndspExit();
	}
	Meta::exit();
	Gui::exit();
	FSUSER_ControlArchive(extarc, ARCHIVE_ACTION_COMMIT_SAVE_DATA, nullptr, 0, nullptr, 0);
	FSUSER_CloseArchive(extarc);
	romfsExit();
	srvExit();
	fsExit();
	cfguExit();
	amExit();
	acExit();
	mcuHwcExit();
	gfxExit();
	return 0;
}

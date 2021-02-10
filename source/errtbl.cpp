#include "errtbl.hpp"
#include <cstring>
extern char errorstr;
extern int errorcode;
extern int CFGLang;
extern int meta_total;
char err[2048];
char* lngc;
char desc[8];

const char* AppErrTbl(int errorcode, u8 lang){
  sprintf(err,"An unknown error code was specified.\n\nPlease contact CyberYoshi64 " \
  "and describe him, what you did to make this error happen.\nHopefully, " \
  "he will fix it soon.");
  if (errorcode < 10000000){
    sprintf(err,"Error summary:\n\nModule: %s \nLevel: %s \nSummary: %s \nDesc.: %s ",SysErr_ModuleStr(errorcode),SysErr_LevelStr(errorcode),SysErr_SummaryStr(errorcode),SysErr_DescStr(errorcode));
  }
  if (errorcode == -1){
    sprintf(err,"An invalid error code was specified.\n\n" \
    "Please contact CyberYoshi64 and describe him, what you did to make this error happen.\n" \
    "Hopefully, he will fix it soon.");
  }
  if (errorcode == 10000000){
    sprintf(err,"The CTR \"AC\" service couldn't be initialized.");
  }
  if (errorcode == 10000001){
    sprintf(err,"The CTR \"AM\" (Application manager) service couldn't be initialized.");
  }
  if (errorcode == 10000002){
    sprintf(err,"The CTR \"CFGU\" (Configuration) service couldn't be initialized.");
  }
  if (errorcode == 10010000){
    sprintf(err,"Invalid error code received from the download API.\n\nDownloadError return value: DL_ERROR_NONE");
  }
  if (errorcode == 10010001){
    sprintf(err,"Failed to write file on the SD card.\n\nDownloadError return value: DL_ERROR_WRITEFILE");
  }
  if (errorcode == 10010002){
    sprintf(err,"Failed to allocate buffer in memory.\n\nDownloadError return value: DL_ERROR_ALLOC");
  }
  if (errorcode == 10010003){
    sprintf(err,"Invalid or no staus code received.\n\nDownloadError return value: DL_ERROR_STATUSCODE");
  }
  if (errorcode == 10010004){
    sprintf(err,"Could not successfully communicate with GitHub.\n\nDownloadError return value: DL_ERROR_GIT\n\nIf you are CyberYoshi64, please consider changing the release type from \"Pre-release\" to \"Release\" or fix up the file names in the release.\nAre the file names \"SB3_Download_Station.*\" and are there a 3DSX and a CIA included?");
  }
  if (errorcode == 10010006){
    sprintf(err,"Could not connect to the internet.\n\nPlease enable wireless communication, or setup an internet connection in System Settings.");
  }
  if (errorcode == 10010008){
    sprintf(err,"The index table on the repository doesn't appear to be refreshed properly.\nThe number of projects found in line 1 doe not match the actual number of projects.\n\nUsing the guessed number of projects for the listing.\n\nIf this error occures for over a week, please file an issue on the GitHub repository \"CyberYoshi64/SB-Download-Station\".");
  }
  if (lang==CFG_LANGUAGE_DE) {
  sprintf(err,"Ein unbekannter Fehlercode wurde ausgelöst.\n\n" \
  "Bitte kontaktiere CyberYoshi64 und erkläre ihm, was du in der App getan hast, um diesen Fehler auszulösen.\n" \
  "Hoffentlich wird er diesen Fehler bald beheben.");
    if (errorcode == -1){
      sprintf(err,"Ein ungültiger Fehlercode wurde ausgelöst.\n\n" \
      "Bitte kontaktiere CyberYoshi64 und erkläre ihm, was du in der App getan hast, um diesen Fehler auszulösen.\n" \
      "Hoffentlich wird er diesen Fehler bald beheben.");
    }
    if (errorcode == 10000000){
      sprintf(err,"Das CTR-AC-Service konnte nicht initialisiert werden.");
    }
    if (errorcode == 10000001){
      sprintf(err,"Das CTR-AM-Service (Applikationsmanager) konnte nicht initialisiert werden.");
    }
    if (errorcode == 10000002){
      sprintf(err,"Das CTR-CFGU-Service (Systemkonfiguration) konnte nicht initialisiert werden.");
    }

  }
  return err;
}

const char* LanguageStringFromInt(u8 lang){
  switch (lang) {
    case 0: return "日本語";
    case 1: return "English";
    case 2: return "Français";
    case 3: return "Deutsch";
    case 4: return "Italiano";
    case 5: return "Español";
    case 6: return "简体中文";
    case 7: return "한국어";
    case 8: return "Nederlands";
    case 9: return "Português";
    case 10: return "Pyccкий";
    case 11: return "繁體中文";
  }
  return"";
}

const char* LanguageStrFromInt(u8 lang){
  switch (lang) {
    case 0: return "JP";
    case 1: return "EN";
    case 2: return "FR";
    case 3: return "DE";
    case 4: return "IT";
    case 5: return "ES";
    case 6: return "ZH";
    case 7: return "KO";
    case 8: return "NL";
    case 9: return "PT";
    case 10: return "RU";
    case 11: return "TW";
  }
  return"";
}

u8 LanguageStrFromInt(const char* lang){
    sprintf(lngc,"JP"); if (lang==lngc) return 0;
    sprintf(lngc,"EN"); if (lang==lngc) return 1;
    sprintf(lngc,"FR"); if (lang==lngc) return 2;
    sprintf(lngc,"DE"); if (lang==lngc) return 3;
    sprintf(lngc,"IT"); if (lang==lngc) return 4;
    sprintf(lngc,"ES"); if (lang==lngc) return 5;
    sprintf(lngc,"ZH"); if (lang==lngc) return 6;
    sprintf(lngc,"KO"); if (lang==lngc) return 7;
    sprintf(lngc,"NL"); if (lang==lngc) return 8;
    sprintf(lngc,"PT"); if (lang==lngc) return 9;
    sprintf(lngc,"RU"); if (lang==lngc) return 10;
    sprintf(lngc,"TW"); if (lang==lngc) return 11;
  return 255;
}

const char* LngpackStr(int lngid, int lang){
	if (lang == CFG_LANGUAGE_JP){ // Japanese
		
	} else if (lang == CFG_LANGUAGE_FR){ // French (Français)
		
	} else if (lang == CFG_LANGUAGE_DE){ // German (Deutsch)
		
	} else if (lang == CFG_LANGUAGE_IT){ // Italian (Italiano)
		
	} else if (lang == CFG_LANGUAGE_ES){ // Spanish (Espanol)
		
	} else if (lang == CFG_LANGUAGE_NL){ // Dutch (Nederlands)
		
	} else if (lang == CFG_LANGUAGE_PT){ // Portuguese (Português)
		
	} else if (lang == CFG_LANGUAGE_RU){ // Russian (Pyccĸuu)
		
	} else if (lang < 12){ // English (English, fallback for valid languages)
		switch (lngid) {
			case LNGTXT_APPNAME: return "SmileBASIC Download Station";
			case LNGTXT_EXIT: return "Exit";
			case LNGTXT_BACK: return "Back";
			case LNGTXT_NEXT: return "Next";
			case LNGTXT_YES: return "Yes";
			case LNGTXT_NO: return "No";
			case LNGTXT_CANCEL: return "Cancel";
			case LNGTXT_CONTINUE: return "Continue";
			
			case LNGTXT_PROCEED_ASK: return "Would you like to proceed?";
			
			case LNGTXT_MNG_PRJ: return "Backup a project";
			case LNGTXT_DOWNLOAD_PRJ: return "Download projects";
			case LNGTXT_UPDATE_APP: return "Download updates";
			
			case LNGTXT_ABOUT: return "About";
			case LNGTXT_ABOUT_SQUAT: return "Original idea: the_squat1115";
			case LNGTXT_ABOUT_CY64: return "App programmer: CyberYoshi64";
			case LNGTXT_ABOUT_SRCCODE: return "Certain pieces of the source code are from:";
			case LNGTXT_ABOUT_SMILEBASIC: return "SmileBASIC is a trademark of SmileBoom Co.Ltd.";
			case LNGTXT_ABOUT_BUILD: return "Built with devkitPro on 20201221.";
			
			case LNGTXT_MAIN_MENU_TXT: return "Welcome to SmileBASIC Download Station!\n\nHere you can manage your SmileBASIC projects\nand download new projects from a GitHub repository.";
			case LNGTXT_PLSWAIT: return "Please wait…";
			case LNGTXT_SETTINGS_TXT: return "Configure SmileBASIC Download Station.";
			case LNGTXT_INSTALLING_TXT: return "Installing project to SmileBASIC...";
			case LNGTXT_UPDATING_TXT: return "Updating application...";
		}
	}
	sprintf(desc,"%02x:%04d",CFGLang,lngid);
	return desc;
}

const char* SysErr_LevelStr(int id){
	
	if (id < 0 || id > 31)
		id = R_LEVEL(id);
	
	switch (id){
		case RL_SUCCESS: return "Success";
		case RL_INFO: return "Info";
		case RL_FATAL: return "Fatal";
		case RL_RESET: return "Reset";
		case RL_REINITIALIZE: return "Reinitialize";
		case RL_USAGE: return "Usage";
		case RL_PERMANENT: return "Permanent";
		case RL_TEMPORARY: return "Temporary";
		case RL_STATUS: return "Status";
		default: return "[invalid level]";
	}
}

const char* SysErr_SummaryStr(int id){
	
	if (id < 0 || id > 63)
		id = R_SUMMARY(id);
	
	switch (id){
		case RS_SUCCESS: return "Success";
		case RS_NOP: return "No operation";
		case RS_WOULDBLOCK: return "Would block";
		case RS_OUTOFRESOURCE: return "Out of resource";
		case RS_NOTFOUND: return "Not found";
		case RS_INVALIDSTATE: return "Invalid state";
		case RS_NOTSUPPORTED: return "Not supported";
		case RS_INVALIDARG: return "Invalid argument";
		case RS_WRONGARG: return "Wrong argument";
		case RS_CANCELED: return "Cancelled";
		case RS_STATUSCHANGED: return "Status changed";
		case RS_INTERNAL: return "Internal error";
		case RS_INVALIDRESVAL: return "Invalid result value";
		default: return "[invalid summary]";
	}
	return "[invalid summary]";
}

const char* SysErr_ModuleStr(int id){
	
	if (id < 0 || id > 255)
		id = R_MODULE(id);
	
	switch (id){
		case RM_COMMON: return "Common";
		case RM_KERNEL: return "Kernel";
		case RM_UTIL: return "Util";
		case RM_FILE_SERVER: return "File server";
		case RM_LOADER_SERVER: return "Loader server";
		case RM_TCB: return "TCB";
		case RM_OS: return "OS";
		case RM_DBG: return "DBG";
		case RM_DMNT: return "DMNT";
		case RM_PDN: return "PDN";
		case RM_GSP: return "GSP";
		case RM_I2C: return "I2C";
		case RM_GPIO: return "GPIO";
		case RM_DD: return "DD";
		case RM_CODEC: return "CODEC";
		case RM_SPI: return "SPI";
		case RM_PXI: return "PXI";
		case RM_FS: return "FS";
		case RM_DI: return "DI";
		case RM_HID: return "HID";
		case RM_CAM: return "CAM";
		case RM_PI: return "PI";
		case RM_PM: return "PM";
		case RM_PM_LOW: return "PM (low)";
		case RM_FSI: return "FSI";
		case RM_SRV: return "SRV";
		case RM_NDM: return "NDM";
		case RM_NWM: return "NWM";
		case RM_SOC: return "SOC";
		case RM_LDR: return "LDR";
		case RM_ACC: return "ACC";
		case RM_ROMFS: return "RomFS";
		case RM_AM: return "AM";
		case RM_HIO: return "HIO";
		case RM_UPDATER: return "Updater";
		case RM_MIC: return "MIC";
		case RM_FND: return "FND";
		case RM_MP: return "MP";
		case RM_MPWL: return "MPWL";
		case RM_AC: return "AC";
		case RM_HTTP: return "HTTP";
		case RM_DSP: return "DSP";
		case RM_SND: return "SND";
		case RM_DLP: return "DLP";
		case RM_HIO_LOW: return "HIO (low)";
		case RM_CSND: return "CSND";
		case RM_SSL: return "SSL";
		case RM_AM_LOW: return "AM (low)";
		case RM_NEX: return "NEX";
		case RM_FRIENDS: return "Friends";
		case RM_RDT: return "RDT";
		case RM_APPLET: return "Applet";
		case RM_NIM: return "NIM";
		case RM_PTM: return "PTM";
		case RM_MIDI: return "MIDI";
		case RM_MC: return "MC";
		case RM_SWC: return "SWC";
		case RM_FATFS: return "FatFS";
		case RM_NGC: return "NGC";
		case RM_CARD: return "Card";
		case RM_CARDNOR: return "CardNOR";
		case RM_SDMC: return "SDmc";
		case RM_BOSS: return "Boss";
		case RM_DBM: return "DBM";
		case RM_CONFIG: return "Config";
		case RM_PS: return "PS";
		case RM_CEC: return "CEC";
		case RM_IR: return "IR";
		case RM_UDS: return "UDS";
		case RM_PL: return "PL";
		case RM_CUP: return "CUP";
		case RM_GYROSCOPE: return "Gyroscope";
		case RM_MCU: return "MCU";
		case RM_NS: return "NS";
		case RM_NEWS: return "News";
		case RM_RO: return "RO";
		case RM_GD: return "GD";
		case RM_CARD_SPI: return "Card SPI";
		case RM_EC: return "EC";
		case RM_WEB_BROWSER: return "Web browser";
		case RM_TEST: return "Test";
		case RM_ENC: return "ENC";
		case RM_PIA: return "PIA";
		case RM_ACT: return "ACT";
		case RM_VCTL: return "VCTL";
		case RM_OLV: return "OLV";
		case RM_NEIA: return "NEIA";
		case RM_NPNS: return "NPNS";
		case RM_AVD: return "AVD";
		case RM_L2B: return "L2B";
		case RM_MVD: return "MVD";
		case RM_NFC: return "NFC";
		case RM_UART: return "UART";
		case RM_SPM: return "SPM";
		case RM_QTM: return "QTM";
		case RM_NFP: return "NFP";
		case RM_APPLICATION: return "Application";
		case RM_INVALIDRESVAL: return "Invalid result value";
		default: return "[invalid module]";
	}
	return "[invalid module]";
}

const char* SysErr_DescStr(int id){
	
	if (id < 0 || id > 0x3FF)
		id = R_DESCRIPTION(id);
	
	switch (id){
		case RD_SUCCESS: return "Success";
		case RD_INVALID_RESULT_VALUE: return "Invalid result value";
		case RD_TIMEOUT: return "Timeout";
		case RD_OUT_OF_RANGE: return "Out of range";
		case RD_ALREADY_EXISTS: return "Already exists";
		case RD_CANCEL_REQUESTED: return "Cancel requested";
		case RD_NOT_FOUND: return "Not found";
		case RD_ALREADY_INITIALIZED: return "Already initialized";
		case RD_NOT_INITIALIZED: return "Not initialized";
		case RD_INVALID_HANDLE: return "Invalid handle";
		case RD_INVALID_POINTER: return "Invalid pointer";
		case RD_INVALID_ADDRESS: return "Invalid address";
		case RD_OUT_OF_MEMORY: return "Out of memory";
		case RD_MISALIGNED_SIZE: return "Misaligned size";
		case RD_MISALIGNED_ADDRESS: return "Misaligned address";
		case RD_BUSY: return "Busy";
		case RD_NO_DATA: return "No data";
		case RD_INVALID_COMBINATION: return "Invalid combination";
		case RD_INVALID_ENUM_VALUE: return "Invalid enumeration value";
		case RD_INVALID_SIZE: return "Invalid size";
		case RD_ALREADY_DONE: return "Already done";
		case RD_TOO_LARGE: return "Too large";
		case RD_INVALID_SELECTION: return "Invalid selection";
		default: return "[invalid level]";
	}
}

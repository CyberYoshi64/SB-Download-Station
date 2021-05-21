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
  if (errorcode < 10000000 || errorcode > 10019999){
    sprintf(err,"A system error has occured with the result code 0x%08x.\n"\
    "It's recommended to exit this app as soon as possible and reboot the system.\n"\
    "If the problem persists, there might be a problem with your system.\n"\
    "Ask for help in the Nintendo Homebrew Discord server.\n\n"\
    "Below is a description of said result code:\n\n"\
	"Module:\n%s (%d)\n\nLevel:\n%s (%d)\n\nSummary:\n%s (%d)\n\nDesc.:\n%s (%d)",\
	errorcode, \
	SysErr_ModuleStr(errorcode), R_MODULE(errorcode),\
	SysErr_LevelStr(errorcode), R_LEVEL(errorcode),\
	SysErr_SummaryStr(errorcode), R_SUMMARY(errorcode),\
	SysErr_DescStr(errorcode), R_DESCRIPTION(errorcode));
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
  if (errorcode == 10019990){
    sprintf(err," ");
  }
  if (errorcode == 10019991){
    sprintf(err,"\n\n\n\n\n\n\n\n\n\n\n\n");
  }
  if (lang==CFG_LANGUAGE_DE) {
  sprintf(err,"Ein unbekannter Fehlercode wurde ausgelöst.\n\n" \
  "Bitte kontaktiere CyberYoshi64 und erkläre ihm, was du in der App getan hast, um diesen Fehler auszulösen.\n" \
  "Hoffentlich wird er diesen Fehler bald beheben.");
	if (errorcode < 10000000 || errorcode > 10019999){
    	sprintf(err,"Ein Systemfehler ist aufgetetren mit dem Ergebniscode 0x%08x.\n"\
    	"Es wird empfohlen, die Anwendung zu beenden und das System neuzustarten.\n"\
    	"Wenn dieses Problem bestehen bleibt, könnte dein System einem Fehler ausgesetzt sein.\n"\
    	"Frage nach Hilfe in dem NH-Discord-Server (auf Englisch).\n\n"\
    	"Unten befindet sich eine Beschreibung des Ergebniscodes:\n\n"\
		"Modul:\n%s (%d)\n\nLevel:\n%s (%d)\n\nZusammenfassung:\n%s (%d)\n\nBeschreibung:\n%s (%d)",\
		errorcode, \
		SysErr_ModuleStr(errorcode), R_MODULE(errorcode),\
		SysErr_LevelStr(errorcode), R_LEVEL(errorcode),\
		SysErr_SummaryStr(errorcode), R_SUMMARY(errorcode),\
		SysErr_DescStr(errorcode), R_DESCRIPTION(errorcode));
	}
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
		switch (lngid) {
			case LNGTXT_APPNAME: return "SmileBASIC Download Station"; break;
			case LNGTXT_EXIT: return "Beenden"; break;
			case LNGTXT_BACK: return "Zurück"; break;
			case LNGTXT_NEXT: return "Weiter"; break;
			case LNGTXT_YES: return "Ja"; break;
			case LNGTXT_NO: return "Nein"; break;
			case LNGTXT_CANCEL: return "Abbrechen"; break;
			case LNGTXT_CONTINUE: return "Fortfahren"; break;
			
			case LNGTXT_PROCEED_ASK: return "Möchtest du fortfahren?"; break;
			
			case LNGTXT_MNG_PRJ: return "Projektsicherung"; break;
			case LNGTXT_DOWNLOAD_PRJ: return "Projekt-Download"; break;
			case LNGTXT_UPDATE_APP: return "Updates"; break;
			
			case LNGTXT_ABOUT: return "Über..."; break;
			case LNGTXT_ABOUT_SQUAT: return "Idee: the_squat1115"; break;
			case LNGTXT_ABOUT_CY64: return "Programmierer: CyberYoshi64"; break;
			case LNGTXT_ABOUT_SRCCODE: return "Einige Quellcodes stammen von:"; break;
			case LNGTXT_ABOUT_SMILEBASIC: return "SmileBASIC ist ein Warenzeichen von SmileBoom Co.Ltd."; break;
			case LNGTXT_ABOUT_BUILD: return "Built with devkitPro on 20210221."; break;
			
			case LNGTXT_MAIN_MENU_TXT: return "Willkommen bei SmileBASIC Download Station!\n\nHier kannst du SmileBASIC-Projekte sichern\nund neue von einem GitHub-Repositorium herunterladen."; break;
			case LNGTXT_PLSWAIT: return "Bitte warten…"; break;
			case LNGTXT_SETTINGS_TXT: return "Konfigurieren…"; break;
			case LNGTXT_INSTALLING_TXT: return "Projekte werden installiert…"; break;
			case LNGTXT_UPDATING_TXT: return "Aktualisierung wird durchgeführt…"; break;
		}
	} else if (lang == CFG_LANGUAGE_IT){ // Italian (Italiano)
		
	} else if (lang == CFG_LANGUAGE_ES){ // Spanish (Espanol)
		
	} else if (lang == CFG_LANGUAGE_NL){ // Dutch (Nederlands)
		
	} else if (lang == CFG_LANGUAGE_PT){ // Portuguese (Português)
		
	} else if (lang == CFG_LANGUAGE_RU){ // Russian (Pyccĸuu)
		
	} else if (lang < 12){ // English (English, fallback for valid languages)
		switch (lngid) {
			case LNGTXT_APPNAME: return "SmileBASIC Download Station"; break;
			case LNGTXT_EXIT: return "Exit"; break;
			case LNGTXT_BACK: return "Back"; break;
			case LNGTXT_NEXT: return "Next"; break;
			case LNGTXT_YES: return "Yes"; break;
			case LNGTXT_NO: return "No"; break;
			case LNGTXT_CANCEL: return "Cancel"; break;
			case LNGTXT_CONTINUE: return "Continue"; break;
			
			case LNGTXT_PROCEED_ASK: return "Would you like to proceed?"; break;
			
			case LNGTXT_MNG_PRJ: return "Backup a project"; break;
			case LNGTXT_DOWNLOAD_PRJ: return "Download projects"; break;
			case LNGTXT_UPDATE_APP: return "Download updates"; break;
			
			case LNGTXT_ABOUT: return "About"; break;
			case LNGTXT_ABOUT_SQUAT: return "Original idea: the_squat1115"; break;
			case LNGTXT_ABOUT_CY64: return "App programmer: CyberYoshi64"; break;
			case LNGTXT_ABOUT_SRCCODE: return "Certain pieces of the source code are from:"; break;
			case LNGTXT_ABOUT_SMILEBASIC: return "SmileBASIC is a trademark of SmileBoom Co.Ltd."; break;
			case LNGTXT_ABOUT_BUILD: return "Built with devkitPro on 20201221."; break;
			
			case LNGTXT_MAIN_MENU_TXT: return "Welcome to SmileBASIC Download Station!\n\nHere you can manage your SmileBASIC projects\nand download new projects from a GitHub repository."; break;
			case LNGTXT_PLSWAIT: return "Please wait…"; break;
			case LNGTXT_SETTINGS_TXT: return "Configure…"; break;
			case LNGTXT_INSTALLING_TXT: return "Installing project to SmileBASIC…"; break;
			case LNGTXT_UPDATING_TXT: return "Updating application…"; break;
		}
	}
	sprintf(desc,"%02x:%04d",CFGLang,lngid);
	return desc;
}

// 
// Following code made with information from 3dbrew.org
// https://www.3dbrew.org/wiki/Error_codes
// 

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
		default: return "unknown";
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
		case RS_CANCELED: return "Canceled";
		case RS_STATUSCHANGED: return "Status changed";
		case RS_INTERNAL: return "Internal error";
		case RS_INVALIDRESVAL: return "Invalid result value";
		default: return "unknown";
	}
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
		case RM_SDMC: return "SDMC";
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
		default: return "unknown";
	}
}

const char* SysErr_DescStr(int id){
	
	if (R_MODULE(id) == RM_APPLICATION) return "Application-specific error.\nThis shouldn't appear because this application doesn't throw system error codes.\n";

	if (id < 0 || id > 0x3FF)
		id = R_DESCRIPTION(id);
	
	switch (id){
		case RD_SUCCESS: return "Success";
		case   2: return "Invalid memory permission (kernel)";
		case   4: return "Invalid ticket version (AM)";
		case   5: return "Invalid string length. (1-8 chars allowed) (srv)";
		case   6: return "Access denied. (srv)";
		case   7: return "Unexpected NULL byte (length doesn't match string content)";
		case   8: return "Camera already in use or busy (qtm)";
		case  10: return "Not enough memory (OS)";
		case  26: return "Session closed by remote (OS)";
		case  32: return "CIA doesn't have content or file is empty (AM)";
		case  37: return "NCCH corrupted or invalid (AM)";
		case  39: return "Invalid title version (AM)";
		case  43: return "Database non-existant or failed to open (AM)";
		case  44: return "Trying to uninstall system app (AM)";
		case  47: return "Invalid command header (OS)";
		case 101: return "Archive not mounted / Mountpoint not found (fs)";
		case 105: return "Request timed out (http)";
		case 106: return "CIA has invalid signature (AM)";
		case 120: return "Title or object not found (fs)";
		case 141: return "Gamecard not detected (fs)";
		case 230: return "Permission and open flag mismatch";
		case 271: return "Invalid configuration (mvd)";
		case 391: return "NCCH hash-check failed (fs)";
		case 392: return "RSA/AES-MAC verification failure (fs)";
		case 393: return "Invalid database (AM)";
		case 395: return "RomFS/Savedata hash-check failed (fs)";
		case 630: return "Command prohibited / Missing permissions (fs)";
		case 702: return "Invalid path (fs)";
		case 761: return "Incorrect read size for ExeFS (fs)";
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
		default: return "unknown";
	}
}

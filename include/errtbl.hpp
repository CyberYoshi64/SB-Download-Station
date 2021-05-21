#ifndef ERRTBL_HPP
#define ERRTBL_HPP
#include <3ds.h>
#include <string>

// language pack sorting ID names
#define LNGTXT_NULL 0
#define LNGTXT_APPNAME 1
#define LNGTXT_EXIT 2
#define LNGTXT_BACK 3
#define LNGTXT_NEXT 4
#define LNGTXT_CONTINUE 5
#define LNGTXT_YES 6
#define LNGTXT_NO 7
#define LNGTXT_CANCEL 8

#define LNGTXT_CHARGE 1000
#define LNGTXT_NOT_CHARGING 1000
#define LNGTXT_CHARGING 1002
#define LNGTXT_CONNECT 1003
#define LNGTXT_NOT_CONNECTED 1004
#define LNGTXT_CONNECTED 1005

#define LNGTXT_MAIN_MENU_TXT 5000
#define LNGTXT_PLSWAIT 5001
#define LNGTXT_SETTINGS_TXT 5002
#define LNGTXT_INSTALLING_TXT 5003
#define LNGTXT_UPDATING_TXT 5004

#define LNGTXT_MNG_PRJ 10000
#define LNGTXT_DOWNLOAD_PRJ 10001
#define LNGTXT_UPDATE_APP 10002

#define LNGTXT_ABOUT 15000
#define LNGTXT_ABOUT_SQUAT 15001
#define LNGTXT_ABOUT_CY64 15002
#define LNGTXT_ABOUT_SRCCODE 15003
#define LNGTXT_ABOUT_SMILEBASIC 15004
#define LNGTXT_ABOUT_BUILD 15005

#define LNGTXT_PROCEED_ASK 16000

// functions and their definitions
const char* AppErrTbl(int errorcode, u8 lang);
const char* LanguageStringFromInt(u8 lang);
const char* LanguageStrFromInt(u8 lang);
u8 LanguageStrFromInt(const char* lang);
const char* LngpackStr(int lngid, int lang);
const char* SysErr_ModuleStr(int id);
const char* SysErr_LevelStr(int id);
const char* SysErr_DescStr(int id);
const char* SysErr_SummaryStr(int id);
#endif

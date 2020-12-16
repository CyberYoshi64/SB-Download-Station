#ifndef ERRTBL_HPP
#define ERRTBL_HPP
#include <3ds.h>
#include <string>

// language pack sorting ID names
#define LNGTXT_NULL 0                        // dummy entry
#define LNGTXT_TOPMENU 1                     // "Top menu"
#define LNGTXT_EXIT 2                        // "Press HOME to exit."
#define LNGTXT_BACK 3                        // "Back"
#define LNGTXT_NEXT 4                        // "Next"
#define LNGTXT_CONTINUE 5                    // "Continue"
#define LNGTXT_YES 6                         // "Yes"
#define LNGTXT_NO 7                          // "No"
#define LNGTXT_CANCEL 8                      // "Cancel"

#define LNGTXT_STORE_PRJ 1000                // "Backup a project"
#define LNGTXT_DOWNLOAD_PRJ 1001             // "Download menu"
#define LNGTXT_UPDATE_APP 1002               // "Check for updates"

#define LNGTXT_ABOUT 1500                    // "About this app"
#define LNGTXT_ABOUT_SQUAT 1501              // "Original idea: the_squat1115"
#define LNGTXT_ABOUT_CY64 1502               // "App programmer: CyberYoshi64"
#define LNGTXT_ABOUT_SRCCODE 1503            // "Certain pieces of the source code are from:"
#define LNGTXT_ABOUT_SMILEBASIC 1504         // "SmileBASIC is a trademark of SmileBoom Co.Ltd."
#define LNGTXT_ABOUT_BUILD 1505              // "Built with devkitPro"

#define NIEK_HAT_STERN_GESAGT 2000           // ;)
#define L_IS_REAL_2501 2001                  // ;)

#define LNGTXT_PROCEED_ASK 3000              // Asking to proceed

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

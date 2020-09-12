#include "animation.hpp"
#include "cia.hpp"
#include "download.hpp"
#include "errtbl.hpp"
#include "keyboard.h"
#include "formatting.hpp"
#include "gui.hpp"
#include "inifile.h"
#include "keyboard.h"
#include "thread.h"

#include <fstream>
#include <sys/stat.h>
#include <vector>
#include <unistd.h>

#define  USER_AGENT   "SmileBASIC Download Station - 0.0.1"

CIniFile versionsFile("sdmc:/SBDownloadStation/data/currentver.ini");

static char* result_buf = NULL;
static size_t result_sz = 0;
static size_t result_written = 0;
std::vector<std::string> _topText;
std::string jsonName;
std::string usernamePasswordCache;
extern C3D_RenderTarget* top;
extern C3D_RenderTarget* bottom;
extern int errorcode;
extern bool updateAvailable;

bool showProgressBar;
u32 size;
u32 contentsize;
int progressBarType=0;
char progressBarMsg[2048];
char downloadmsg[256];

extern u64 installOffset,installSize;


Result downloadToFile(const char* url, const char* outfile)
{
    Result ret=0;
    httpcContext context;
    char *newurl=NULL;
    u32 statuscode=0;
    u32 readsize=0;
    contentsize=1;
    size=0;
    u8 *buf, *lastbuf;

    do {
        ret = httpcOpenContext(&context, HTTPC_METHOD_GET, url, 1);

        // This disables SSL cert verification, so https:// will be usable
        ret = httpcSetSSLOpt(&context, SSLCOPT_DisableVerify);

        // Enable Keep-Alive connections
        ret = httpcSetKeepAlive(&context, HTTPC_KEEPALIVE_ENABLED);

        // Set a User-Agent header so websites can identify your application
        ret = httpcAddRequestHeaderField(&context, "User-Agent", "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/83.0.4103.116 Safari/537.36");

        // Tell the server we can support Keep-Alive connections.
        // This will delay connection teardown momentarily (typically 5s)
        // in case there is another request made to the same server.
        ret = httpcAddRequestHeaderField(&context, "Connection", "Keep-Alive");

        ret = httpcBeginRequest(&context);
        if(ret!=0){
        	showError("HTTP Request couldn't be started.");
            httpcCloseContext(&context);
            if(newurl!=NULL) free(newurl);
            return ret;
        }

        ret = httpcGetResponseStatusCode(&context, &statuscode);
        if(ret!=0){
        	sprintf(downloadmsg,"Error summary (%lx):\n\nModule: %s \nLevel: %s \nSummary: %s \nDesc.: %s ",ret,SysErr_ModuleStr(ret),SysErr_LevelStr(ret),SysErr_SummaryStr(ret),SysErr_DescStr(ret));
        	showError(downloadmsg);
            httpcCloseContext(&context);
            if(newurl!=NULL) free(newurl);
            return ret;
        }

        if ((statuscode >= 301 && statuscode <= 303) || (statuscode >= 307 && statuscode <= 308)) {
            if(newurl==NULL) newurl = (char*)malloc(0x1000); // One 4K page for new URL
            if (newurl==NULL){
            	showError("Relocation of URL has not succeeded");
                httpcCloseContext(&context);
                return -1;
            }
            ret = httpcGetResponseHeader(&context, "Location", newurl, 0x1000);
            url = newurl; // Change pointer to the url that we just learned
            
            httpcCloseContext(&context); // Close this context before we try the next
        }
    } while ((statuscode >= 301 && statuscode <= 303) || (statuscode >= 307 && statuscode <= 308));

    if(statuscode!=200 && statuscode!=203){
        sprintf(downloadmsg,"HTTP response code %ld received. HTTP 200 was expected.",statuscode);
        showError(downloadmsg);
        httpcCloseContext(&context);
        if(newurl!=NULL) free(newurl);
        return -2;
    }

    // This relies on an optional Content-Length header and may be 0
    ret=httpcGetDownloadSizeState(&context, NULL, &contentsize);
    if(ret!=0){
    	showError("Couldn't receive Content-Length from server.");
        httpcCloseContext(&context);
        if(newurl!=NULL) free(newurl);
        return ret;
    }

    // Start with a single page buffer
    buf = (u8*)malloc(0x1000);
    if(buf==NULL){
    	showError("Could not malloc( 4096 ) the buffer.");
        httpcCloseContext(&context);
        if(newurl!=NULL) free(newurl);
        return -1;
    }

    do {
        // This download loop resizes the buffer as data is read.
        ret = httpcDownloadData(&context, buf+size, 0x1000, &readsize);
        size += readsize; 
        if (ret == (s32)HTTPC_RESULTCODE_DOWNLOADPENDING){
            lastbuf = buf; // Save the old pointer, in case realloc() fails.
            buf = (u8*)realloc(buf, size + 0x1000);
            if(buf==NULL){
            	showError("Could not reallocate the buffer with the new size.");
                httpcCloseContext(&context);
                free(lastbuf);
                if(newurl!=NULL) free(newurl);
                return -1;
            }
        }
    } while (ret == (s32)HTTPC_RESULTCODE_DOWNLOADPENDING); 

    if(ret!=0){
    	showError("Expected a different return value from httpcDownloadData().");
        httpcCloseContext(&context);
        if(newurl!=NULL) free(newurl);
        free(buf);
        return -1;
    }

    FILE* out = fopen(outfile, "wb");
    fwrite(buf, 1, size, out);
    fclose(out);

    httpcCloseContext(&context);
    free(buf);
    if (newurl!=NULL) free(newurl);

    return 0;
}
/**
 * Check Wi-Fi status.
 * @return True if Wi-Fi is connected; false if not.
 */
bool checkWifiStatus(void) {
	// return true; // Citra
	u32 wifiStatus;
	bool res = false;

	if(R_SUCCEEDED(ACU_GetWifiStatus(&wifiStatus)) && wifiStatus) {
		res = true;
	}

	return res;
}

void downloadFailed(void) {
	errorcode=10000007;
}

void notConnectedMsg(void) {
	errorcode=10000008;
}

void doneMsg(void) {
	errorcode=1;
}

void setMessageText(const std::string &text) {
	std::string _topTextStr(text);
	std::vector<std::string> words;
	std::size_t pos;
	// std::replace( _topTextStr.begin(), _topTextStr.end(), '\n', ' ');
	_topTextStr.erase(std::remove(_topTextStr.begin(), _topTextStr.end(), '\r'), _topTextStr.end());
	while((pos = _topTextStr.find(' ')) != std::string::npos) {
		words.push_back(_topTextStr.substr(0, pos));
		_topTextStr = _topTextStr.substr(pos + 1);
	}
	if(_topTextStr.size())
		words.push_back(_topTextStr);
	std::string temp;
	_topText.clear();
	for(auto word : words) {
		int width = Draw_GetTextWidth(0.5f, (temp + " " + word).c_str());
		if(word.find('\n') != -1u)
		{
			word.erase(std::remove(word.begin(), word.end(), '\n'), word.end());
			temp += " " + word;
			_topText.push_back(temp);
			temp = "";
		}
		else if(width > 256)
		{
			_topText.push_back(temp);
			temp = word;
		}
		else
		{
			temp += " " + word;
		}
	}
	if(temp.size())
		_topText.push_back(temp);
}

void drawMessageText(int position, bool showExitText) {
	Gui::clearTextBufs();
	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
	C2D_TargetClear(bottom, 0xFF403020);
	set_screen(bottom);
	Draw_Text(18, 24, .7, BLACK, jsonName.c_str());
	for (int i = 0; i < (int)_topText.size() && i < (showExitText ? 9 : 10); i++) {
		Draw_Text(24, ((i * 16) + 48), 0.5f, BLACK, _topText[i+position].c_str());
	}
	if(showExitText)
		Draw_Text(24, 200, 0.5f, BLACK, " Cancel    Update");
	C3D_FrameEnd(0);
}


void displayProgressBar() {
	char str[256];
	while(showProgressBar) {
		if (contentsize < 1.0f) {
			contentsize = 1.0f;
		}
		if (contentsize < size) {
			contentsize = size;
		}

		// Downloading.
		if (progressBarType == 0){
			snprintf(str, sizeof(str), "%s / %s (%.2f%%)",
					formatBytes(size).c_str(),
					formatBytes(contentsize).c_str(),
					((float)size/(float)contentsize) * 100.0f);
					// Extracting.
		} else if (progressBarType == 1) {
			snprintf(str, sizeof(str), "%s / %s (%.2f%%)",
					formatBytes(installOffset).c_str(),
					formatBytes(installSize).c_str(),
					((float)installOffset/((float)installSize + 0.0000001f)) * 100.0f);
		};

		Gui::clearTextBufs();
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_TargetClear(bottom, 0xFF806030);
		set_screen(bottom);
		Draw_Text_Center(160, 40, 0.60f, BLACK, progressBarMsg);
		// Only display this by downloading.
		if (progressBarType == 0) {
			Draw_Text_Center(160, 100, 0.6f, BLACK, str);
			// Outline of progressbar.
			Draw_Rect(60, 169, 202, 30, BLACK);
			Animation::DrawProgressBar(size, contentsize);
		}
		// Only Display this by extracting.
		if (progressBarType == 1) {
			// Text.
			Draw_Text_Center(160, 100, 0.4f, BLACK, str);
			// Outline of progressbar.
			Draw_Rect(60, 169, 202, 30, BLACK);
			Animation::DrawProgressBar((float)installOffset, (float)installSize);
		}
		C3D_FrameEnd(0);
		gspWaitForVBlank();
	}
}

bool promtUsernamePassword(void) {
	Msg::DisplayMsg("The GitHub API rate limit has been\n"
					 "exceeded for your IP. You can regain\n"
					 "access by signing in to a GitHub account,\n"
					 "or waiting for a bit.\n"
					 "(or press  but some things won't work)\n\n\n\n\n\n\n\n"
					 " Cancel    Authenticate");

	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
	set_screen(bottom);
	Draw_Text(20, 100, 0.45f, BLACK, "Username:");
	Draw_Rect(100, 100, 100, 14, GRAY);
	Draw_Text(20, 120, 0.45f, BLACK, "Password:");
	Draw_Rect(100, 120, 100, 14, GRAY);

	Draw_Rect(100, 140, 14, 14, GRAY);
	Draw_Text(120, 140, 0.45f, BLACK, "Save login?");
	C3D_FrameEnd(0);

	bool save = false;
	int hDown;
	std::string username, password;
	while(1) {
		do {
			gspWaitForVBlank();
			hidScanInput();
			hDown = hidKeysDown();
		} while(!hDown);

		if(hDown & KEY_A) {
			usernamePasswordCache = username + ":" + password;
			return save;
		} else if(hDown & KEY_B) {
			usernamePasswordCache = "";
			return save;
		} else if(hDown & KEY_TOUCH) {
			touchPosition touch;
			touchRead(&touch);
			if(touch.px >= 100 && touch.px <= 200 && touch.py >= 100 && touch.py <= 114) {
				username = keyboardInput("Username","");
				C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
				set_screen(bottom);
				Draw_Rect(100, 100, 100, 14, GRAY);
				Draw_Text(100, 100, 0.45f, BLACK, username.c_str());
				C3D_FrameEnd(0);
			} else if(touch.px >= 100 && touch.px <= 200 && touch.py >= 120 && touch.py <= 134) {
				password = keyboardInput("Password","",64,1);
				C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
				set_screen(bottom);
				Draw_Rect(100, 120, 100, 14, GRAY);
				Draw_Text(100, 120, 0.45f, BLACK, password.c_str());
				C3D_FrameEnd(0);
			} else if(touch.px >= 100 && touch.px <= 114 && touch.py >= 140 && touch.py <= 154) {
				save = !save;
				C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
				set_screen(bottom);
				Draw_Rect(102, 142, 10, 10, save ? GREEN : GRAY);
				C3D_FrameEnd(0);
			}
		}
	}
}

void loadUsernamePassword(void) {
	std::ifstream in("sdmc:/SBDownloadStation/data/usernamePassword");
	if(in.good())	in >> usernamePasswordCache;
	in.close();
}

void saveUsernamePassword() {
	std::ofstream out("sdmc:/SBDownloadStation/data/usernamePassword");
	if(out.good())	out << usernamePasswordCache;
	out.close();
}

void saveUpdateData(void) {
	versionsFile.SaveIniFile("sdmc:/SBDownloadStation/data/currentver.ini");
}

void updateApp(std::string commit) {
		showProgressBar = true;
		progressBarType = 0;
		createThread((ThreadFunc)displayProgressBar);
		snprintf(progressBarMsg, sizeof(progressBarMsg), "Downloading file list...");
		if(downloadToFile("https://github.com/CyberYoshi64/SKKBAUI-prv/raw/master/raw/T.SYS_LAUNCHER", "/SBDownloadStation/cache/T.SYS_LAUNCHER") != 0) {
			//showProgressBar = false;
			downloadFailed();
		}
		
		snprintf(progressBarMsg, sizeof(progressBarMsg), "Downloading newest release...");
		if(downloadToFile("http://sbapi.me/get/DKA8N3GF/info?json=1", "/SBDownloadStation/cache/files.json") != 0) {
			//showProgressBar = false;
			downloadFailed();
		}

		snprintf(progressBarMsg, sizeof(progressBarMsg), "Downloading newest release...");
		if(downloadToFile("http://github.com/CyberYoshi64/SKKBAUI-prv/raw/master/raw/T__SHELL", "/SBDownloadStation/cache/T__SHELL") != 0) {
			//showProgressBar = false;
			downloadFailed();
		}

		snprintf(progressBarMsg, sizeof(progressBarMsg), "Downloading newest release...");
		if(downloadToFile("http://github.com/CyberYoshi64/SKKBAUI-prv/raw/master/raw/T_______LNC_SET", "/SBDownloadStation/cache/T_______LNC_SET") != 0) {
			showProgressBar = false;
			downloadFailed();
			return;
		}
		
		snprintf(progressBarMsg, sizeof(progressBarMsg), "Installing newest release...");
		progressBarType = 1;
		//installCia("/SBDownloadStation/cache/SB3_DS.cia", false);
		showProgressBar = false;
		//deleteFile("sdmc:/SBDownloadStation/cache/lol.7z");
		//deleteFile("sdmc:/SBDownloadStation/cache/SB3_DS.cia");

		updateAvailable = false;
		doneMsg();
}

#include "animation.hpp"
#include "cia.hpp"
#include "download.hpp"
//#include "extract.hpp"
#include "formatting.hpp"
#include "gui.hpp"
#include "init.hpp"
#include "inifile.h"
#include "keyboard.h"
#include "thread.h"

#include <fstream>
#include <sys/stat.h>
#include <vector>
#include <unistd.h>

#include <curl/curl.h>
#include <curl/easy.h>

#define  USER_AGENT   "SmileBASIC Download Station - 0.0.2"

static char* result_buf = NULL;
static size_t result_sz = 0;
static size_t result_written = 0;
std::vector<std::string> _topText;
std::string jsonName;
CIniFile versionsFile("sdmc:/SB-Download-Station/data/currentVersion.ini");
std::string latestMenuReleaseCache = "";
std::string latestMenuNightlyCache = "";
std::string usernamePasswordCache = "";

// Metadata for titles
extern int meta_total;
extern std::string meta_prjn[];
extern std::string meta_title[];
extern std::string meta_desc[];
extern int meta_ver[];

// External stuff
extern bool exiting;
extern int errorcode;
extern int waitposx;
extern int waitposy;
extern C3D_RenderTarget* top;
extern C3D_RenderTarget* bottom;
extern bool updateAvailable[];
extern bool updated3dsx;
extern bool waiting;
//extern int filesExtracted;
//extern std::string extractingFile;

char progressBarMsg[128] = "";
bool showProgressBar = false;
int progressBarType = 0; // 0 = Download | 1 = Install

// That are our extract Progressbar variables.
//extern u64 extractSize, writeOffset;
// That are our install Progressbar variables.
extern u64 installSize, installOffset;

curl_off_t downloadTotal = 1; //Dont initialize with 0 to avoid division by zero later
curl_off_t downloadNow = 0;

static FILE *downfile = NULL;
static size_t file_buffer_pos = 0;
static size_t file_toCommit_size = 0;
static char* g_buffers[2] = { NULL };
static u8 g_index = 0;
static Thread fsCommitThread;
static LightEvent readyToCommit;
static LightEvent waitCommit;
static bool killThread = false;
static bool writeError = false;
#define FILE_ALLOC_SIZE 0x60000

static int curlProgress(CURL *hnd,
					curl_off_t dltotal, curl_off_t dlnow,
					curl_off_t ultotal, curl_off_t ulnow)
{
	downloadTotal = dltotal;
	downloadNow = dlnow;

	return 0;
}

bool filecommit() {
	if (!downfile) return false;
	fseek(downfile, 0, SEEK_END);
	u32 byteswritten = fwrite(g_buffers[!g_index], 1, file_toCommit_size, downfile);
	if (byteswritten != file_toCommit_size) return false;
	file_toCommit_size = 0;
	return true;
}

static void commitToFileThreadFunc(void* args) {
	LightEvent_Signal(&waitCommit);
	while (true) {
		LightEvent_Wait(&readyToCommit);
		LightEvent_Clear(&readyToCommit);
		if (killThread) threadExit(0);
		writeError = !filecommit();
		LightEvent_Signal(&waitCommit);
	}
}

static size_t file_handle_data(char *ptr, size_t size, size_t nmemb, void *userdata) {
	(void)userdata;
	const size_t bsz = size * nmemb;
	size_t tofill = 0;
	if (writeError) return 0;
	if (!g_buffers[g_index]) {

		LightEvent_Init(&waitCommit, RESET_STICKY);
		LightEvent_Init(&readyToCommit, RESET_STICKY);

		s32 prio = 0;
		svcGetThreadPriority(&prio, CUR_THREAD_HANDLE);
		fsCommitThread = threadCreate(commitToFileThreadFunc, NULL, 0x1000, prio - 1, -2, true);

		g_buffers[0] = (char*)memalign(0x1000, FILE_ALLOC_SIZE);
		g_buffers[1] = (char*)memalign(0x1000, FILE_ALLOC_SIZE);

		if (!fsCommitThread || !g_buffers[0] || !g_buffers[1]) return 0;
	}
	if (file_buffer_pos + bsz >= FILE_ALLOC_SIZE) {
		tofill = FILE_ALLOC_SIZE - file_buffer_pos;
		memcpy(g_buffers[g_index] + file_buffer_pos, ptr, tofill);
		
		LightEvent_Wait(&waitCommit);
		LightEvent_Clear(&waitCommit);
		file_toCommit_size = file_buffer_pos + tofill;
		file_buffer_pos = 0;
		svcFlushProcessDataCache(CUR_PROCESS_HANDLE, (u32)g_buffers[g_index], file_toCommit_size);
		g_index = !g_index;
		LightEvent_Signal(&readyToCommit);
	}
	memcpy(g_buffers[g_index] + file_buffer_pos, ptr + tofill, bsz - tofill);
	file_buffer_pos += bsz - tofill;
	return bsz;
}

Result downloadToFile(std::string url, std::string path) {

	Result retcode = 0;
	downloadTotal = 1;
	downloadNow = 0;
	int res;
	CURL *hnd;
	CURLcode cres;

	printf("Downloading from:\n%s\nto:\n%s\n", url.c_str(), path.c_str());
	const char* filepath = path.c_str();

	void *socubuf = memalign(0x1000, 0x100000);
	if (!socubuf) {
		retcode = -1;
		goto exit;
	}
	
	res = socInit((u32*)socubuf, 0x100000);
	if (R_FAILED(res)) {
		retcode = res;
		goto exit;
	}

	makeDirs(strdup(filepath));
	downfile = fopen(filepath, "wb");
	if (!downfile) {
		retcode = -2;
		goto exit;
	}

	hnd = curl_easy_init();
	curl_easy_setopt(hnd, CURLOPT_BUFFERSIZE, FILE_ALLOC_SIZE);
	curl_easy_setopt(hnd, CURLOPT_URL, url.c_str());
	curl_easy_setopt(hnd, CURLOPT_NOPROGRESS, 0L);
	curl_easy_setopt(hnd, CURLOPT_USERAGENT, USER_AGENT);
	curl_easy_setopt(hnd, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(hnd, CURLOPT_FAILONERROR, 1L);
	curl_easy_setopt(hnd, CURLOPT_ACCEPT_ENCODING, "gzip");
	curl_easy_setopt(hnd, CURLOPT_MAXREDIRS, 50L);
	curl_easy_setopt(hnd, CURLOPT_XFERINFOFUNCTION, curlProgress);
	curl_easy_setopt(hnd, CURLOPT_HTTP_VERSION, (long)CURL_HTTP_VERSION_2TLS);
	curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, file_handle_data);
	curl_easy_setopt(hnd, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(hnd, CURLOPT_VERBOSE, 1L);
	curl_easy_setopt(hnd, CURLOPT_STDERR, stdout);

	cres = curl_easy_perform(hnd);
	curl_easy_cleanup(hnd);
	
	if (cres != CURLE_OK) {
		retcode = -cres;
		goto exit;
	}

	LightEvent_Wait(&waitCommit);
	LightEvent_Clear(&waitCommit);

	file_toCommit_size = file_buffer_pos;
	svcFlushProcessDataCache(CUR_PROCESS_HANDLE, (u32)g_buffers[g_index], file_toCommit_size);
	g_index = !g_index;
	if (!filecommit()) {
		retcode = -3;
		goto exit;
	}
	fflush(downfile);
	
exit:
	if (fsCommitThread) {
		killThread = true;
		LightEvent_Signal(&readyToCommit);
		threadJoin(fsCommitThread, U64_MAX);
		killThread = false;
		fsCommitThread = NULL;
	}

	socExit();
	
	if (socubuf) {
		free(socubuf);
	}
	if (downfile) {
		fclose(downfile);
		downfile = NULL;
	}
	if (g_buffers[0]) {
		free(g_buffers[0]);
		g_buffers[0] = NULL;
	}
	if (g_buffers[1]) {
		free(g_buffers[1]);
		g_buffers[1] = NULL;
	}
	g_index = 0;
	file_buffer_pos = 0;
	file_toCommit_size = 0;
	writeError = false;
	
	return retcode;
}

// following function is from
// https://github.com/angelsl/libctrfgh/blob/master/curl_test/src/main.c
static size_t handle_data(char* ptr, size_t size, size_t nmemb, void* userdata)
{
	(void) userdata;
	const size_t bsz = size*nmemb;

	if (result_sz == 0 || !result_buf)
	{
		result_sz = 0x1000;
		result_buf = (char*)malloc(result_sz);
	}

	bool need_realloc = false;
	while (result_written + bsz > result_sz)
	{
		result_sz <<= 1;
		need_realloc = true;
	}

	if (need_realloc)
	{
		char *new_buf = (char*)realloc(result_buf, result_sz);
		if (!new_buf)
		{
			return 0;
		}
		result_buf = new_buf;
	}

	if (!result_buf)
	{
		return 0;
	}

	memcpy(result_buf + result_written, ptr, bsz);
	result_written += bsz;
	return bsz;
}

static Result setupContext(CURL *hnd, const char * url) {
	downloadTotal = 1;
	downloadNow = 0;
	curl_easy_setopt(hnd, CURLOPT_XFERINFOFUNCTION, curlProgress);

	curl_easy_setopt(hnd, CURLOPT_BUFFERSIZE, 102400L);
	curl_easy_setopt(hnd, CURLOPT_URL, url);
	curl_easy_setopt(hnd, CURLOPT_NOPROGRESS, 0L);
	curl_easy_setopt(hnd, CURLOPT_USERAGENT, USER_AGENT);
	curl_easy_setopt(hnd, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(hnd, CURLOPT_MAXREDIRS, 50L);
	curl_easy_setopt(hnd, CURLOPT_HTTP_VERSION, (long)CURL_HTTP_VERSION_2TLS);
	curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, handle_data);
	curl_easy_setopt(hnd, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(hnd, CURLOPT_VERBOSE, 1L);
	curl_easy_setopt(hnd, CURLOPT_STDERR, stdout);
	if(usernamePasswordCache != "")	curl_easy_setopt(hnd, CURLOPT_USERPWD, usernamePasswordCache.c_str());

	return 0;
}

Result downloadFromRelease(std::string url, std::string asset, std::string path) {
	Result ret = 0;
	void *socubuf = memalign(0x1000, 0x100000);
	if(!socubuf) {
		return -1;
	}

	ret = socInit((u32*)socubuf, 0x100000);
	if(R_FAILED(ret)) {
		free(socubuf);
		return ret;
	}

	std::regex parseUrl("github\\.com\\/(.+)\\/(.+)");
	std::smatch result;
	regex_search(url, result, parseUrl);

	std::string repoOwner = result[1].str(), repoName = result[2].str();

	std::stringstream apiurlStream;
	apiurlStream << "https://api.github.com/repos/" << repoOwner << "/" << repoName << "/releases/latest";
	std::string apiurl = apiurlStream.str();

	printf("Downloading latest release from repo:\n%s\nby:\n%s\n", repoName.c_str(), repoOwner.c_str());
	printf("Crafted API url:\n%s\n", apiurl.c_str());

	CURL *hnd = curl_easy_init();
	ret = setupContext(hnd, apiurl.c_str());
	if(ret != 0) {
		socExit();
		free(result_buf);
		free(socubuf);
		result_buf = NULL;
		result_sz = 0;
		result_written = 0;
		return ret;
	}

	CURLcode cres = curl_easy_perform(hnd);
	curl_easy_cleanup(hnd);
	char* newbuf = (char*)realloc(result_buf, result_written + 1);
	result_buf = newbuf;
	result_buf[result_written] = 0; //nullbyte to end it as a proper C style string

	if(cres != CURLE_OK) {
		printf("Error in:\ncurl\n");
		socExit();
		free(result_buf);
		free(socubuf);
		result_buf = NULL;
		result_sz = 0;
		result_written = 0;
		return -1;
	}

	printf("Looking for asset with name matching:\n%s\n", asset.c_str());
	std::string assetUrl;
	json parsedAPI = json::parse(result_buf);
	if(parsedAPI["assets"].is_array()) {
		for (auto jsonAsset : parsedAPI["assets"]) {
			if(jsonAsset.is_object() && jsonAsset["name"].is_string() && jsonAsset["browser_download_url"].is_string()) {
				std::string assetName = jsonAsset["name"];
				if(matchPattern(asset, assetName)) {
					assetUrl = jsonAsset["browser_download_url"];
					break;
				}
			}
		}
	}
	socExit();
	free(result_buf);
	free(socubuf);
	result_buf = NULL;
	result_sz = 0;
	result_written = 0;

	if(assetUrl.empty())
		ret = DL_ERROR_GIT;
	else
		ret = downloadToFile(assetUrl, path);

	return ret;
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

void downloadFailed(Result res) {
	errorcode=10010000+res;
}

void notConnectedMsg(void) {
	errorcode=10010006;
}

void doneMsg(void) {
	showError("Download was successful.\n\nThe app will restart, once the new version is installed.",9999999);
}

std::string getLatestRelease(std::string repo, std::string item, bool retrying) {
	Result ret = 0;
	void *socubuf = memalign(0x1000, 0x100000);
	if(!socubuf) {
		return "";
	}

	ret = socInit((u32*)socubuf, 0x100000);
	if(R_FAILED(ret)) {
		free(socubuf);
		return "";
	}

	std::stringstream apiurlStream;
	apiurlStream << "https://api.github.com/repos/" << repo << "/releases/latest";
	std::string apiurl = apiurlStream.str();

	CURL *hnd = curl_easy_init();
	ret = setupContext(hnd, apiurl.c_str());
	if(ret != 0) {
		socExit();
		free(result_buf);
		free(socubuf);
		result_buf = NULL;
		result_sz = 0;
		result_written = 0;
		return "";
	}

	CURLcode cres = curl_easy_perform(hnd);
	int httpCode;
	curl_easy_getinfo(hnd, CURLINFO_RESPONSE_CODE, &httpCode);
	curl_easy_cleanup(hnd);
	char* newbuf = (char*)realloc(result_buf, result_written + 1);
	result_buf = newbuf;
	result_buf[result_written] = 0; //nullbyte to end it as a proper C style string

	if(httpCode == 401 || httpCode == 403) {
		bool save = promtUsernamePassword();
		if(save)	saveUsernamePassword();
		socExit();
		free(result_buf);
		free(socubuf);
		result_buf = NULL;
		result_sz = 0;
		result_written = 0;
		if(usernamePasswordCache == "" || retrying) {
			usernamePasswordCache = "";
			return "API";
		} else {
			return getLatestRelease(repo, item, true);
		}
	}

	if(cres != CURLE_OK) {
		socExit();
		free(result_buf);
		free(socubuf);
		result_buf = NULL;
		result_sz = 0;
		result_written = 0;
		return "";
	}


	std::string jsonItem;
	json parsedAPI = json::parse(result_buf);
	if(parsedAPI[item].is_string()) {
		jsonItem = parsedAPI[item];
	}

	socExit();
	free(result_buf);
	free(socubuf);
	result_buf = NULL;
	result_sz = 0;
	result_written = 0;

	return jsonItem;
}

std::vector<std::string> getRecentCommits(std::string repo, std::string item, bool retrying) {
	std::vector<std::string> emptyVector;
	Result ret = 0;
	void *socubuf = memalign(0x1000, 0x100000);
	if(!socubuf) {
		return emptyVector;
	}

	ret = socInit((u32*)socubuf, 0x100000);
	if(R_FAILED(ret)) {
		free(socubuf);
		return emptyVector;
	}

	std::stringstream apiurlStream;
	apiurlStream << "https://api.github.com/repos/" << repo << "/commits";
	std::string apiurl = apiurlStream.str();

	CURL *hnd = curl_easy_init();
	ret = setupContext(hnd, apiurl.c_str());
	if(ret != 0) {
		socExit();
		free(result_buf);
		free(socubuf);
		result_buf = NULL;
		result_sz = 0;
		result_written = 0;
		return emptyVector;
	}

	CURLcode cres = curl_easy_perform(hnd);
	int httpCode;
	curl_easy_getinfo(hnd, CURLINFO_RESPONSE_CODE, &httpCode);
	curl_easy_cleanup(hnd);
	char* newbuf = (char*)realloc(result_buf, result_written + 1);
	result_buf = newbuf;
	result_buf[result_written] = 0; //nullbyte to end it as a proper C style string

	if(httpCode == 401 || httpCode == 403) {
		bool save = promtUsernamePassword();
		if(save)	saveUsernamePassword();
		socExit();
		free(result_buf);
		free(socubuf);
		result_buf = NULL;
		result_sz = 0;
		result_written = 0;
		if(usernamePasswordCache == "" || retrying) {
			usernamePasswordCache = "";
			return {"API"};
		} else {
			return getRecentCommits(repo, item, true);
		}
	}

	if(cres != CURLE_OK) {
		printf("Error in:\ncurl\n");
		socExit();
		free(result_buf);
		free(socubuf);
		result_buf = NULL;
		result_sz = 0;
		result_written = 0;
		return emptyVector;
	}

	std::vector<std::string> jsonItems;
	json parsedAPI = json::parse(result_buf);
	for(uint i=0;i<parsedAPI.size();i++) {
		if(parsedAPI[i][item].is_string()) {
			jsonItems.push_back(parsedAPI[i][item]);
		}
	}

	socExit();
	free(result_buf);
	free(socubuf);
	result_buf = NULL;
	result_sz = 0;
	result_written = 0;

	return jsonItems;
}

std::vector<std::string> getRecentCommitsArray(std::string repo, std::string array, std::string item, bool retrying) {
	std::vector<std::string> emptyVector;
	Result ret = 0;
	void *socubuf = memalign(0x1000, 0x100000);
	if(!socubuf) {
		return emptyVector;
	}

	ret = socInit((u32*)socubuf, 0x100000);
	if(R_FAILED(ret)) {
		free(socubuf);
		return emptyVector;
	}

	std::stringstream apiurlStream;
	apiurlStream << "https://api.github.com/repos/" << repo << "/commits";
	std::string apiurl = apiurlStream.str();

	CURL *hnd = curl_easy_init();
	ret = setupContext(hnd, apiurl.c_str());
	if(ret != 0) {
		socExit();
		free(result_buf);
		free(socubuf);
		result_buf = NULL;
		result_sz = 0;
		result_written = 0;
		return emptyVector;
	}

	CURLcode cres = curl_easy_perform(hnd);
	int httpCode;
	curl_easy_getinfo(hnd, CURLINFO_RESPONSE_CODE, &httpCode);
	curl_easy_cleanup(hnd);
	char* newbuf = (char*)realloc(result_buf, result_written + 1);
	result_buf = newbuf;
	result_buf[result_written] = 0; //nullbyte to end it as a proper C style string

	if(httpCode == 401 || httpCode == 403) {
		bool save = promtUsernamePassword();
		if(save)	saveUsernamePassword();
		socExit();
		free(result_buf);
		free(socubuf);
		result_buf = NULL;
		result_sz = 0;
		result_written = 0;
		if(usernamePasswordCache == "" || retrying) {
			usernamePasswordCache = "";
			return {"API"};
		} else {
			return getRecentCommitsArray(repo, array, item, true);
		}
	}

	if(cres != CURLE_OK) {
		printf("Error in:\ncurl\n");
		socExit();
		free(result_buf);
		free(socubuf);
		result_buf = NULL;
		result_sz = 0;
		result_written = 0;
		return emptyVector;
	}

	std::vector<std::string> jsonItems;
	json parsedAPI = json::parse(result_buf);
	for(uint i=0;i<parsedAPI.size();i++) {
		if(parsedAPI[i][array][item].is_string()) {
			jsonItems.push_back(parsedAPI[i][array][item]);
		}
	}

	socExit();
	free(result_buf);
	free(socubuf);
	result_buf = NULL;
	result_sz = 0;
	result_written = 0;

	return jsonItems;
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
		if (temp[0] == 32){temp = temp.substr(1,temp.length());}
		int width = Draw_GetTextWidth(0.5f, (temp + " " + word).c_str());
		if(word.find('\n') != -1u)
		{
			word.erase(std::remove(word.begin(), word.end(), '\n'), word.end());
			temp += " " + word;
			_topText.push_back(temp);
			temp = "";
		}
		else if(width > 288)
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

void drawMessageText(int x, int y, int position) {
	set_screen(bottom);
	for (int i = 0; i < (int)_topText.size() && i < 10; i++) {
		DrawStrBox(x, (i * 16) + y, 0.5f, -1, _topText[i+position].c_str(), (320-x*2), 0.975);
	}
}


void displayProgressBar() {
	char str[256];
	while(showProgressBar) {
		if (downloadTotal < 1.0f) {
			downloadTotal = 1.0f;
		}
		if (downloadTotal < downloadNow) {
			downloadTotal = downloadNow;
		}

		// Downloading.
		if (progressBarType == 0){
			snprintf(str, sizeof(str), "%s / %s (%.2f%%)",
					formatBytes(downloadNow).c_str(),
					formatBytes(downloadTotal).c_str(),
					((float)downloadNow/(float)downloadTotal) * 100.0f);
					// Extracting.
		} else if (progressBarType == 2) {
			//snprintf(str, sizeof(str), "%s / %s (%.2f%%)",
			//		formatBytes(writeOffset).c_str(),
			//		formatBytes(extractSize).c_str(),
			//		((float)writeOffset/(float)extractSize) * 100.0f);
			// Installing.
		} else if (progressBarType == 1){
			snprintf(str, sizeof(str), "%s / %s (%.2f%%)",
					formatBytes(installOffset).c_str(),
					formatBytes(installSize).c_str(),
					((float)installOffset/(float)installSize) * 100.0f);
		}

		Gui::clearTextBufs();
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_TargetClear(bottom, 0xff905020);
		set_screen(bottom);
		Draw_Text_Center(160, 40, 0.60f, BLACK, progressBarMsg);
		// Only display this by downloading.
		if (progressBarType == 0) {
			Draw_Text_Center(160, 100, 0.6f, BLACK, str);
			// Outline of progressbar.
			Draw_Rect(60, 169, 202, 30, BLACK);
			Animation::DrawProgressBar(downloadNow, downloadTotal);
		}
		// Only display this by installing.
		if (progressBarType == 1) {
			Draw_Text_Center(160, 100, 0.4f, BLACK, str);
			// Outline of progressbar.
			Draw_Rect(60, 169, 202, 30, BLACK);
			Animation::DrawProgressBar((float)installOffset, (float)installSize);
		}
		C3D_FrameEnd(0);
		gspWaitForVBlank();
	}
}

std::string getProjectList(){

	std::string liststr="";
	std::string str;

	if (downloadToFile("https://raw.githubusercontent.com/CyberYoshi64/SB3-DS-Projects/main/prjlist.txt","sdmc:/SB-Download-Station/cache/prjlist.txt") != 0){
		showError("Project list could not be downloaded.\n\nUsing existing cache.\n(You may not be getting the latest updates of the projects.)",10020000);
	}
	std::ifstream file("sdmc:/SB-Download-Station/cache/prjlist.txt");
	if (file.is_open()){
		while (file.good()){
			getline(file, str);
			liststr += str;
			liststr += "\n";
		}
		file.close();
	}

	return liststr;
}

bool promtUsernamePassword(void) {
	// Msg::DisplayMsg("The GitHub API rate limit has been\n"
	//				 "exceeded for your IP. You can regain\n"
	//				 "access by signing in to a GitHub account,\n"
	//				 "or waiting for a bit.\n"
	//				 "(or press  but some things won't work)\n\n\n\n\n\n\n\n"
	//				 " Cancel    Authenticate");

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
				password = keyboardInput("Password","");
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
	std::ifstream in("sdmc:/SB-Download-Station/data/usernamePassword");
	if(in.good())	in >> usernamePasswordCache;
	in.close();
}

void saveUsernamePassword() {
	std::ofstream out("sdmc:/SB-Download-Station/data/usernamePassword");
	if(out.good())	out << usernamePasswordCache;
	out.close();
}

std::string latestMenuRelease(void) {
	if(latestMenuReleaseCache == "") {
		std::string apiInfo = getLatestRelease("CyberYoshi64/SB-Download-Station", "tag_name");
		if(apiInfo != "API") {
			latestMenuReleaseCache = apiInfo;
		}
	}
	return latestMenuReleaseCache;
}

void saveUpdateData(void) {
	versionsFile.SaveIniFile("sdmc:/SB-Download-Station/data/currentVersion.ini");
}

std::string getInstalledVersion(std::string component) {
	return versionsFile.GetString(component, "VERSION", "");
}

void setInstalledVersion(std::string component, std::string version) {
	versionsFile.SetString(component, "VERSION", version);
}

void checkForUpdates() {
	// First remove the old versions file
	unlink("sdmc:/SB-Download-Station/data/currentVersion.ini");

	std::string menuVersion = getInstalledVersion("SB3-DS");

	updateAvailable[0] = menuVersion != latestMenuRelease();
}

void updateApp(std::string commit) {
		Result downloadres;
		if (!checkWifiStatus()){
			notConnectedMsg();
			return;
		}
		snprintf(progressBarMsg, sizeof(progressBarMsg), "Downloading app (CIA)...");
		showProgressBar = true;
		progressBarType = 0;
		if (strncmp(commit.c_str(),"no-install",10)!=0){createThread((ThreadFunc)displayProgressBar);}
		downloadres = downloadFromRelease("https://github.com/CyberYoshi64/SB-Download-Station", "SB3_Download_Station.cia", "sdmc:/SB-Download-Station/cache/app.cia");
		if(downloadres != 0) {
			showProgressBar = false;
			downloadFailed(downloadres);
			return;
		}

		snprintf(progressBarMsg, sizeof(progressBarMsg), "Downloading app (3DSX)...");
		downloadres = downloadFromRelease("https://github.com/CyberYoshi64/SB-Download-Station", "SB3_Download_Station.3dsx", "sdmc:/3ds/SB-Download-Station.3dsx");
		if(downloadres != 0) {
			showProgressBar = false;
			downloadFailed(downloadres);
			return;
		}
		if (strncmp(commit.c_str(),"no-install",10)!=0){
			snprintf(progressBarMsg, sizeof(progressBarMsg), "Installing CIA...");
			progressBarType = 1;
			doneMsg();
			bool envHbl = !envIsHomebrew();
			Result ciainsterr=installCia("sdmc:/SB-Download-Station/cache/app.cia", envHbl);
			if R_FAILED(ciainsterr){
				showError("CIA failed to install.\n\nPlease try again later.\n\nThe application will restart now.",ciainsterr);
			}
		}
		showProgressBar = false;
		deleteFile("sdmc:/SB-Download-Station/cache/app.cia");
		setInstalledVersion("SB3-DS", latestMenuRelease());
		saveUpdateData();
		updateAvailable[0] = false;
}

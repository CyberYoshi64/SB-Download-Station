#pragma once
#include "common.hpp"
enum DownloadError {
	DL_ERROR_NONE = 0,
	DL_ERROR_WRITEFILE,
	DL_ERROR_ALLOC,
	DL_ERROR_STATUSCODE,
	DL_ERROR_GIT,
};
bool promtUsernamePassword(void);
void loadUsernamePassword(void);
void saveUsernamePassword(void);
Result downloadToFile(const char url, const char path);
bool checkWifiStatus(void);
void notConnectedMsg(void);
void checkForUpdates(void);
void updateApp(std::string commit);

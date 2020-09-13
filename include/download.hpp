#pragma once

#include "common.hpp"

#define APP_TITLE "TWLMenu Updater"

enum DownloadError {
	DL_ERROR_NONE = 0,
	DL_ERROR_WRITEFILE,
	DL_ERROR_ALLOC,
	DL_ERROR_STATUSCODE,
	DL_ERROR_GIT,
};

struct ThemeEntry {
	std::string downloadUrl;
	std::string name;
	std::string path;
	std::string sdPath;
};

// Prompts the user to sign in to GitHub to get past the rate limit
bool promtUsernamePassword(void);

// Loads saved username / password for GitHub API
void loadUsernamePassword(void);

// Saves the username / password for GitHub API
void saveUsernamePassword(void);

Result downloadToFile(std::string url, std::string path);
Result downloadFromRelease(std::string url, std::string asset, std::string path);

/**
 * Check Wi-Fi status.
 * @return True if Wi-Fi is connected; false if not.
 */
bool checkWifiStatus(void);

/**
 * Display "Please connect to Wi-Fi" for 2s.
 */
void notConnectedMsg(void);

/**
 * Get info from the GitHub API about a Release.
 * repo is where to get from. (Ex. "DS-Homebrew/TWiLightMenu")
 * item is that to get from the API. (Ex. "tag_name")
 * @return the string from the API.
 */
std::string getLatestRelease(std::string repo, std::string item, bool retrying = false);

/**
 * Get info from the GitHub API about a Commit.
 * repo is where to get from. (Ex. "DS-Homebrew/TWiLightMenu")
 * item is that to get from the API. (Ex. "sha")
 * @return the string from the API.
 */
std::vector<std::string> getRecentCommits(std::string repo, std::string item, bool retrying = false);

/**
 * Get info from the GitHub API about a Commit.
 * repo is where to get from. (Ex. "DS-Homebrew/TWiLightMenu")
 * array is the array the item is in. (Ex. "commit")
 * item is that to get from the API. (Ex. "message")
 * @return the string from the API.
 */
std::vector<std::string> getRecentCommitsArray(std::string repo, std::string array, std::string item, bool retrying = false);

/**
 * Show the latest release's name and message.
 * repo is where to get from. (Ex. "DS-Homebrew/TWiLightMenu")
 * drawMessageText is whether to show the Cancel/Update text at the bottom
 */
bool showReleaseInfo(std::string repo, bool drawMessageText);

/**
 * Prepare text for showing a release/commit message.
 * text is the text you want to show.
 */
void setMessageText(const std::string &text);

/**
 * Draw text prepared by setMessageText.
 * position is which line start on.
 */
void drawMessageText(int position, bool drawMessageText);

/**
 * Check for updates.
 */
void checkForUpdates(void);

void updateApp(std::string commit);

/**
 * Update the TWiLight Menu++ Updater to the latest build.
 * commit is the TWlBot commit, leave blank for release
 */
void updateSelf(std::string commit);

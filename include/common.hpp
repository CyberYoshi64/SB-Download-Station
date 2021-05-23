#pragma once

#include <3ds.h>

#ifdef __cplusplus
extern "C" {
#endif
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "file.h"

#ifdef __cplusplus
}

#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>

#include "inifile.h"
#include "keyboard.h"
#include "sound.h"
#include "thread.h"
#include "sha256.h"
#include "gfx.hpp"
#include "gui.hpp"
#include "json.hpp"
#include "meta.hpp"
#include "stringutils.hpp"
#include <curl/curl.h>
#include <curl/easy.h>
#include "dialog.hpp"
#include "download.hpp"
#include "cia.hpp"
#include "screen.hpp"
#include "errtbl.hpp"
#include "sb3files.hpp"

// Spritesheets.
#include "button.h"
#include "common.h"
#include "cy64.h"
#include "ghud.h"
#include "scrbg.h"
#include "appdlg.h"

using json = nlohmann::json;

#endif

extern char * arg0;

#define WORKING_DIR       "/3ds/"

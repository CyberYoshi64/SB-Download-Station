#pragma once

#include <3ds.h>
#include <dirent.h>
#include <unistd.h>

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

#include "gfx.hpp"
#include "gui.hpp"
#include "cyvar.hpp"
#include "json.hpp"
#include "msg.hpp"
#include "meta.hpp"
#include <curl/curl.h>
#include <curl/easy.h>
#include "stringutils.hpp"
#include "download.hpp"
#include "inifile.h"
#include "init.hpp"
#include "cia.hpp"
#include "keyboard.h"
#include "sound.h"
#include "thread.h"
#include "screen.hpp"
#include "sb3files.hpp"

using json = nlohmann::json;

#endif

extern char * arg0;

#define WORKING_DIR       "/3ds/"

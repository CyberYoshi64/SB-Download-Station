#pragma once
#include <3ds.h>
#include <string>

#ifndef SB3FILES_HPP
	#define SB3FILES_HPP
	namespace SB3File {
		void GetDate();
		void GetTime();
		u8 GetFileType();
		u8 GetSubType();
		void GetCreator();
		void GetEditor();
	}
#endif
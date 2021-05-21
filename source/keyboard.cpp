#include <stdio.h>
#include <string>
#include <3ds.h>
#include <citro2d.h>
#include <citro3d.h>
#include "screen.hpp"

errorConf kerr;
extern C3D_RenderTarget *top;
extern C3D_RenderTarget *bottom;


/**
 * This method show the software original keyboard from the 3ds OS.
 * @param: const wchar_t* text to show a hint in keyboard
 * @return: std::string with user input
 */

std::string keyboardInput(const char* hint, const char* content, u16 numChars, u8 pswdmode, u32 feat, u32 filter, u8 numDigits, SwkbdValidInput IsValidInput) {
	SwkbdState keyboardState;
	numChars=numChars-1 % 4096;
	char input[numChars];
	swkbdInit(&keyboardState, SWKBD_TYPE_NORMAL, 2, sizeof(input));
	swkbdSetHintText(&keyboardState, hint);
	swkbdSetInitialText(&keyboardState, content);
	swkbdSetPasswordMode(&keyboardState, (SwkbdPasswordMode)pswdmode);
	swkbdSetFeatures(&keyboardState, feat);
	swkbdSetValidation(&keyboardState, IsValidInput, filter, numDigits);
	swkbdInputText(&keyboardState, input, sizeof(input));
	return std::string(input);
}

/**
 * This method show the software original keyboard from the 3ds OS.
 * @param: const char* text to show a hint in keyboard
 * @return: std::string with user input
 */

int keyboardInputInt(const char* hint) {
	SwkbdState keyState;
	char input[4];
	swkbdInit(&keyState, SWKBD_TYPE_NUMPAD, 2, 4);
	swkbdSetHintText(&keyState, hint);

	SwkbdButton pressed = swkbdInputText(&keyState, input, 4);
	int res;
	if(pressed == SWKBD_BUTTON_LEFT) {
		res = 0;
	} else {
		res = strtol(input, NULL, 10);
		if(res > 255)
			res = 255;
	}
	return res;
}

void showError(const char* errstr, int errcode){
	errorType typ=ERROR_TEXT_WORD_WRAP;
	CFG_Language lang=CFG_LANGUAGE_EN;
	errorInit(&kerr, typ, lang);
	errorText(&kerr, errstr);
	if (errcode >= 0 && errorcode <= 10009999)
		errorCode(&kerr,errcode);
	errorDisp(&kerr);
}

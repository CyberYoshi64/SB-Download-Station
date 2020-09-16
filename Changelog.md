# Changelog for SmileBASIC Download Station

## 9/15/2020 - v0.0.3 - Download menu beginning update

* Fix for issue #0: App will crash if DSP failed to dump and a SFX tries to play
* Fix for issue #1: App will crash or freeze when DSP firm is attempted to be dumped (if wasn't not found before)
* Added download menu (WIP)

## 9/13/2020 - v0.0.2 - Main menu update

* Added main menu with 3 buttons (2 of them don't work lol)
* Added update system (which downloads the 3DSX and CIA from here)

## 9/12/2020 - v0.0.1 - Initial start of this project

* Using most code from [TwiLight Menu++ Updater](https://github.com/RocketRobz/TWiLightMenu-Updater).
* Modified keyboard.cpp to add the error applet caller and improved arguments taken by `keyboardInput()`.
* Added errtbl.cpp - Error code table and translation for the text found in the app
* Removed any curl-related code from the sources (I cannot build curl-using projects)
* Removed splash screens from the code.
* Removed GUI and replaced it with a HOME menu-related string.
* Replacing `downloadToFile()` with an httpc-based alternative.

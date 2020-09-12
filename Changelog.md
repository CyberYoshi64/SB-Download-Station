# Changelog for SmileBASIC Download Station

## 9/12/2020 - v0.0.1 - Initial start of this project

* Using most code from [TwiLight Menu++ Updater](https://github.com/RocketRobz/TWiLightMenu-Updater).
* Modified keyboard.cpp to add the error applet caller and improved arguments taken by `keyboardInput()`.
* Added errtbl.cpp - Error code table and translation for the text found in the app
* Removed any curl-related code from the sources (I cannot build curl-using projects)
* Removed splash screens from the code.
* Removed GUI and replaced it with a HOME menu-related string.
* Replacing `downloadToFile()` with an httpc-based alternative.

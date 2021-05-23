# Changelog for SmileBASIC Download Station

## 1/13/2021 - v0.1.0 - App remaster (not ready for Releases)

* GUI is now revamped and running independently of the rest of the app
* Added dialog boxes
  * Can have 0-2 buttons
  * Can wait for a shared boolean to be flipped by a background thread
    * If 1 or 2 buttons are included, background thread can be informed by setting a shared boolean and stopped prematurely
* Currently no features
* Remade graphic assets
* Added functions, like `countThreads()`, `TouchedHitbox()` and `DrawStrCC()`, as well as `SysFontCnvSz()` and `NUMBER_PI`

> ### Planning on:
> * Reimplementing back-end [obvious]
> * Adding custom error applet. See [this mock-up image](https://cdn.discordapp.com/attachments/163074261903343616/822824355142434816/cy64error-mock1.png)
> * Reimplementing languages with "language packs"
> * Use some kind of BCLYT-like layout framework

## 10/28/2020 - v0.0.4 - Downloader and injector update

* Updated graphic assets
* The downloader has been completed, which will inject projects into SmileBASIC.
* Reinstated curl-based downloader code

## 9/15/2020 - v0.0.3 - Download menu beginning update

* Fix for issue #0: App will crash if DSP failed to dump and a SFX tries to play
* Fix for issue #1: App will crash or freeze when DSP firm is attempted to be dumped (if wasn't not found before)
* Added download menu (WIP)

## 9/13/2020 - v0.0.2 - Main menu update

* Added main menu
* Added update system (which downloads the 3DSX and CIA from the Releases page)

## 9/12/2020 - v0.0.1 - Initial start of this project

* Using most code from [TwiLight Menu++ Updater](https://github.com/RocketRobz/TWiLightMenu-Updater).
* Modified keyboard.cpp to add the error applet caller and improved arguments taken by `keyboardInput()`.
* Added errtbl.cpp - Error code table and translation for the text found in the app
* Removed any curl-related code from the sources (I cannot build curl-using projects)
* Removed splash screens from the code.
* Removed GUI and replaced it with a HOME menu-related string.
* Replacing `downloadToFile()` with an httpc-based alternative.

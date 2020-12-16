#!/bin/sh
mkdir build
bannertool makebanner \
-i app/banner.png \
-a app/audio.wav \
-o build/banner.bnr

bannertool makesmdh \
-s "SmileBASIC Download Station" \
-l "SmileBASIC Download Station — DEV BUILD" \
-p "0.0.4-dev; 20201215; 46938900" \
-i app/icon.png -o build/icon.icn -r regionfree -f visible,nosavebackups,allow3d

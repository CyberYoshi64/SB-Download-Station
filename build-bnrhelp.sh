#!/bin/sh

clear

echo ""
echo " ============================================================================== "
echo "   SmileBASIC Download Station v0.1.0 ß1 - release candidant - Building script  "
echo " ============================================================================== "
echo ""
echo " Please use this script to build this application."
echo " Do NOT use “make” outside of the script as it will fail!"
echo " See BUILDING.md for details."
echo ""
echo "  This process may take up to 30 minutes, depending on your computer's power."
echo ""

echo " --- Cleaning any compiled data for this project..."
rm -rf release-data > /dev/null
rm -rf romfs/gfx > /dev/null
rm -rf assets/gfx/t3x.png > /dev/null
make clean > /dev/null

echo ""
echo " --- Preparing build folder and ExeFS data..."

mkdir build
bannertool makebanner \
-i app/banner.png \
-a app/audio.wav \
-o build/banner.bnr \
\
-eei app/banner/eu-en.png \
-efi app/banner/eu-fr.png \
-egi app/banner/eu-de.png \
-eii app/banner/eu-it.png \
-esi app/banner/eu-es.png \
-edi app/banner/eu-nl.png \
-epi app/banner/eu-pt.png \
-eri app/banner/eu-ru.png \
-jji app/banner/ja-jp.png \
-uei app/banner/us-en.png \
-ufi app/banner/us-fr.png \
-usi app/banner/us-es.png \
-upi app/banner/us-pt.png

bannertool makesmdh \
-s "SmileBASIC Download Station" \
-l "SmileBASIC Download Station" \
-p "the_squat1115, CyberYoshi64" \
-i app/icon.png \
-o build/icon.icn.tmp \
-r regionfree -f visible,nosavebackups,allow3d

tools/bannertool-smdhfix build/icon.icn.tmp app/smdh.wtxt build/icon.icn

echo ""
echo " --- Start the tedious compiling process..."
if make elf 3dsx cia 3ds; then
	echo ""
	echo " --- Re-arranging output folder..."
	cd output
	mkdir fallback
	mkdir 3ds
	mkdir 3ds/SB3_Download_Station
	cp SB3_Download_Station.3ds 3ds/SB3_Download_Station/SB3_Download_Station.3ds
	cp SB3_Download_Station.3dsx 3ds/SB3_Download_Station/SB3_Download_Station.3dsx
	cp ../build/icon.icn 3ds/SB3_Download_Station/SB3_Download_Station.smdh
	mkdir cia
	cp SB3_Download_Station.cia cia/SB3_Download_Station.cia
	mv SB3_Download_Station.elf fallback/SB3_Download_Station.elf
	mv SB3_Download_Station.3ds fallback/SB3_Download_Station.3ds
	mv SB3_Download_Station.cia fallback/SB3_Download_Station.cia
	mv SB3_Download_Station.3dsx fallback/SB3_Download_Station.3dsx
	mv fallback ../release-data
	cd ..
	echo ""
	echo "Wahoo! Your build was successful!"
	echo ""
	echo "Copy the contents of the output folder to the root of the SD card."
	echo "See the INSTALL.md for installing details."
	exit 0
else
	echo ""
	echo "A compilation error has occured."
	echo "This shouldn't have happened..."
	echo ""
	echo "  Try the following:"
	echo ""
	echo "  - Refresh dkp-libs by typing “sudo dkp-pacman -Syu” (or copy the quoted string)"
	echo "  - Reinstall the 3DS tools (see BUILDING.md for details)"
	echo "  - Reinstall devkitPro entirely"
	echo ""
	echo "  If these suggestions didn't help, then I may be a bonehead and made one"
	echo "  or more mistakes in the source code."
	echo ""
	echo "  Blame school for killing my free time. :.("
	echo "  THE TEACHERS ARE THE BONEHEADS THAT MADE ME A BONEHEAD!"
	echo ""
	echo " Whatever the issue is, the process has failed."
	exit 1
fi

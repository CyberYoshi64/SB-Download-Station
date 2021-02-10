#!/bin/sh

# 
# BCFNT automated generator + tester
# 
# May only work on CyberYoshi64's PC.
# 

cd ~/Schriftarten
mkbcfnt -s 18 \
-o /data/home/CyberYoshi64/github/SB-Download-Station/romfs/font/0.bcfnt \
nintendo_ext_003.ttf nintendoNTLG-DB.ttf SMILEBASIC_PUAonly.ttf \
TREBUC.TTF
cd /data/home/CyberYoshi64/github/SB-Download-Station
if [-d build.sh]; then
	./build-dev.sh
else
	./build.sh
fi
3dslink -a $IP_3DS_MAIN output/3ds/SB3_Download_Station/SB3_Download_Station.3dsx
3dslink -a $IP_3DS_SECONDARY output/3ds/SB3_Download_Station/SB3_Download_Station.3dsx

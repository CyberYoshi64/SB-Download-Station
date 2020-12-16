#!/bin/sh
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
-l "SmileBASIC Download Station: Beta version (0.0.4)" \
-p "the_squat1115, CyberYoshi64" \
-i app/icon.png \
-o build/icon.icn \
\
-js "プチコン３号ダウンロードステーション" \
-jl "プチコン３号ダウンロードステーション: BETA" \
-jp "the_squat1115, CyberYoshi64" \
\
-es "SmileBASIC Download Station" \
-el "SmileBASIC Download Station: Beta ver." \
-ep "the_squat1115, CyberYoshi64" \
\
-fs "Station de téléchargement SmileBASIC" \
-fl "Station de téléchargement SmileBASIC: BETA" \
-fp "the_squat1115, CyberYoshi64" \
\
-gs "SmileBASIC-Download-Station" \
-gl "SmileBASIC-Download-Station: Beta-Ver." \
-gp "the_squat1115, CyberYoshi64" \
\
-is "SmileBASIC Download Station" \
-il "SmileBASIC Download Station: BETA" \
-ip "the_squat1115, CyberYoshi64" \
\
-ss "Estación de descarga SmileBASIC" \
-sl "Estación de descarga SmileBASIC: BETA" \
-sp "the_squat1115, CyberYoshi64" \
\
-scs "SB3下載站" \
-scl "SB3下載站: BETA" \
-scp "the_squat1115, CyberYoshi64" \
\
-ks "SB3 다운로드 스테이션" \
-kl "SB3 다운로드 스테이션: BETA" \
-kp "the_squat1115, CyberYoshi64" \
\
-ds "SmileBASIC Download Station" \
-dl "SmileBASIC Download Station: BETA" \
-dp "the_squat1115, CyberYoshi64" \
\
-ps "Estação de download SmileBASIC" \
-pl "Estação de download SmileBASIC: BETA" \
-pp "the_squat1115, CyberYoshi64" \
\
-rs "Станция загрузки SmileBASIC" \
-rl "Станция загрузки SmileBASIC: BETA" \
-rp "the_squat1115, CyberYoshi64" \
\
-tcs "SB3下载站" \
-tcl "SB3下载站: BETA" \
-tcp "the_squat1115, CyberYoshi64" \
\
-r regionfree -f visible,nosavebackups,allow3d

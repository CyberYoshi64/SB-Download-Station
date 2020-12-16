#!/bin/sh
clear
echo ""
echo " ============================================================================== "
echo "    SmileBASIC Download Station v0.0.4-dev; State 20201215 - Building script    "
echo " ============================================================================== "
echo ""
echo " Please use this script to build this application."
echo " Do NOT use “make” outside of the script as it will fail upon linking the 3DSX."
echo ""
echo "\$> make clean"
make clean
echo ""
echo "\$> ./mkbnr-dev.sh"
./mkbnr-dev.sh
echo ""
echo "\$> make elf 3dsx cia 3ds"
make elf 3dsx cia 3ds
echo ""
echo "Success! Enjoy your dev build!"

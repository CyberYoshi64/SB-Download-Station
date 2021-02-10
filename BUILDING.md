# How to build the release/dev build of SmileBASIC Download Station

---

## What's the difference between release and dev?

To be honest, the only difference is the banner (for the CIA) and the SMDH being more simplistic for the "dev" version.

## Prerequisites

- devkitPro (dkp-pacman, MSYS2 for Windows)
- bannertool v1.1 or v1.2
- makerom v0.17 or higher

Disclaimer: The commands used and the binary building are only tested on GNU/Linux (Debian 10) and may not work on other Linux distros, and definitely not Windows or MacOS.

### Setting up devkitPro

Use the following command:

- Windows: `dkp-pacman -S 3ds-dev 3ds-portlibs`
- Linux: `sudo dkp-pacman -Syu; sudo dkp-pacman -S 3ds-dev 3ds-portlibs`

Agree to the changes made to devkitPro and let it install.

## Start building

Navigate to the directory containing the folder structure for the source code.
It should have folders, such as `source`, `romfs`, `app` and `include`.

Run one of the following scripts, for different configurations:

- `build.sh` - Linux only, build release candidate
- `build-dev.sh` - Linux only, build dev binaries
- `build-bnrhelp.sh` - Linux only, build release candidate with a helper tool (see Appendix A)

This should compile the source code and make an `output` folder.

See INSTALL.md for help installing th binaries on the 3DS.

---

## Appendix A: Fixing errors from bannertool v1.1

There may be a circumstance, where your PC doesn't want to use the latest build of bannertool.

If your PC, at least, supports v1.1, then you can still compile release binaries with help from tools/bannertool-smdhfix.

See [this issue from Steveice10/bannertool](https://github.com/Steveice10/bannertool/issues/18) for details about the conversion problem with v1.1.

You may need to recompile this helper tool, when the terminal throws errors about it. See Appendix B for details.

If you decide to ignore the conversion error and use `build.sh` with bannertool v1.1, your system may behave weirdly when displaying its name in FBI or the HOME menu.

That's because your system language is e.g. set to Spanish and some characters appear corrupted.

## Appendix B: Recompiling bannertool-smdhfix

Open your terminal and type `cd tools`

Next, you use `gcc` to recompile the C++ code into the binary.

```bash
gcc -o bannertool-smdhfix -pie -x c++ bannertool-smdhfix.cxx
chmod +x bannertool-smdhfix
```

Then, retry the buildng procedure.

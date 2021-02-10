# Installation

### Requirement

- Your system has to run Luma 3DS CFW 10.2.1+. If it's not hacked yet, [do that right now!](https://3ds.hacks.guide)
- Your system must be runnning on the latest firmware. As of writing this markdown, it's 11.14.
- You have to own SmileBASIC 3.6, which must originate from the Nintendo eShop.
- SmileBASIC must have created its project storage room, by running SB3 once.

After building the app, or downloading it, follow these steps:

### Step 1: Copying - after building from the source code

Simply copy the contents of the `output` folder to the root of the SD card.

See [this picture from the NH-Discord](https://i.imgur.com/QXHIvOz.jpg).

### Step 1: Copying - after downloading the release binaries

Place the binaries into a directory in the SD card, depending on which binary you want to use:

- CIA: `/cia` or `/cias`
- 3DSX w/o SMDH: `/3ds`
- 3DSX + SMDH: `/3ds/SB3_Download_Station`

Note: The elf binary has no purpose on the 3DS and should be left alone.

### Step 2: Installing (only CIA)

1. Turn on your Nintendo 3DS system with the SD card inserted.
2. Launch FBI and go to `SD` → `cias` → `SB3_Download_Station.cia` → `Install CIA` → `Yes`.

### Step 3: Running (for 3DSX)

1. Launch the Homebrew Launcher.
2. Find `SmileBASIC Download Station` in the list and press A on it.

### Step 3: Running (for CIA)

1. Find SmileBASIC Download Station's icon and press A on it.

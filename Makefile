#   
#   SmileBASIC Download Station - Makefile
#   
#   It is not useful whatsoever, if used without build.sh.
#   
#   2019-2020 CyberYoshi64
#   
#   Original idea by the_squat1115 on Old SmileBASIC Source
#   

ifeq ($(strip $(DEVKITARM)),)
$(error "DevKitARM is missing in your environment variables. Do export DEVKITARM=<path to>devkitARM")
endif

TOPDIR ?= $(CURDIR)
include $(DEVKITARM)/3ds_rules

TARGET      := SB3_Download_Station
BUILD       := build
SOURCES     := source
DATA        := data
INCLUDES    := include
GRAPHICS    := assets/gfx
OUTPUT      := output
RESOURCES   := app
ROMFS       := romfs
GFXBUILD    := $(ROMFS)/gfx
NO_SMDH     := true

BANNER_AUDIO    := $(RESOURCES)/audio
BANNER_IMAGE    := $(RESOURCES)/banner
ICON            := $(RESOURCES)/icon.png
RSF             := $(TOPDIR)/$(RESOURCES)/ciaprops.rsf

ARCH        := -march=armv6k -mtune=mpcore -mfloat-abi=hard -mtp=soft
COMMON      := -Wall -O2 -mword-relocations -fomit-frame-pointer -ffunction-sections $(ARCH) $(INCLUDE) -DARM11 -D_3DS
CFLAGS      := $(COMMON) -std=gnu99 `curl-config --cflags`
CXXFLAGS    := $(COMMON) -fno-rtti -fno-exceptions -std=gnu++11
ASFLAGS     := $(ARCH)
LDFLAGS     = -specs=3dsx.specs $(ARCH) -Wl,-Map,$(notdir $*.map)

LIBS := -O2 -lcurl -lm -lctru -lcitro2d -lcitro3d `curl-config --libs`

LIBDIRS	:= $(PORTLIBS) $(CTRULIB)

ifneq ($(BUILD),$(notdir $(CURDIR)))

export TOPDIR      := $(CURDIR)
export OUTPUT_DIR  := $(TOPDIR)/$(OUTPUT)
export OUTPUT_FILE := $(OUTPUT_DIR)/$(TARGET)
export VPATH       := $(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
                      $(foreach dir,$(GRAPHICS),$(CURDIR)/$(dir)) \
                      $(foreach dir,$(DATA),$(CURDIR)/$(dir))

export DEPSDIR     := $(CURDIR)/$(BUILD)

CFILES             := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES           := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES             := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
PICAFILES          := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.v.pica)))
SHLISTFILES        := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.shlist)))
GFXFILES           := $(foreach dir,$(GRAPHICS),$(notdir $(wildcard $(dir)/*.t3s)))
BINFILES           := $(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))

ifeq ($(strip $(CPPFILES)),)
	export LD := $(CC)
else
	export LD := $(CXX)
endif

export T3XFILES	      := $(GFXFILES:.t3s=.t3x)

export OFILES_SOURCES := $(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)
export OFILES_BIN     := $(addsuffix .o,$(BINFILES)) \
                         $(PICAFILES:.v.pica=.shbin.o) $(SHLISTFILES:.shlist=.shbin.o) \
                         $(if $(filter $(BUILD),$(GFXBUILD)),$(addsuffix .o,$(T3XFILES)))
export OFILES         := $(OFILES_BIN) $(OFILES_SOURCES)
export HFILES         := $(PICAFILES:.v.pica=_shbin.h) $(SHLISTFILES:.shlist=_shbin.h) \
                         $(addsuffix .h,$(subst .,_,$(BINFILES))) \
                         $(GFXFILES:.t3s=.h)
export INCLUDE        := $(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
                         $(foreach dir,$(LIBDIRS),-I$(dir)/include) \
                         -I$(CURDIR)/$(BUILD)

export LIBPATHS       := $(foreach dir,$(LIBDIRS),-L$(dir)/lib)

export _3DSXDEPS      := $(if $(NO_SMDH),,$(OUTPUT_FILE).smdh)

ifeq ($(strip $(ICON)),)
	icons := $(wildcard *.png)
	ifneq (,$(findstring $(TARGET).png,$(icons)))
		export APP_ICON := $(TOPDIR)/$(TARGET).png
	else
		ifneq (,$(findstring icon.png,$(icons)))
			export APP_ICON := $(TOPDIR)/icon.png
		endif
	endif
else
	export APP_ICON := $(TOPDIR)/$(ICON)
endif

export _3DSXFLAGS += --smdh=$(TOPDIR)/$(BUILD)/icon.icn

ifneq ($(ROMFS),)
	export _3DSXFLAGS += --romfs=$(CURDIR)/$(ROMFS)
endif

.PHONY : clean all bootstrap 3dsx cia elf 3ds

all : bootstrap
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

3dsx : bootstrap
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile $@

cia : bootstrap
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile $@

3ds : bootstrap
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile $@

elf : bootstrap
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile $@

bootstrap :
	@[ -d $(BUILD) ] || mkdir -p $(BUILD)
	@[ -d $(OUTPUT_DIR) ] || mkdir -p $(OUTPUT_DIR)
	@[ -d $(GFXBUILD) ] || mkdir -p $(GFXBUILD)

clean :
	@rm -rf $(BUILD) $(OUTPUT)

else

DEPENDS	:=	$(OFILES:.o=.d)

APP_TITLE         := SBDS-004
APP_PRODUCT_CODE  := CY64-SBDS
APP_UNIQUE_ID     := 0x16901
APP_VERSION_MAJOR := 0
APP_VERSION_MINOR := 0
APP_VERSION_MICRO := 4
APP_ROMFS         := $(TOPDIR)/$(ROMFS)

ifneq ("$(wildcard $(TOPDIR)/$(BANNER_IMAGE).cgfx)","")
	BANNER_IMAGE_FILE := $(TOPDIR)/$(BANNER_IMAGE).cgfx
	BANNER_IMAGE_ARG  := -ci $(BANNER_IMAGE_FILE)
else
	BANNER_IMAGE_FILE := $(TOPDIR)/$(BANNER_IMAGE).png
	BANNER_IMAGE_ARG  := -i $(BANNER_IMAGE_FILE)
endif

ifneq ("$(wildcard $(TOPDIR)/$(BANNER_AUDIO).cwav)","")
	BANNER_AUDIO_FILE := $(TOPDIR)/$(BANNER_AUDIO).cwav
	BANNER_AUDIO_ARG  := -ca $(BANNER_AUDIO_FILE)
else
	BANNER_AUDIO_FILE := $(TOPDIR)/$(BANNER_AUDIO).wav
	BANNER_AUDIO_ARG  := -a $(BANNER_AUDIO_FILE)
endif

COMMON_MAKEROM_PARAMS := -rsf $(RSF) -target t -exefslogo -elf $(OUTPUT_FILE).elf -icon icon.icn \
-banner banner.bnr -DAPP_TITLE="$(APP_TITLE)" -DAPP_PRODUCT_CODE="$(APP_PRODUCT_CODE)" \
-DAPP_UNIQUE_ID="$(APP_UNIQUE_ID)" -DAPP_ROMFS="$(APP_ROMFS)" -DAPP_SYSTEM_MODE="64MB" \
-DAPP_SYSTEM_MODE_EXT="Legacy" -major "$(APP_VERSION_MAJOR)" -minor "$(APP_VERSION_MINOR)" \
-micro "$(APP_VERSION_MICRO)"

ifeq ($(OS),Windows_NT)
	MAKEROM = makerom.exe
	BANNERTOOL = bannertool.exe
	CITRA = citra.exe
	_3DSXTOOL = 3dsxtool.exe
	SMDHTOOL = smdhtool.exe
	TEX3DS = tex3ds.exe
else
	MAKEROM = makerom
	BANNERTOOL = bannertool
	CITRA = citra
	_3DSXTOOL = 3dsxtool
	SMDHTOOL = smdhtool
	TEX3DS = tex3ds
endif

.PHONY: all 3dsx cia elf 3ds citra

$(OUTPUT_FILE).3dsx : $(OUTPUT_FILE).elf $(_3DSXDEPS)
	$(_3DSXTOOL) $< $@ $(_3DSXFLAGS)
	@echo "built ... $(notdir $@)"

$(OFILES_SOURCES) : $(HFILES)

$(OUTPUT_FILE).elf : $(OFILES)

$(OUTPUT_FILE).3ds : $(OUTPUT_FILE).elf banner.bnr icon.icn
	@$(MAKEROM) -f cci -o $(OUTPUT_FILE).3ds -DAPP_ENCRYPTED=true $(COMMON_MAKEROM_PARAMS)
	@echo "built ... $(notdir $@)"

$(OUTPUT_FILE).cia : $(OUTPUT_FILE).elf banner.bnr icon.icn
	@$(MAKEROM) -f cia -o $(OUTPUT_FILE).cia -DAPP_ENCRYPTED=false $(COMMON_MAKEROM_PARAMS)
	@echo "built ... $(notdir $@)"

3dsx : $(OUTPUT_FILE).3dsx

cia : $(OUTPUT_FILE).cia

3ds : $(OUTPUT_FILE).3ds

elf : $(OUTPUT_FILE).elf

citra : 3dsx
	$(CITRA) $(OUTPUT_FILE).3dsx

%.bin.o	%_bin.h : %.bin
	@echo $(notdir $<)
	@$(bin2o)

.PRECIOUS : %.t3x

%.t3x.o	%_t3x.h : %.t3x
	@$(bin2o)

define shader-as
	$(eval CURBIN := $*.shbin)
	$(eval DEPSFILE := $(DEPSDIR)/$*.shbin.d)
	echo "$(CURBIN).o: $< $1" > $(DEPSFILE)
	echo "extern const u8" `(echo $(CURBIN) | sed -e 's/^\([0-9]\)/_\1/' | tr . _)`"_end[];" > `(echo $(CURBIN) | tr . _)`.h
	echo "extern const u8" `(echo $(CURBIN) | sed -e 's/^\([0-9]\)/_\1/' | tr . _)`"[];" >> `(echo $(CURBIN) | tr . _)`.h
	echo "extern const u32" `(echo $(CURBIN) | sed -e 's/^\([0-9]\)/_\1/' | tr . _)`_size";" >> `(echo $(CURBIN) | tr . _)`.h
	picasso -o $(CURBIN) $1
	bin2s $(CURBIN) | $(AS) -o $*.shbin.o
endef

%.shbin.o %_shbin.h : %.v.pica %.g.pica
	@echo $(notdir $^)
	@$(call shader-as,$^)

%.shbin.o %_shbin.h : %.v.pica
	@echo $(notdir $<)
	@$(call shader-as,$<)

%.shbin.o %_shbin.h : %.shlist
	@echo $(notdir $<)
	@$(call shader-as,$(foreach file,$(shell cat $<),$(dir $<)$(file)))

%.t3x %.h :  %.t3s
	@echo $(notdir $<)
	@$(TEX3DS) -i $< -H $*.h -d $*.d -o $(TOPDIR)/$(GFXBUILD)/$*.t3x

-include $(DEPSDIR)/*.d
endif

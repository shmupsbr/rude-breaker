# Rude Breaker — PC port (SDL2)
# Usage:
#   make              # SDL2 host build
#   make test-assets  # parse extracted SGM1 if present
#   make linux        # Linux build: binary loads libSDL2-2.0.so.0 from its own folder first

PLATFORM ?= sdl2
BUILD    := build
CC       ?= gcc
CXX      ?= g++
CFLAGS   := -std=c99 -Wall -Wextra -Wpedantic -O2 -Iinclude
# Empty unless `make debug`. That target defines KRAKEN_DEBUG.
EXTRA_CFLAGS ?=
CFLAGS   += $(EXTRA_CFLAGS)
CXXFLAGS := -std=c++14 -O2 -w -isystem third_party/ymfm -Iinclude -Isrc/hal/sdl2
LDFLAGS  :=

CORE_SRCS := \
	src/core/panels.c \
	src/core/main.c \
	src/core/exepath.c \
	src/core/game.c \
	src/core/player.c \
	src/core/enemy.c \
	src/core/collision.c \
	src/core/stage.c \
	src/core/stage_data.c \
	src/core/assets.c \
	src/core/diskimg.c \
	src/core/inflate.c \
	src/core/vm.c \
	src/core/gfx.c \
	src/core/pace.c \
	src/core/cfilter.c \
	src/core/crt.c \
	src/core/eq.c \
	src/core/demo.c \
	src/core/kraken.c \
	src/core/kraken_hit.c

ifeq ($(PLATFORM),sdl2)
  HAL_SRCS := \
	src/hal/sdl2/video_sdl2.c \
	src/hal/sdl2/audio_sdl2.c \
	src/hal/sdl2/fplay.c \
	src/hal/sdl2/fplay_sfx.c \
	src/hal/sdl2/chipsynth.c \
	src/hal/sdl2/input_sdl2.c
  YMFM_SRCS := \
	third_party/ymfm/ymfm_opn.cpp \
	third_party/ymfm/ymfm_ssg.cpp \
	third_party/ymfm/ymfm_adpcm.cpp \
	src/hal/sdl2/opna_ymfm.cpp \
	src/hal/sdl2/opna_sfx.cpp \
	src/hal/sdl2/md_fm.cpp
  SDL2_CFLAGS ?= $(shell pkg-config --cflags sdl2)
  SDL2_LIBS   ?= $(shell pkg-config --libs sdl2)
  CFLAGS  += $(SDL2_CFLAGS) -DSDL_MAIN_HANDLED
  LDFLAGS += $(SDL2_LIBS)
  # Linux: look for a bundled libSDL2-2.0.so.0 next to the binary first,
  # and link the C++ runtime statically so only libc/libm are needed.
  ifeq ($(LINUX_BUNDLE),1)
    LDFLAGS += -Wl,-rpath,'$$ORIGIN' -static-libstdc++ -static-libgcc
  endif
  # timeBeginPeriod. The .exe cross link is the Windows build.
  ifneq ($(EXE),)
    LDFLAGS += -lwinmm
  endif
  # Cross builds pass EXE=.exe so the recipe's -o name matches MinGW.
  EXE     ?=
  TARGET  := $(BUILD)/rudebreaker$(EXE)
  LINK    := $(CXX)
else
  $(error Unknown PLATFORM=$(PLATFORM); use sdl2)
endif

SRCS := $(CORE_SRCS) $(HAL_SRCS)
OBJS := $(SRCS:%.c=$(BUILD)/$(PLATFORM)/%.o) \
        $(YMFM_SRCS:%.cpp=$(BUILD)/$(PLATFORM)/%.o)

.PHONY: all linux clean test-assets test-endian vm-check cfg-check zx-test hit-check dirs debug

# `debug` must not be the first target: plain `make` is the release build.
.DEFAULT_GOAL := all

all: dirs $(TARGET) test-endian hit-check

# Debug build: same game, plus F9 for the Kraken appearance sequence.
# Leave this target and the KRAKEN_DEBUG blocks out of a release build.
debug:
	$(MAKE) EXTRA_CFLAGS=-DKRAKEN_DEBUG=1 all

linux:
	$(MAKE) LINUX_BUNDLE=1 all

dirs:
	@mkdir -p $(BUILD)/$(PLATFORM)/src/core \
	          $(BUILD)/$(PLATFORM)/src/hal/sdl2

$(TARGET): $(OBJS)
	$(LINK) -o $@ $^ $(LDFLAGS)

# Rebuild objects when the flags change, so `make debug` cannot leave
# -DKRAKEN_DEBUG in the binary that a later plain `make` links.
$(BUILD)/$(PLATFORM)/cflags.tag: FORCE
	@mkdir -p $(dir $@)
	@printf '%s\n' '$(CFLAGS)' | cmp -s - $@ || printf '%s\n' '$(CFLAGS)' > $@

FORCE:
.PHONY: FORCE

$(BUILD)/$(PLATFORM)/%.o: %.c $(BUILD)/$(PLATFORM)/cflags.tag
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)/$(PLATFORM)/%.o: %.cpp $(BUILD)/$(PLATFORM)/cflags.tag
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

test-assets:
	@python3 tools/parse_sgm1.py assets/raw/RUDEBR/SGM1.DAT assets/sgm1 || \
	  echo "Place extracted SGM1.DAT under assets/raw/RUDEBR first."

vm-check: dirs
	$(CC) $(CFLAGS) -o $(BUILD)/vm_check tools/vm_check.c tools/vm_stub.c \
		src/core/vm.c src/core/gfx.c src/core/assets.c src/core/collision.c

# Config load/save/defaults round trip; no game data needed.
CFG_CHECK_OBJS := $(filter-out %/main.o %/game.o,$(OBJS))
cfg-check: dirs $(CFG_CHECK_OBJS)
	$(CC) $(CFLAGS) -Wno-unused-function -c -o $(BUILD)/cfg_check.o tools/cfg_check.c
	$(LINK) -o $(BUILD)/cfg_check$(EXE) $(BUILD)/cfg_check.o $(CFG_CHECK_OBJS) $(LDFLAGS)
	@if [ -z "$(EXE)" ]; then $(BUILD)/cfg_check; else echo "cross compile: skipped running cfg_check$(EXE)"; fi

# ZX filter rules on synthetic frames (no game data).
# Kraken hit rules. No game data.
hit-check:
	@mkdir -p $(BUILD)
	$(CC) $(CFLAGS) -o $(BUILD)/kraken_hit_test tools/kraken_hit_test.c src/core/kraken_hit.c src/core/pace.c
	$(BUILD)/kraken_hit_test

pad-check:
	@mkdir -p $(BUILD)
	$(CC) $(CFLAGS) -o $(BUILD)/pad_check tools/pad_check.c src/hal/sdl2/input_sdl2.c $(SDL2_LIBS)
	SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy $(BUILD)/pad_check

zx-test: dirs
	$(CC) $(CFLAGS) -c -o $(BUILD)/cfilter_zx.o src/core/cfilter.c
	$(CC) $(CFLAGS) -o $(BUILD)/zx_rules_test tools/zx_rules_test.c $(BUILD)/cfilter_zx.o
	$(BUILD)/zx_rules_test

test-endian: dirs
	$(CC) $(CFLAGS) -o $(BUILD)/test_endian$(EXE) tools/test_endian.c
	@if [ -z "$(EXE)" ]; then $(BUILD)/test_endian; else echo "cross compile: skipped running test_endian$(EXE)"; fi

clean:
	rm -rf $(BUILD)



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
	src/core/pace.c

ifeq ($(PLATFORM),sdl2)
  HAL_SRCS := \
	src/hal/sdl2/video_sdl2.c \
	src/hal/sdl2/audio_sdl2.c \
	src/hal/sdl2/fplay.c \
	src/hal/sdl2/input_sdl2.c
  YMFM_SRCS := \
	third_party/ymfm/ymfm_opn.cpp \
	third_party/ymfm/ymfm_ssg.cpp \
	third_party/ymfm/ymfm_adpcm.cpp \
	src/hal/sdl2/opna_ymfm.cpp
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

.PHONY: all linux clean test-assets test-endian vm-check dirs

all: dirs $(TARGET) test-endian

linux:
	$(MAKE) LINUX_BUNDLE=1 all

dirs:
	@mkdir -p $(BUILD)/$(PLATFORM)/src/core \
	          $(BUILD)/$(PLATFORM)/src/hal/sdl2

$(TARGET): $(OBJS)
	$(LINK) -o $@ $^ $(LDFLAGS)

$(BUILD)/$(PLATFORM)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)/$(PLATFORM)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

test-assets:
	@python3 tools/parse_sgm1.py assets/raw/RUDEBR/SGM1.DAT assets/sgm1 || \
	  echo "Place extracted SGM1.DAT under assets/raw/RUDEBR first."

vm-check: dirs
	$(CC) $(CFLAGS) -o $(BUILD)/vm_check tools/vm_check.c tools/vm_stub.c \
		src/core/vm.c src/core/gfx.c src/core/assets.c src/core/collision.c

test-endian: dirs
	$(CC) $(CFLAGS) -o $(BUILD)/test_endian$(EXE) tools/test_endian.c
	@if [ -z "$(EXE)" ]; then $(BUILD)/test_endian; else echo "cross compile: skipped running test_endian$(EXE)"; fi

clean:
	rm -rf $(BUILD)



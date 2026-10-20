# Rude Breaker — Phase 1 (SDL2) / Phase 2 (X68000)
# Usage:
#   make              # SDL2 host build
#   make PLATFORM=x68k
#   make test-assets  # parse extracted SGM1 if present

PLATFORM ?= sdl2
BUILD    := build
CC       ?= gcc
CFLAGS   := -std=c99 -Wall -Wextra -Wpedantic -O2 -Iinclude
LDFLAGS  :=

CORE_SRCS := \
	src/core/main.c \
	src/core/game.c \
	src/core/player.c \
	src/core/enemy.c \
	src/core/collision.c \
	src/core/stage.c \
	src/core/stage_data.c \
	src/core/assets.c \
	src/core/vm.c

ifeq ($(PLATFORM),sdl2)
  HAL_SRCS := \
	src/hal/sdl2/video_sdl2.c \
	src/hal/sdl2/audio_sdl2.c \
	src/hal/sdl2/input_sdl2.c
  CFLAGS  += $(shell pkg-config --cflags sdl2)
  LDFLAGS += $(shell pkg-config --libs sdl2)
  TARGET  := $(BUILD)/rudebreaker
else ifeq ($(PLATFORM),x68k)
  HAL_SRCS := \
	src/hal/x68k/video_x68k.c \
	src/hal/x68k/audio_x68k.c \
	src/hal/x68k/input_x68k.c
  CFLAGS  += -DRB_PLATFORM_X68K
  # Human68k cross compiler — override on the command line if needed.
  CC      ?= human68k-gcc
  TARGET  := $(BUILD)/rudebreaker.x
else
  $(error Unknown PLATFORM=$(PLATFORM); use sdl2 or x68k)
endif

SRCS := $(CORE_SRCS) $(HAL_SRCS)
OBJS := $(SRCS:%.c=$(BUILD)/$(PLATFORM)/%.o)

.PHONY: all clean test-assets test-endian vm-check dirs

all: dirs $(TARGET) test-endian

dirs:
	@mkdir -p $(BUILD)/$(PLATFORM)/src/core \
	          $(BUILD)/$(PLATFORM)/src/hal/sdl2 \
	          $(BUILD)/$(PLATFORM)/src/hal/x68k

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

$(BUILD)/$(PLATFORM)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

test-assets:
	@python3 tools/parse_sgm1.py assets/raw/RUDEBR/SGM1.DAT assets/sgm1 || \
	  echo "Place extracted SGM1.DAT under assets/raw/RUDEBR first."

vm-check: dirs
	$(CC) $(CFLAGS) -o $(BUILD)/vm_check tools/vm_check.c tools/vm_stub.c \
		src/core/vm.c src/core/assets.c src/core/collision.c

test-endian: dirs
	$(CC) $(CFLAGS) -o $(BUILD)/test_endian tools/test_endian.c
	$(BUILD)/test_endian

clean:
	rm -rf $(BUILD)

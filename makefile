MAKEFLAGS += --silent

# Compiler and flags
CC = gcc
WINDOWS_CC = x86_64-w64-mingw32-gcc

# Common flags
CFLAGS = -I./include -I./res -Wall -std=c11 -fms-extensions -g -O1 -flarge-source-files -DSURVIVAL_MODE
LDFLAGS = -lpthread -lSDL2 -lSDL2main -lGL -lGLU -lm -lopenal -lvorbisfile -lz

# Linux-specific flags
#LINUX_CFLAGS = $(CFLAGS)
#LINUX_LDFLAGS = -static-libgcc -static-libstdc++ \
	-Wl,-Bstatic \
	-lSDL2 -lGLEW -lGLU -lvorbisfile -lvorbis -logg -lz \
	-Wl,-Bdynamic \
	-lGL -lopenal \
	-lX11 -lXext -lXrandr -lXi -lXcursor -lXinerama -lXxf86vm -lXfixes -lXss \
	-lwayland-client -lwayland-cursor -lwayland-egl -lxkbcommon -ldecor-0 \
	-lpulse -lpulse-simple \
	-ldrm -lgbm \
	-lpthread -lm -ldl -lrt -lasound
LINUX_CFLAGS = $(CFLAGS) -fsanitize=address
LINUX_LDFLAGS = $(LDFLAGS) -fsanitize=address -lGLEW

# Windows-specific flags
WINDOWS_CFLAGS = -I./include -I./res -I./lib/zlib -Wall -std=c11 -fms-extensions -Dmain=main \
	-isystem ./lib -DSURVIVAL_MODE -DGLEW_STATIC -g2 -gdwarf-2 -O3
WINDOWS_LDFLAGS = \
    -L./lib \
    -static -static-libgcc -static-libstdc++ \
    -lmingw32 -lSDL2main -lSDL2 \
    -lopengl32 -lglu32 -lz \
	-lOpenAL32 -lwinmm \
    -lvorbisfile -lvorbis -logg -lkernel32 \
    -lsetupapi -limm32 -loleaut32 -lversion -luuid \
    -lgdi32 -lcomdlg32 -luser32 -lole32 -ladvapi32 \
    -lm ./res/assets/app/icon.res

# Directories
OBJDIR = bin
SRCDIR = src
LIBSRCDIR = lib/GL
ASSETSDIR = res/assets
HEADERSDIR = $(ASSETSDIR)

# Source and object files
rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))
SRC = $(call rwildcard,$(SRCDIR),*.c)
LINUX_OBJS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRC))
WINDOWS_OBJS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.win.o, $(SRC)) $(OBJDIR)/glew.win.o

# Linux build
minecraft: $(LINUX_OBJS)
	@echo !==== LINKING FOR LINUX ====!
	$(CC) $(LINUX_OBJS) $(LINUX_LDFLAGS) -o minecraft

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@echo !==== COMPILING FOR LINUX $< ====!
	@mkdir -p $(@D)
	$(CC) $(LINUX_CFLAGS) -c $< -o $@

# Windows build
windows: $(WINDOWS_OBJS)
	@echo !==== LINKING FOR WINDOWS ====!
	$(WINDOWS_CC) $(WINDOWS_OBJS) $(WINDOWS_LDFLAGS) -o minecraft.exe

$(OBJDIR)/%.win.o: $(SRCDIR)/%.c
	@echo !==== COMPILING FOR WINDOWS $< ====!
	@mkdir -p $(@D)
	$(WINDOWS_CC) $(WINDOWS_CFLAGS) -c $< -o $@

$(OBJDIR)/glew.win.o: $(LIBSRCDIR)/glew.c
	@echo !==== COMPILING GLEW FOR WINDOWS $< ====!
	@mkdir -p $(@D)
	$(WINDOWS_CC) $(WINDOWS_CFLAGS) -w -c $< -o $@

icon:
	@echo !==== COMPILING ICON ====!
	@mkdir -p $(OBJDIR)
	x86_64-w64-mingw32-windres -i ./res/assets/app/icon.rc -O coff -o ./res/assets/app/icon.res

# Find all .png and .ogg files
ASSET_FILES := $(shell find $(ASSETSDIR) -type f \( -name "*.png" -o -name "*.ogg" \))

# Convert asset files into header files
ASSETS := $(patsubst $(ASSETSDIR)/%, $(HEADERSDIR)/%.h, $(basename $(ASSET_FILES)))

$(HEADERSDIR)/%.h: $(ASSETSDIR)/%.png
	@echo !==== CONVERTING $< TO C HEADER ====!
	@mkdir -p $(@D)
	@FILENAME=$(shell basename $< .png | tr -c 'a-zA-Z0-9' '_') && \
	PREFIXED_NAME="asset_$${FILENAME}" && \
	convert $< -depth 8 rgba:- | xxd -i -c 16 | \
	sed "1s/^/static const unsigned char $${PREFIXED_NAME}rgba[] = {\n/" | \
	sed "\$$s/\$$/};/" > $@ && \
	echo "static const unsigned int $${PREFIXED_NAME}width = $(shell identify -format "%w" $<);" >> $@ && \
	echo "static const unsigned int $${PREFIXED_NAME}height = $(shell identify -format "%h" $<);" >> $@

$(HEADERSDIR)/%.h: $(ASSETSDIR)/%.ogg
	@echo !==== CONVERTING $< TO C HEADER ====!
	@mkdir -p $(@D)
	@FILENAME=$(shell basename $< .ogg | tr -c 'a-zA-Z0-9' '_') && \
	PREFIXED_NAME="asset_$${FILENAME}" && \
	xxd -p -c 16 $< | sed 's/\(..\)/0x\1,/g' | \
	sed "1s/^/static const unsigned char $${PREFIXED_NAME}ogg[] = {\n/" | \
	sed "\$$s/,\$$/\n};/" > $@ && \
	echo "static const unsigned int $${PREFIXED_NAME}size = $(shell stat -c%s $<);" >> $@

assets: $(ASSETS)
	@echo !==== ALL ASSETS CONVERTED ====!

clean:
	@echo !==== CLEANING ====!
	rm -rf $(OBJDIR) minecraft minecraft.exe

cleanassets:
	@echo !==== CLEANING ASSETS ====!
	@find $(HEADERSDIR) -type f -name "*.h" -delete
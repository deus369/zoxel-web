# zoxel makefile: [linux, web, android, windows]
#	sudo apt install make && make install-required

# determine the operating system #
ifeq ($(OS),Windows_NT)
    SYSTEM := Windows
else
    SYSTEM := $(shell uname -s)
endif
CC = gcc
# compiler tools and flags #
NAME := zoxel_web
TARGET = zoxel_web
ifeq ($(SYSTEM),Windows)
TARGET = zoxel_web.exe
endif
ifeq ($(SYSTEM),Windows)
    SRCS := $(shell find src/ -type f \( -name "*.c" -o -name "*.h" \))
else
    SRCS := $(shell find src/ -type f \( -name "*.c" -o -name "*.h" \))
endif
# OBJS = ../src/main.c
CFLAGS =
LDLIBS =

$(TARGET): $(SRCS)
	bash bash/run.sh

run:
	./$(TARGET)

install:
	@echo "	make is installing zoxel-web"

uninstall:
	@echo "	make is uninstalling zoxel-web"

git-push:
	@echo "	make is pushing zoxel-web"
	bash bash/git_push.sh

git-pull:
	@echo "	make is pulling zoxel-web"
	git pull

codeberg:
	open https://codeberg.org/deus/zoxel-web &

github:
	open https://github.com/deus369/zoxel-web &

# lost ones #

help:
	@echo "zoxel web -> an open source website"
	@echo "	latest @ https://codeberg.org/deus/zoxel-web"
	@echo "  make <target>"
	@echo "    <empty>  			builds release"
	@echo "    run				runs $(TARGET)"
	@echo "  > setup"
	@echo "    install			installs zoxel-web"
	@echo "    uninstall			inuninstalls zoxel-web"
	@echo "  > util"
	@echo "    github			opens zoxel-web on github"
	@echo "    codeberg			opens zoxel-web on codeberg"
	@echo "  > git"
	@echo "    git-pull			pulls latest git"
	@echo "    git-push			pushes git updates (requires ssh access)"
	@echo "    git-keys			creates a ssh key to add to git servers"
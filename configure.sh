cat << EOF > ./make/config.mk
ABS_PATH = $PWD

CC = clang++

CPPFLAGS = -std=c++1y -pedantic -Wall -Wextra -Werror -g3
LDLIBS = -lGL -lepoxy -lglfw -lm -lpng

CPPFLAGS += -I ./include
CPPFLAGS += -I \$(ABS_PATH)/include

SRC_DIR = src
BUILD_DIR =_build

DIST_SRC_DIR = \$(ABS_PATH)/src
DIST_BLD_DIR = \$(ABS_PATH)/_build

DIST_SRC = init.cc
DIST_OBJ = \$(addprefix \$(DIST_BLD_DIR)/, \$(DIST_SRC:.cc=.o))
EOF

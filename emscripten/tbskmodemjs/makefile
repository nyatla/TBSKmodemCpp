NAME_APP=wasm_main.html

PATH_ROOT=$(shell pwd)
DIR_SOURCE=./src
DIR_INCLUDE=../../src

PATH_SOURCE=$(PATH_ROOT)/$(DIR_SOURCE)
PATH_OBJECT=$(PATH_ROOT)/obj
PATH_OUTPUT=$(PATH_ROOT)

TARGET_INCLUDE=

include ../makefile.env

#relative path
SOURCE_FILES=wasm_main.cpp
TARGET_FILES=$(patsubst %.cpp,%.o,$(SOURCE_FILES))



TO_OBJ_NAME = \
	$(addprefix $(PATH_OBJECT)/, \
		$(patsubst %.cpp,%.o,$(1)) \
	)
TO_SRC_NAME = \
	$(addprefix $(PATH_SOURCE)/, \
		$(patsubst %.o,%.cpp,$(1)) \
	)


TARGETS=$(call TO_OBJ_NAME,$(SOURCE_FILES))
SOURCES=$(call TO_SRC_NAME,$(SOURCE_FILES))


all:$(PATH_OUTPUT)/$(NAME_APP)

clean:
	rm -f $(PATH_OBJECT)/*.o
	rm -f $(PATH_OUTPUT)/$(NAME_APP)


$(PATH_OUTPUT)/$(NAME_APP):$(TARGET_FILES)
	$(CC) -O0 -o $(PATH_OUTPUT)/$(NAME_APP)  $(FLGS) $(TARGETS) ../libtbskmodem/libtbskmodem.a --js-library ./src/jsfunctions.js

$(TARGET_FILES):$(SOURCES);mkdir -p $(PATH_OBJECT)
	$(CC) $(FLGS)  -s NO_EXIT_RUNTIME=1 \
	-s "EXTRA_EXPORTED_RUNTIME_METHODS=['getValue', 'setValue']" \
	-c $(TARGET_INCLUDE)  $(call TO_SRC_NAME,$@) -o $(call TO_OBJ_NAME,$@)









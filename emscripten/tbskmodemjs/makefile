NAME_APP=tbskmodem_wasm.js
NAME_MOD=tbskmodem_wasm_mod.js

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



js:$(PATH_OUTPUT)/$(NAME_APP)
mod:$(PATH_OUTPUT)/$(NAME_MOD)
all:js mod


clean:
	rm -f $(PATH_OBJECT)/*.o
	rm -f $(PATH_OUTPUT)/$(NAME_APP) $(PATH_OUTPUT)/$(NAME_MOD)


$(PATH_OUTPUT)/$(NAME_APP):$(TARGET_FILES)
	$(CC) -O2 -o $(PATH_OUTPUT)/$(NAME_APP)  $(FLGS) $(TARGETS) ../libtbskmodem/libtbskmodem.a

# For TBSKmodemJS
$(PATH_OUTPUT)/$(NAME_MOD):$(TARGET_FILES)
	$(CC) \
	-s AUTO_NATIVE_LIBRARIES=0 \
	-s ENVIRONMENT=web \
	-s MODULARIZE=1 \
	-s EXPORT_ES6=1 \
	-O2 -o $(PATH_OUTPUT)/$(NAME_MOD)  $(FLGS) $(TARGETS) ../libtbskmodem/libtbskmodem.a


# For standard
$(TARGET_FILES):$(SOURCES);mkdir -p $(PATH_OBJECT)
	$(CC) $(FLGS)  \
	-c $(TARGET_INCLUDE)  $(call TO_SRC_NAME,$@) -o $(call TO_OBJ_NAME,$@)









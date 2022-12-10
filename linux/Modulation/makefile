NAME_APP=tbsk_modulate

PATH_ROOT=$(shell pwd)
DIR_SOURCE=../../src
DIR_INCLUDE=../../src

PATH_SOURCE=$(PATH_ROOT)
PATH_OBJECT=$(PATH_ROOT)/obj
PATH_OUTPUT=$(PATH_ROOT)

TARGET_INCLUDE=

include ../makefile.env

#relative path
SOURCES= \
	$(shell ls -d `find $(PATH_SOURCE) -name *.cpp -type f` )

TARGETS= \
	$(patsubst %.cpp,%.o,$(SOURCES))

	
TARGET_TO_OBJ_NAME = \
	$(addsuffix .o, \
		$(addprefix $(PATH_OBJECT)/, \
			$(subst /,_,\
				$(basename \
					$(subst $(PATH_SOURCE)/,,$(1)) \
				) \
			) \
		) \
	)
TARGET_TO_SRC_NAME = \
	$(patsubst %.o,%.cpp,$(1))





all:$(PATH_OUTPUT)/$(NAME_APP)

clean:
	rm -f $(PATH_OBJECT)/*.o
	rm -f $(PATH_OUTPUT)/$(NAME_APP)


$(PATH_OUTPUT)/$(NAME_APP):$(TARGETS)
	$(CC) -o $(PATH_OUTPUT)/$(NAME_APP)  $(call TARGET_TO_OBJ_NAME,$(TARGETS)) ../libtbskmodem/libtbskmodem.a

$(TARGETS):$(SOURCES);mkdir -p $(PATH_OBJECT)
	$(CC) $(FLGS) -c $(TARGET_INCLUDE)  $(call TARGET_TO_SRC_NAME,$@) -o $(call TARGET_TO_OBJ_NAME,$@)









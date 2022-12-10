NAME_LIBRARY=libtbskmodem.a

PATH_ROOT=$(shell pwd)
DIR_SOURCE=../../src
DIR_INCLUDE=../../src

PATH_SOURCE=$(abspath $(PATH_ROOT)/$(DIR_SOURCE))
PATH_OBJECT=$(PATH_ROOT)/obj
PATH_OUTPUT=$(PATH_ROOT)

#TARGET_INCLUDE=$(addprefix -I$(DIR_INCLUDE)/,tbskmodem)
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


#TO_SRC_NAME = $(1)
#TARGETSに対応するファイルセット




all:$(PATH_OUTPUT)/$(NAME_LIBRARY)

clean:
	rm -f $(PATH_OBJECT)/*.o
	rm -f $(PATH_OUTPUT)/$(NAME_LIBRARY)


$(PATH_OUTPUT)/$(NAME_LIBRARY):$(TARGETS)
	ar -r $(PATH_OUTPUT)/$(NAME_LIBRARY) $(call TARGET_TO_OBJ_NAME,$(TARGETS))
	ranlib $@

$(TARGETS):$(SOURCES);mkdir -p $(PATH_OBJECT)
	$(CC) $(FLGS) -c $(TARGET_INCLUDE)  $(call TARGET_TO_SRC_NAME,$@) -o $(call TARGET_TO_OBJ_NAME,$@)



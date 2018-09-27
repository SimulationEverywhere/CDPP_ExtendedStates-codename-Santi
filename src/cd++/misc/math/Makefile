CPP_SOURCES   = $(wildcard *.cpp)
C_SOURCES     = $(wildcard *.c)
DEPS          = $(CPP_SOURCES:.cpp=.dep) $(C_SOURCES:.c=.dep)
OBJS          = $(CPP_SOURCES:.cpp=.o) $(C_SOURCES:.c=.o)
OBJS_W_DIR    = $(addprefix ${OBJ_DIR}/, $(OBJS))

NODEPS = clean

include ../../Makefile.rules


all   : $(OBJS_W_DIR)

clean :
	@rm -rf $(DEPS) $(OBJ_DIR)
	
ifeq (0, $(words $(findstring $(MAKECMDGOALS), $(NODEPS))))
sinclude $(DEPS)
endif	
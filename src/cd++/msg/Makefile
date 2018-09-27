ALL_SOURCES      = $(wildcard *.cpp)
SOURCES          = $(filter-out msgadm.cpp, $(ALL_SOURCES))

DEPS          = $(SOURCES:.cpp=.dep)
OBJS          = $(SOURCES:.cpp=.o)
OBJS_W_DIR    = $(addprefix ${OBJ_DIR}/, $(OBJS))

NODEPS = clean

include ../Makefile.rules


all   : $(OBJS_W_DIR)

clean :
	@rm -rf $(DEPS) $(OBJ_DIR)
	
ifeq (0, $(words $(findstring $(MAKECMDGOALS), $(NODEPS))))
sinclude $(DEPS)
endif
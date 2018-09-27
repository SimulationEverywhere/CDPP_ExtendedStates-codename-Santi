CPP_SOURCES   = $(wildcard *.cpp)
DEPS          = $(CPP_SOURCES:.cpp=.dep)
OBJS          = $(CPP_SOURCES:.cpp=.o)
OBJS_W_DIR    = $(addprefix ${OBJ_DIR}/, $(OBJS))

NODEPS = clean

include ../../Makefile.rules


all   : grammar $(OBJS_W_DIR)

clean :
	@rm -f gram.c gram.h gram.output
	@rm -rf $(DEPS) $(OBJ_DIR)
	
grammar : gram.y
	@rm -f gram.c gram.h gram.output
	@bison -d -v -o gram.c gram.y
	@mkdir -p $(OBJ_DIR)
	${CPP} -c ${INCLUDES_CPP} ${DEFINES_CPP} ${DEBUGFLAGS} ${CPPFLAGS} ${OPTCPPFLAGS} gram.c -o $(OBJ_DIR)/gram.o
	@rm gram.c
	
parser.dep : parser.cpp
	@touch gram.h
	@set -e; rm -f $@; \
	$(CPP) -MM ${INCLUDES_CPP} -MT '$${OBJ_DIR}/$(basename $@).o' ${DEFINES_CPP} ${DEBUGFLAGS} ${CPPFLAGS} ${OPTCPPFLAGS} $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
	
ifeq (0, $(words $(findstring $(MAKECMDGOALS), $(NODEPS))))
sinclude $(DEPS)
endif
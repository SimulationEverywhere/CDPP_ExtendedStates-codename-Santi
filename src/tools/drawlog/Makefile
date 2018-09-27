KERNEL_SRC    = ../../cd++

TOOL_NAME     = drawlog

SOURCES       = $(wildcard *.cpp)
DEPS          = $(SOURCES:.cpp=.dep)
OBJS          = $(SOURCES:.cpp=.o)
OBJS_W_DIR    = $(addprefix ${OBJ_DIR}/, $(OBJS))

DEPS_OBJ      = logparser.o ntupla.o cellstate.o strutil.o ini.o VTime.o real.o impresion.o tbool.o realprec.o except.o macroexp.o tokit.o value.o

NODEPS = clean

include $(KERNEL_SRC)/Makefile.defs
include $(KERNEL_SRC)/Makefile.rules


all   : $(OBJS_W_DIR) $(TOOL_NAME)

$(TOOL_NAME) :
	@mkdir -p ${OUT_DIR}
	$(eval ALL_OBJS = $(shell find ../../ -type f -name '*.o'))
	$(eval REQUIRED_OBJS = $(filter $(addprefix %, $(DEPS_OBJ)), $(ALL_OBJS)))
	${CPP} ${CPPFLAGS} ${LDFLAGS} -o ${OUT_DIR}/$@ ${REQUIRED_OBJS} ${OBJS_W_DIR}

clean :
	@rm -rf $(DEPS) $(OBJ_DIR)
	
ifeq (0, $(words $(findstring $(MAKECMDGOALS), $(NODEPS))))
sinclude $(DEPS)
endif
KERNEL_SRC = $(CURDIR)
SRC_DIR    = $(KERNEL_SRC)/..

include $(KERNEL_SRC)/Makefile.rules
include $(KERNEL_SRC)/Makefile.defs


ifndef OUT_DIR
OUT_DIR = $(SRC_DIR)/bin
endif

OBJ_DIR           = obj

EXECUTABLE        = cd++
LIB_NAME          = simu
LIBRARY           = lib$(LIB_NAME).a

LIB_COMPONENTS    = atomics cell evt misc model msg parser proc sim utils val
NONLIB_COMPONENTS = main warped
ALL_COMPONENTS    = $(LIB_COMPONENTS) $(NONLIB_COMPONENTS)
CLEAN_COMPONENTS  = $(addsuffix .clean, $(ALL_COMPONENTS))


all : $(ALL_COMPONENTS) $(LIBRARY) $(EXECUTABLE)

lib : $(LIBRARY)

clean : $(CLEAN_COMPONENTS)
	@rm -rf ${OUT_DIR}  

$(ALL_COMPONENTS) :
	$(MAKE) -C $(KERNEL_SRC)/$@ OBJ_DIR=$(OBJ_DIR) SRC_DIR=$(KERNEL_SRC) MODEL_FLAGS=$(MODEL_FLAGS)
	
$(CLEAN_COMPONENTS) :
	$(MAKE) -C $(KERNEL_SRC)/$(basename $@) clean OBJ_DIR=$(OBJ_DIR) SRC_DIR=$(KERNEL_SRC)

$(EXECUTABLE) : $(ALL_COMPONENTS) $(LIBRARY)
	@mkdir -p ${OUT_DIR}
	$(eval NONLIB_OBJS = $(shell find $(NONLIB_COMPONENTS) -type f -name '*.o'))
	$(eval MODEL_OBJS = $(shell find $(OUT_DIR) -type f -name '*.o'))
	${CPP} ${LDFLAGS} -o ${OUT_DIR}/$@ ${NONLIB_OBJS} ${MODEL_OBJS} -l${LIB_NAME}
	
$(LIBRARY) : $(LIB_COMPONENTS)
	$(eval LIB_OBJS = $(shell find $(LIB_COMPONENTS) -type f -name '*.o'))
	@mkdir -p ${OUT_DIR}
	@ar crs ${OUT_DIR}/$@ ${LIB_OBJS}
	
	
.PHONY : $(ALL_COMPONENTS) $(CLEAN_COMPONENTS) $(LIBRARY) $(EXECUTABLE)
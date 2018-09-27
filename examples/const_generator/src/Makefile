KERNEL_SRC    = ../../../src/cd++

OUT_DIR       = $(CURDIR)/bin
OBJ_DIR       = $(OUT_DIR)

MODEL_FLAGS   = -DREGISTER_ATOMICS

SOURCES       = $(wildcard *.cpp)
DEPS          = $(SOURCES:.cpp=.dep)
OBJS          = $(SOURCES:.cpp=.o)
OBJS_W_DIR    = $(addprefix ${OBJ_DIR}/, $(notdir $(OBJS)))

NODEPS = clean

include $(KERNEL_SRC)/Makefile.rules
include $(KERNEL_SRC)/Makefile.defs


all   : $(OBJS_W_DIR) kernel

kernel :
	$(MAKE) -C $(KERNEL_SRC) cd++ OUT_DIR=$(OUT_DIR) MODEL_FLAGS=$(MODEL_FLAGS)

clean :
	@rm -rf *.dep $(OBJ_DIR) $(OUT_DIR)

ifeq (0, $(words $(findstring $(MAKECMDGOALS), $(NODEPS))))
sinclude $(DEPS)
endif

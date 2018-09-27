ALL_SOURCES      = $(wildcard *.cpp)
SOURCES          = $(filter-out modeladm.cpp smodeladm.cpp, $(ALL_SOURCES))

DEPS             = $(SOURCES:.cpp=.dep)
OBJS             = $(SOURCES:.cpp=.o)
OBJS_W_DIR       = $(addprefix ${OBJ_DIR}/, $(OBJS))

COMPONENTS       = state
CLEAN_COMPONENTS = $(addsuffix .clean, $(COMPONENTS))

NODEPS = clean

include ../Makefile.rules


all   : $(COMPONENTS) $(OBJS_W_DIR)

$(COMPONENTS) :
	$(MAKE) -C $@ OBJ_DIR=$(OBJ_DIR) SRC_DIR=$(SRC_DIR)

clean : $(CLEAN_COMPONENTS)
	@rm -rf $(DEPS) $(OBJ_DIR)
	
$(CLEAN_COMPONENTS) :
	$(MAKE) -C $(basename $@) clean OBJ_DIR=$(OBJ_DIR) SRC_DIR=$(SRC_DIR)
	
ifeq (0, $(words $(findstring $(MAKECMDGOALS), $(NODEPS))))
sinclude $(DEPS)
endif	
	
	
.PHONY : $(COMPONENTS) $(CLEAN_COMPONENTS)
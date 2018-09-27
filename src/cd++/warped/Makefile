COMPONENTS       = common notime
CLEAN_COMPONENTS = $(addsuffix .clean, $(COMPONENTS))

include ../Makefile.rules


all   : $(COMPONENTS)

$(COMPONENTS) :
	$(MAKE) -C $@ OBJ_DIR=$(OBJ_DIR) SRC_DIR=$(SRC_DIR)

clean : $(CLEAN_COMPONENTS)
	@rm -rf $(DEPS) $(OBJ_DIR)
	
$(CLEAN_COMPONENTS) :
	$(MAKE) -C $(basename $@) clean OBJ_DIR=$(OBJ_DIR) SRC_DIR=$(SRC_DIR)	
	
	
.PHONY : $(COMPONENTS) $(CLEAN_COMPONENTS)
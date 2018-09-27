TOOLS = drawlog makerand parlog toCDPP toMap

CLEAN_TOOLS = $(addsuffix .clean, $(TOOLS))


all   : $(TOOLS) 

clean : $(CLEAN_TOOLS)
	
$(TOOLS) :
	$(MAKE) -C $@ OUT_DIR=$(OUT_DIR) OBJ_DIR=$(OBJ_DIR)

$(CLEAN_TOOLS) :
	$(MAKE) -C $(basename $@) clean OBJ_DIR=$(OBJ_DIR)
	
	
.PHONY : $(TOOLS) $(CLEAN_TOOLS)

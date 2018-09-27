EXAMPLES       = $(shell find $(CURDIR) -mindepth 1 -maxdepth 1 -type d)
CLEAN_EXAMPLES = $(addsuffix .clean, $(EXAMPLES))


all   : $(EXAMPLES) 

clean : $(CLEAN_EXAMPLES)
	
$(EXAMPLES) :
	$(MAKE) -C $@

$(CLEAN_EXAMPLES) :
	$(MAKE) -C $(basename $@) clean
	
	
.PHONY : $(EXAMPLES) $(CLEAN_EXAMPLES)

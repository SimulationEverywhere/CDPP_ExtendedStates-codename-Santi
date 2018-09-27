KERNEL_SRC = $(CURDIR)/cd++

ifndef OUT_DIR
OUT_DIR = $(CURDIR)/bin
endif

OBJ_DIR   = obj
CDPP      = cd++
TOOLS     = tools


all : $(CDPP)

clean : 
	@rm -rf $(OUT_DIR)  
	$(MAKE) -C $(CDPP) clean OBJ_DIR=$(OBJ_DIR)
	$(MAKE) -C $(TOOLS) clean OBJ_DIR=$(OBJ_DIR)

$(CDPP) : 
	$(MAKE) -C $@ OUT_DIR=$(OUT_DIR) OBJ_DIR=$(OBJ_DIR)
	
$(TOOLS) : $(CDPP)
	$(MAKE) -C $@ OUT_DIR=$(OUT_DIR) OBJ_DIR=$(OBJ_DIR)

	
.PHONY : $(CDPP) $(TOOLS)

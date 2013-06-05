.PHONY: all clean realclean

export OUTPUT_PATH=./output

all :
	mkdir -p $(OUTPUT_PATH)
	cp myjfmspider.conf.sample $(OUTPUT_PATH)/myjfmspider.conf
	@echo
	@echo "make src..."
	@$(MAKE) -C src
	@echo "make src ok."
	@echo

clean :
	@$(MAKE) clean -C src

realclean :
	@$(MAKE) realclean -C src
	rm $(OUTPUT_PATH) -rf


.PHONY: all clean realclean

export OUTPUT_PATH=./output

all :
	mkdir -p $(OUTPUT_PATH)
	cp myjfm_spider.conf.sample $(OUTPUT_PATH)/myjfm_spider.conf
	cp scripts/start scripts/stop $(OUTPUT_PATH)
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


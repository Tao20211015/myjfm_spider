.PHONY: all clean realclean

export OUTPUT_PATH=./output

all :
	mkdir -p $(OUTPUT_PATH)
	cp myjfm_spider.conf.sample $(OUTPUT_PATH)/myjfm_spider.conf
	cp scripts/start scripts/stop $(OUTPUT_PATH)
	@echo
	@echo "make deps..."
	@$(MAKE) -C deps
	@echo "make deps ok."
	@echo
	@echo "make src..."
	@$(MAKE) -C src
	@echo "make src ok."
	@echo

clean :
	@$(MAKE) clean -C src

realclean :
	@$(MAKE) realclean -C src
	@$(MAKE) realclean -C deps
	rm $(OUTPUT_PATH) -rf


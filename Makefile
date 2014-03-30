.PHONY: clean All

All:
	@echo "----------Building project:[ irrlicht - Release ]----------"
	@cd "irrlicht" && $(MAKE) -f  "irrlicht.mk"
clean:
	@echo "----------Cleaning project:[ irrlicht - Release ]----------"
	@cd "irrlicht" && $(MAKE) -f  "irrlicht.mk" clean

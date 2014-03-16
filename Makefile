.PHONY: clean All

All:
	@echo "----------Building project:[ irrlicht - Debug ]----------"
	@cd "irrlicht" && $(MAKE) -f  "irrlicht.mk"
clean:
	@echo "----------Cleaning project:[ irrlicht - Debug ]----------"
	@cd "irrlicht" && $(MAKE) -f  "irrlicht.mk" clean

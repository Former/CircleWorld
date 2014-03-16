.PHONY: clean All

All:
	@echo "----------Building project:[ CircleWorld - Release ]----------"
	@$(MAKE) -f  "CircleWorld.mk"
clean:
	@echo "----------Cleaning project:[ CircleWorld - Release ]----------"
	@$(MAKE) -f  "CircleWorld.mk" clean

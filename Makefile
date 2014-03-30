.PHONY: clean All

All:
	@echo "----------Building project:[ CircleWorld - Debug ]----------"
	@$(MAKE) -f  "CircleWorld.mk"
clean:
	@echo "----------Cleaning project:[ CircleWorld - Debug ]----------"
	@$(MAKE) -f  "CircleWorld.mk" clean

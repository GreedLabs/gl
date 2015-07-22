include make/config.mk

TARGETS = context.proj cube.proj transformations.proj

include make/recursive.mk

clean:
	$(RM) -r $(BUILD_DIR) $(DIST_BLD_DIR)

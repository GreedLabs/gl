include make/config.mk

TARGETS = proj.proj

include make/recursive.mk

clean:
	$(RM) -r $(BUILD_DIR) $(DIST_BLD_DIR)

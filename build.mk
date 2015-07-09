OBJ = $(addprefix $(BUILD_DIR)/, $(SRC:.cc=.o))

all: $(TARGET)

$(TARGET): $(OBJ) $(DIST_OBJ)
	$(LINK.c) -o $(TARGET) $^ $(LDLIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cc
	mkdir -p $(@D)
	$(COMPILE.c) -o $@ $^

$(DIST_BLD_DIR)/%.o: $(DIST_SRC_DIR)/%.cc
	mkdir -p $(@D)
	$(COMPILE.c) -o $@ $^
clean:
	$(RM) $(OBJ) $(DIST_OBJ)
	$(RM) -rf $(BUILD_DIR) $(DIST_BLD_DIR)
	$(RM) $(TARGET)

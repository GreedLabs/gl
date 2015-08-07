OBJ = $(addprefix $(BUILD_DIR)/, $(SRC:.cc=.o))

all: $(TARGET)

$(TARGET): $(OBJ) $(DIST_OBJ) | bin
	$(LINK.c) -o $(TARGET) -o $(ABS_PATH)/bin/$(TARGET) $^ $(LDLIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cc
	mkdir -p $(@D)
	$(COMPILE.c) -o $@ $^

$(DIST_BLD_DIR)/%.o: $(DIST_SRC_DIR)/%.cc
	mkdir -p $(@D)
	$(COMPILE.c) -o $@ $^

bin:
	mkdir -p $(ABS_PATH)/bin

clean:
	$(RM) $(OBJ) $(DIST_OBJ)
	$(RM) $(TARGET)
	$(RM) -r $(BUILD_DIR)
	$(RM) -r $(ABS_PATH)/bin

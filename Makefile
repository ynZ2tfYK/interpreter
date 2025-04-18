CC = gcc
CFLAGS = -Wall -fPIC -g
LDFLAGS = -lm -ldl
PLUGIN_LDFLAGS = -shared
MATH_LDFLAGS = -lm
UPX = upx-ucl

SRC_DIR = plugins_c
PLUGIN_DIR = plugins
BIN = interpreter

# Source files for the interpreter
SRC = interpreter.c eval.c parse.c

# Plugin source files
PLUGINS_SRC = $(SRC_DIR)/add_plugin.c \
              $(SRC_DIR)/sub_plugin.c \
              $(SRC_DIR)/mul_plugin.c \
              $(SRC_DIR)/div_plugin.c \
              $(SRC_DIR)/pow_plugin.c \
              $(SRC_DIR)/mod_plugin.c

# Plugin binary outputs (shared libraries)
PLUGINS = $(PLUGIN_DIR)/add_plugin.so \
          $(PLUGIN_DIR)/sub_plugin.so \
          $(PLUGIN_DIR)/mul_plugin.so \
          $(PLUGIN_DIR)/div_plugin.so \
          $(PLUGIN_DIR)/pow_plugin.so \
          $(PLUGIN_DIR)/mod_plugin.so

# Default target
all: $(BIN) $(PLUGINS)

# Interpreter binary (executable)
$(BIN): $(SRC) | $(PLUGIN_DIR)
	$(CC) $(CFLAGS) -o $@ $(SRC) $(LDFLAGS)
	$(UPX) $@

# Plugins not requiring -lm (shared library binaries)
$(PLUGIN_DIR)/add_plugin.so: $(SRC_DIR)/add_plugin.c | $(PLUGIN_DIR)
	$(CC) $(CFLAGS) $(PLUGIN_LDFLAGS) -o $@ $<
	$(UPX) $@

$(PLUGIN_DIR)/sub_plugin.so: $(SRC_DIR)/sub_plugin.c | $(PLUGIN_DIR)
	$(CC) $(CFLAGS) $(PLUGIN_LDFLAGS) -o $@ $<
	$(UPX) $@

$(PLUGIN_DIR)/mul_plugin.so: $(SRC_DIR)/mul_plugin.c | $(PLUGIN_DIR)
	$(CC) $(CFLAGS) $(PLUGIN_LDFLAGS) -o $@ $<
	$(UPX) $@

# Plugins requiring -lm (shared library binaries)
$(PLUGIN_DIR)/div_plugin.so: $(SRC_DIR)/div_plugin.c | $(PLUGIN_DIR)
	$(CC) $(CFLAGS) $(PLUGIN_LDFLAGS) -o $@ $< $(MATH_LDFLAGS)
	$(UPX) $@

$(PLUGIN_DIR)/pow_plugin.so: $(SRC_DIR)/pow_plugin.c | $(PLUGIN_DIR)
	$(CC) $(CFLAGS) $(PLUGIN_LDFLAGS) -o $@ $< $(MATH_LDFLAGS)
	$(UPX) $@

$(PLUGIN_DIR)/mod_plugin.so: $(SRC_DIR)/mod_plugin.c | $(PLUGIN_DIR)
	$(CC) $(CFLAGS) $(PLUGIN_LDFLAGS) -o $@ $< $(MATH_LDFLAGS)
	$(UPX) $@

# Create plugins directory
$(PLUGIN_DIR):
	mkdir -p $(PLUGIN_DIR)

# Clean up
clean:
	rm -f $(BIN) $(PLUGINS)
	rm -rf $(PLUGIN_DIR)

.PHONY: all clean

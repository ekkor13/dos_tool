CC = g++
CFLAGS = -Wall -std=c++17 -O2 -I./cli -I./core -I./utils
LDFLAGS = -lpthread
TARGET = dos_tool

SRC_FILES = main.cpp \
	cli/cli.cpp \
	core/attacker.cpp \
	core/packet.cpp \
	utils/checksum.cpp

OBJ_FILES = main.o \
	cli/cli.o \
	core/attacker.o \
	core/packet.o \
	utils/checksum.o

all: $(TARGET)

$(TARGET): $(OBJ_FILES)
	@echo "Linking executable..."
	$(CC) $(OBJ_FILES) -o $(TARGET) $(LDFLAGS)
	@echo "Build complete. File: $(TARGET)"

cli/%.o: cli/%.cpp
	@echo "Compiling $<"
	$(CC) $(CFLAGS) -c $< -o $@

core/%.o: core/%.cpp
	@echo "Compiling $<"
	$(CC) $(CFLAGS) -c $< -o $@

utils/%.o: utils/%.cpp
	@echo "Compiling $<"
	$(CC) $(CFLAGS) -c $< -o $@

main.o: main.cpp
	@echo "Compiling $<"
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "Removing object files and executable..."
	rm -f $(TARGET) $(OBJ_FILES) cli/*.o core/*.o utils/*.o
	@echo "Cleanup complete."

install: $(TARGET)
	@echo "Installing $(TARGET) to /usr/local/bin/..."
	@sudo cp $(TARGET) /usr/local/bin/
	@echo "Installation complete. Run with 'sudo $(TARGET)'"

uninstall:
	@echo "Removing $(TARGET) from /usr/local/bin/..."
	@sudo rm -f /usr/local/bin/$(TARGET)
	@echo "Uninstallation complete."

.PHONY: all clean install uninstall

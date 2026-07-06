.PHONY: all build clean run install help

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
LDFLAGS = -lfltk -lfltk_images

# Output binary
TARGET = Draftz

# Source files
SOURCES = main.cpp
OBJECTS = $(SOURCES:.cpp=.o)

# Default target
all: $(TARGET)

# Build target
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS) $(LDFLAGS)
	@echo "Build complete! Run './$(TARGET)' to start."

# Compile source files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(OBJECTS) $(TARGET)
	@echo "Clean complete!"

# Run the application
run: $(TARGET)
	./$(TARGET)

# Install (optional)
install: $(TARGET)
	install -m 755 $(TARGET) /usr/local/bin/
	@echo "Installed $(TARGET) to /usr/local/bin/"

# Help
help:
	@echo "Available targets:"
	@echo "  make        - Build the project"
	@echo "  make run    - Build and run the application"
	@echo "  make clean  - Remove build artifacts"
	@echo "  make install - Install binary to /usr/local/bin/"

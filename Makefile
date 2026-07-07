# .PHONY: all build clean run install help

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iheader
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

# Release build (optimized and packaged)
release: CXXFLAGS += -O3
release: clean $(TARGET)
	@echo "Release build complete."
	@mkdir -p dist
	@cp $(TARGET) dist/
	@tar czf dist/$(TARGET)-$(shell uname -s)-$(shell uname -m).tar.gz -C dist $(TARGET) || true
	@zip -j dist/$(TARGET)-$(shell uname -s)-$(shell uname -m).zip dist/$(TARGET) || true
	@echo "Artifacts placed in dist/"

# Create a simple Debian package (Linux runners only)
deb: release
	@echo "Creating simple .deb package (amd64)..."
	@rm -rf package
	@mkdir -p package/DEBIAN package/usr/local/bin
	@printf "Package: draftz\nVersion: 1.0.0\nSection: editors\nPriority: optional\nArchitecture: amd64\nMaintainer: xhaevrithon <xhaevrithon@example.com>\nDescription: Draftz - simple lightweight text editor built with FLTK\n" > package/DEBIAN/control
	@cp $(TARGET) package/usr/local/bin/
	@dpkg-deb --build package draftz_1.0.0_amd64.deb || true
	@mv draftz_1.0.0_amd64.deb dist/ || true
	@echo "Debian package (if built) is in dist/"

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
	@echo "  make release - Build optimized release and create artifacts in dist/"
	@echo "  make deb    - (Linux) Create a simple .deb package (amd64)"
	@echo "  make run    - Build and run the application"
	@echo "  make clean  - Remove build artifacts"
	@echo "  make install - Install binary to /usr/local/bin/"

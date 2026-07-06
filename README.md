# Draftz

**Draftz** is a simple, lightweight text editor built with C++ and FLTK (Fast Light Toolkit). It provides essential text editing features in a clean, minimalist interface.

## Features

- ✏️ **Write text** - Clean, distraction-free editing experience
- 📄 **New** - Create new documents
- 📂 **Open** - Load existing text files
- 💾 **Save** - Save your work with ease
- ↩️ **Undo** - Undo your last actions
- ↪️ **Redo** - Redo undone actions

## Requirements

### Debian/Ubuntu
```bash
sudo apt-get install libfltk1.3-dev cmake build-essential
```

### macOS (with Homebrew)
```bash
brew install fltk cmake
```

### Windows
- Download FLTK from [fltk.org](https://www.fltk.org/)
- Install CMake from [cmake.org](https://cmake.org/)
- Or use vcpkg: `vcpkg install fltk:x64-windows`

## Compilation

### Using CMake (Recommended for all platforms)

#### Debian/Ubuntu
```bash
git clone https://github.com/xhaevrithon/Draftz.git
cd Draftz
mkdir build
cd build
cmake ..
make
./Draftz
```

#### macOS
```bash
git clone https://github.com/xhaevrithon/Draftz.git
cd Draftz
mkdir build
cd build
cmake ..
make
./Draftz
```

#### Windows (with Visual Studio)
```bash
git clone https://github.com/xhaevrithon/Draftz.git
cd Draftz
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022"
cmake --build . --config Release
.\Release\Draftz.exe
```

#### Windows (with MinGW)
```bash
git clone https://github.com/xhaevrithon/Draftz.git
cd Draftz
mkdir build
cd build
cmake .. -G "MinGW Makefiles"
mingw32-make
.\Draftz.exe
```

### Using Make (Direct compilation - Linux/macOS only)

If you prefer not to use CMake:

```bash
git clone https://github.com/xhaevrithon/Draftz.git
cd Draftz
make
./Draftz
```

Available make targets:
```bash
make          # Build the project
make run      # Build and run the application
make clean    # Remove build artifacts
make install  # Install binary to /usr/local/bin/
make help     # Show all available targets
```

### Direct g++ compilation (Linux/macOS only)

```bash
g++ main.cpp -std=c++17 -lfltk -lfltk_images -o Draftz
./Draftz
```

## Usage

1. Launch the application:
   ```bash
   ./Draftz
   ```

2. Start typing in the text area

3. Use the menu bar to:
   - **File > New** - Create a new document
   - **File > Open** - Open an existing file
   - **File > Save** - Save your current document
   - **Edit > Undo** - Undo last action
   - **Edit > Redo** - Redo last action

## Project Structure

```
Draftz/
├── CMakeLists.txt    # CMake configuration file
├── Makefile          # Makefile for direct compilation
├── main.cpp          # Main application source code
├── README.md         # This file
├── LICENSE           # MIT License
└── .gitignore        # Git ignore patterns
```

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Contributing

Contributions are welcome! Feel free to fork this project and submit pull requests.

## Author

**xhaevrithon**

## Support

For issues, questions, or suggestions, please open an [issue](https://github.com/xhaevrithon/Draftz/issues) on GitHub.

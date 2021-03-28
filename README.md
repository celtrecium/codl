# codl
Cursor-Optimized Drawing Library.

This is curses-like library, written on C. The library is fully supported by GNU/Linux and Windows 10.

Thanks to Alexey Kachaev(https://github.com/Gebet94) for porting the codl_get_key() function to Windows.

# Build and install

GNU/Linux:

1. `mkdir build/`
2. `cd build`
3. `cmake ..`
4. `make`
5. `sudo make install`

# Uninstall
Run `sudo xargs rm < install_manifest.txt` command

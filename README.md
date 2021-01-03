# codl
Cursor-Optimized Drawing Library.

This is curses-like library, written on C. Currently, the library is fully supported by GNU/Linux. Windows version doesn't support input functions (`codl_get_key`, `codl_input_form`).

# Build and install

GNU/Linux:

1. `mkdir build/`
2. `cd build`
3. `cmake ..`
4. `make`
5. `sudo make install`

# Uninstall
Run `sudo xargs rm < install_manifest.txt` command

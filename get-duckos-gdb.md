1. `git clone lone binutils-gdb`
2. `cd binutils-gdb`
3. `mkdir build-gdb`
4. `cd build-gdb`
5. ```bash
    ../configure  --target=i386-pc-linux-gnu \
    --prefix="/home/.../DuckOS/DuckOS-gdb" \
    --with-python=yes --enable-tui \
    --with-curses --disable-nls \
    --disable-werror --disable-gas \
    --disable-binutils --disable-ld \
    --disable-gprof
    ```
6. `make`
7. If 6 fails, install any missing libraries, try 6 again.
8. `make install`
CC=clang
SRC_DIR=src
LIB_DIR=/lib
INCLUDE_DIR=/usr/include
SRC_FILES=$(wildcard $(SRC_DIR)/*.c)
OBJ_FILES=$(patsubst $(SRC_DIR)/%.c, %.o, $(SRC_FILES))
CFLAGS=-Iinclude -pedantic -Wall -Wextra -Wsign-conversion -Wconversion -Wshadow -fPIC

all: $(OBJ_FILES) libcodl.so

%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

libcodl.so: $(OBJ_FILES)
	$(CC) -shared $^ -o $@
	rm -rf $(OBJ_FILES)

install:
	cp libcodl.so $(LIB_DIR)
	cp include/codl.h $(INCLUDE_DIR)/

uninstall:
	rm -rf libcodl.so $(LIB_DIR)
	rm -rf $(INCLUDE_DIR)/codl.h


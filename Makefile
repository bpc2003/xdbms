CC = gcc
BUILD = target
C_FLAGS = -Wall -lmdb -std=c11 -O2
D_FLAGS = -L$(BUILD) -Wl,-rpath=$(BUILD)
L_FLAGS = -c -fPIC -Wall -Werror

all: mdb
mdb: lib
	$(CC) src/*.c $(D_FLAGS) $(C_FLAGS) -o $(BUILD)/mdb.out
lib: $(BUILD)
	$(CC) src/lib/*.c $(L_FLAGS)
	$(CC) -shared -o $(BUILD)/libmdb.so *.o
	rm *.o

dev: dev_lib
	$(CC) src/*.c $(D_FLAGS) -g $(C_FLAGS) -o $(BUILD)/devmdb.out
dev_lib: $(BUILD)
	$(CC) src/lib/*.c $(L_FLAGS) -g
	$(CC) -shared -o $(BUILD)/libmdb.so *.o
	rm *.o

$(BUILD):
	mkdir $(BUILD)

install: lib
	cp src/lib/mdb.h /usr/include/mdb.h
	mv $(BUILD)/libmdb.so /usr/lib/libmdb.so
	ldconfig
	$(CC) src/*.c $(C_FLAGS) -o /usr/bin/mdb

uninstall:
	rm /usr/bin/mdb
	rm /usr/lib/libmdb.so
	rm /usr/include/mdb.h
remove: uninstall

mdb: lib
	gcc src/*.c -Lsrc/lib -lmdb -std=c11 -O2 -o mdb.out
lib:
	gcc src/lib/*.c -c -O2
	ld -r *.o -o src/lib/libmdb.so
	rm -rf *.o

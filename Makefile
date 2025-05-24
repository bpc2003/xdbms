CC = clang
BUILD = target
C_FLAGS = -Wall -lxdbms -std=c11
D_FLAGS = -L$(BUILD) -Wl,-rpath=$(BUILD)
O_FLAGS = -c -fPIC -Wall -Wextra

all: xdbms
xdbms: lib
	$(CC) src/main.c $(D_FLAGS) -O2 $(C_FLAGS) -o $(BUILD)/xdbms.out
lib: $(BUILD)
	$(CC) src/include/engine/*.c $(O_FLAGS) -std=c11
	ar rcs $(BUILD)/libengine.a *.o
	rm *.o
	$(CC) src/include/xml/*.c $(O_FLAGS) -std=c11
	ar rcs $(BUILD)/libxml.a *.o
	$(CC) src/include/xdbms.c $(O_FLAGS) -std=c11
	$(CC) -shared -o $(BUILD)/libxdbms.so\
		xdbms.o $(BUILD)/libengine.a $(BUILD)/libxml.a\
		-Wl,--exclude-libs,$(BUILD)/libengine.a -fvisibility=hidden\
		-Wl,--exclude-libs,$(BUILD)/libxml.a -fvisibility=hidden -O2
	rm *.o
	rm $(BUILD)/*.a

dev: dev_lib
	$(CC) src/main.c $(D_FLAGS) -O0 -g $(C_FLAGS) -o $(BUILD)/xdbms.out
dev_lib: $(BUILD)
	$(CC) src/include/engine/*.c $(O_FLAGS) -g -std=c11
	ar rcs $(BUILD)/libengine.a *.o
	rm *.o
	$(CC) src/include/xml/*.c $(O_FLAGS) -g -std=c11
	ar rcs $(BUILD)/libxml.a *.o
	$(CC) src/include/xdbms.c $(O_FLAGS) -std=c11 -g
	$(CC) -shared -o $(BUILD)/libxdbms.so\
		xdbms.o $(BUILD)/libengine.a $(BUILD)/libxml.a -g
	rm *.o

test: $(BUILD)
	$(CC) -I. -g tests/*.c src/include/engine/*.c src/include/xml/*.c unity/unity.c -o $(BUILD)/test.out
	valgrind --tool=memcheck --log-file=mem_dbg $(BUILD)/test.out
	valgrind --tool=drd -s --log-file=thrd_dbg $(BUILD)/test.out
	rm -rf $(BUILD)

$(BUILD):
	mkdir $(BUILD)

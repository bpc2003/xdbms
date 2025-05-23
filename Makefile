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
	$(CC) src/include/init.c $(O_FLAGS) -std=c11
	$(CC) -shared -o $(BUILD)/libxdbms.so init.o $(BUILD)/libengine.a\
		-Wl,--exclude-libs,$(BUILD)/libengine.a -fvisibility=hidden -O2
	rm *.o
	rm $(BUILD)/libengine.a

test: $(BUILD)
	$(CC) -I. tests/*.c src/include/engine/*.c unity/unity.c -o $(BUILD)/test.out
	valgrind --tool=memcheck --log-file=mem_dbg $(BUILD)/test.out
	valgrind --tool=drd -s --log-file=thrd_dbg $(BUILD)/test.out
	rm -rf $(BUILD)

$(BUILD):
	mkdir $(BUILD)

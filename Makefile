preload.so: preload.o
	gcc -shared -o preload.so preload.o

preload.o: preload.c
	gcc $(CFLAGS) -c preload.c -fPIC -g -o preload.o

clean:
	rm -f arenaalloc.so preload.o

.PHONY: clean

CC=clang
CFLAGS=-Wall -g

BINS= libmyalloc.so

all: $(BINS)

libmyalloc.so: allocator.c
	$(CC) $(CFLAGS) -fPIC -shared -ldl -o libmyalloc.so allocator.c

%: %.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm $(BINS)
	
	

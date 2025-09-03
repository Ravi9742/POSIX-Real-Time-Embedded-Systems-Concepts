#CFLAGS = -DDEBUG
CFLAGS = -Wall

beowulfpc: twoprocs.c
	gcc $(CFLAGS) -o twoprocs  twoprocs.c -lrt -pthread

clean:
	rm -f twoprocs

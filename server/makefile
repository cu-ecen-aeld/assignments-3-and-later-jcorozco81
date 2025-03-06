CC = $(CROSS_COMPILE)gcc
CFLAGS = -Wall -g
LDFLAGS = -lpthread
TARGET: aesdsocket

all : $(TARGET)

aesdsocket : aesdsocket.o

		$(CC) $(CFLAGS) -o aesdsocket aesdsocket.o $(LDFLAGS)

aesdsocket.o : aesdsocket.c
		$(CC) -c aesdsocket.c

clean :
		rm -f *.o *.out
# Set compiler to use
CC=g++
CFLAGS=-g -I. -fpermissive
LDFLAGS=
OBJS=Main.o Matrix.o Tetris.o CTetris.o ttymodes.o
DEBUG=0

all:: Main.exe

Main.exe: $(OBJS)
	$(CC) -pthread -o $@ $^ $(CFLAGS) $(LDFLAGS_TET)

%.o: %.c $(DEPS)
	$(CC) -c -pthread -o $@ $< $(CFLAGS)

%.o: %.cpp $(DEPS)
	$(CC) -c -pthread -o $@ $< $(CFLAGS)

clean: 
	rm -f *.exe *.o *~ *.stackdump

# Set compiler to use
CC=g++
CFLAGS=-g -I. -fpermissive -lncursesw
LDFLAGS=
OBJS=Main.o Matrix.o Tetris.o CTetris.o ttymodes.o
DEBUG=0

all:: Main.exe Server.exe

Server.exe: Server.cpp
	$(CC) -o $@ $<

Main.exe: $(OBJS)
	$(CC) -pthread -o $@ $^ $(CFLAGS) $(LDFLAGS_TET)

%.o: %.c $(DEPS)
	$(CC) -c -pthread -o $@ $< $(CFLAGS)

%.o: %.cpp $(DEPS)
	$(CC) -c -pthread -o $@ $< $(CFLAGS)

clean: 
	rm -f *.exe *.o *~ *.stackdump

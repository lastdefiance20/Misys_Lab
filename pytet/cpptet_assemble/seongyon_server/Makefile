# Set compiler to use
CC=g++
CFLAGS=-g -I. -fpermissive
LDFLAGS=
OBJS_CLIENT=Main.o Matrix.o Tetris.o ttymodes.o CTetris.o ModelView.o SendRecv.o
OBJS_SERVER=server.o Matrix.o Tetris.o ttymodes.o CTetris.o ModelView.o SendRecv.o
DEBUG=0

all:: Server.exe

Server.exe: $(OBJS_SERVER)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS_TET) -lpthread -lncursesw

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

clean: 
	rm -f *.exe *.o *~ *.stackdump

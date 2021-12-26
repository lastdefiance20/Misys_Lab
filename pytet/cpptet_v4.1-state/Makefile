# Set compiler to use
CC=g++
CFLAGS=-g -I. -fpermissive -lncursesw
LDFLAGS=
MAIN_OBJS=Main.o Matrix.o Tetris.o CTetris.o ttymodes.o ClassSetup.o View.o Model.o SendController.o RecvController.o
SERVER_OBJS=Server.o Matrix.o Tetris.o CTetris.o ttymodes.o ClassSetup.o View.o Model.o SendController.o RecvController.o
DEBUG=0

all:: Main.exe Server.exe

Server.exe: $(SERVER_OBJS)
	$(CC) -pthread -o $@ $^ $(CFLAGS) $(LDFLAGS_TET)

Main.exe: $(MAIN_OBJS)
	$(CC) -pthread -o $@ $^ $(CFLAGS) $(LDFLAGS_TET)

%.o: %.c $(DEPS)
	$(CC) -c -pthread -o $@ $< $(CFLAGS)

%.o: %.cpp $(DEPS)
	$(CC) -c -pthread -o $@ $< $(CFLAGS)

clean: 
	rm -f *.exe *.o *~ *.stackdump

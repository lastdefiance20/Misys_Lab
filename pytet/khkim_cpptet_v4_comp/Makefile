# Set compiler to use
CC=g++
CFLAGS=-g -I. -fpermissive -lncursesw
LDFLAGS=
MAIN_OBJS=main.o Matrix.o Tetris.o CTetris.o Window.o MsgPubSub.o View.o Model.o TimeCtrl.o KbdCtrl.o ThreadCtrl.o RandomCtrl.o ReplayCtrl.o LogCtrl.o SendCtrl.o WLCtrl.o Umpire.o FlowCtrl.o RecvCtrl.o ttymodes.o
CLIENT_OBJS=cli.o Matrix.o Tetris.o CTetris.o ttymodes.o ClassSetup.o View.o Model.o SendController.o RecvController.o
SERVER_OBJS=Server.o Matrix.o Tetris.o CTetris.o ttymodes.o ClassSetup.o View.o Model.o SendController.o RecvController.o
DEBUG=0

all:: main.exe # cli.exe # svr.exe

main.exe: $(MAIN_OBJS)
	$(CC) -pthread -o $@ $^ $(CFLAGS) $(LDFLAGS_TET)

cli.exe: $(CLIENT_OBJS)
	$(CC) -pthread -o $@ $^ $(CFLAGS) $(LDFLAGS_TET)

svr.exe: $(SERVER_OBJS)
	$(CC) -pthread -o $@ $^ $(CFLAGS) $(LDFLAGS_TET)

%.o: %.c $(DEPS)
	$(CC) -c -pthread -o $@ $< $(CFLAGS)

%.o: %.cpp $(DEPS)
	$(CC) -c -pthread -o $@ $< $(CFLAGS)

clean: 
	rm -f *.exe *.o *~ *.stackdump

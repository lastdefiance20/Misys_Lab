#include "Model.h"

Model::Model(Window *w, string n): mat_msg(MSG_MAT, 0, NULL)
{
  win = w;
  name = n;
  board = new CTetris(20, 15);
  srand((unsigned int)time(NULL));
  Msg tempMsg = Msg(MSG_KEY, (char)('0' + rand() % 7), NULL);
  state = board->accept(&tempMsg);
};

void Model::handle(Msg *msg)
{
  state = board->accept(msg);

  if (state == NewBlock) {
    Msg tempMsg = Msg(MSG_KEY, (char)('0' + rand() % 7), NULL);
    state = board->accept(&tempMsg);
    if (state == Finished) {
      Matrix *matScreen = new Matrix(board->getScreen());
      mat_msg.mat = matScreen;
      notifySubs(&mat_msg);
      win->printw("tetris done.\n");
    }
  }
  Matrix *matScreen = new Matrix(board->getScreen());
  mat_msg.mat = matScreen;
  notifySubs(&mat_msg);
  //win->printw(name + ".handle() called.\n");
  /*
  //win->printw(name + ".handle() called.\n;")
  if (msg->what == MSG_KEY)
    win->printw(name + ": key = " + msg->key + "\n");

  if (msg->what == MSG_KEY) {
    //extern Msg msg3;
    //notifySubs(&msg3);
    mat_msg.mat = new Matrix(arrayScreen, 20, 15);
    notifySubs(&mat_msg);
  }
  */
}


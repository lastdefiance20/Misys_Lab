#include "Model.h"

Model::Model(Window *w, string n): mat_msg(MSG_MAT, 0, NULL)
{
  win = w;
  name = n;

  //CTetris init
  board = new CTetris(20, 15);
};

void Model::handle(Msg *msg)
{
  mat_msg.key = TetrisState(board->accept(msg).key);

  Matrix *matScreen = new Matrix(board->getScreen());
  mat_msg.mat = matScreen;

  notifySubs(&mat_msg);
}


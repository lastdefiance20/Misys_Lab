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
  if ((msg->what == MSG_KEY) || ((msg->what == MSG_MAT) && (TetrisState(msg->key) == NewBlockDelR))){
    mat_msg.key = TetrisState(board->accept(msg).key);

    Matrix *matScreen = new Matrix(board->getData(TetrisState(mat_msg.key)));
    mat_msg.mat = matScreen;

    notifySubs(&mat_msg);
  }
}
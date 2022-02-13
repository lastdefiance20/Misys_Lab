#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>

#include "Matrix.h"

#define MAX_SUBS 100
#define MSG_END (-1)
#define MSG_KEY  (1)
#define MSG_MAT (2)

//Union 데이터타입, char과 matrix로 구성되며 int what으로 타입 및 Game End를 결정한다
class Msg {
 public:
  Msg(int w, char k, Matrix *m) { what = w; key = k; mat = m; } //생성자
  int what; // 1 (MSG_KEY), 2 (MSG_MAT)
  char key; //char
  Matrix *mat; //matrix
};

class Sub { //Subscriber
 protected:
  string name;           // object name 
  queue<Msg*> que;       // input message queue
  mutex mtx;             // mutex to protect 'que'
  condition_variable cv; // to wake up the thread of 'run()'

 public:
  void update(Msg *m);
  void run();
  virtual void handle(Msg *m) { }
};

class Pub { //Publisher
 protected:
  Sub* subs[MAX_SUBS];
  int nsubs = 0;
  
 public:
  void addSubs(Sub* sub);
  void notifySubs(Msg *m);
};


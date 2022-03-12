#include <RecvCtrl.h>

RecvCtrl::RecvCtrl(Window *w, string n, int sock): key_msg(MSG_KEY, 0, NULL)
{
    win = w;
    name = n;
    socket = sock;
};

void RecvCtrl::handle(Msg *msg){
    memset(r_buff, 0, 256);
    int read_chk = read(socket, r_buff, 1); // 읽기 버퍼사이즈-1 만큼 read(읽기)
    if(read_chk == -1){
        //printMsg("read error");
    }else{
        r_buff[strlen(r_buff)] = '\n';
    }
    key_msg.key = r_buff[0];
    notifySubs(&key_msg);
}

void RecvCtrl::run() 
{
  //win->printw(name + ".run() begins.\n");
  Msg *msg;
  while (true) {
    mtx.lock();
    if (que.empty()) 
      msg = NULL;
    else {
      msg = que.front();
      que.pop();
    }
    mtx.unlock();
    //win->printw(name + ".run() wakes up.\n");

	  if (msg && msg->what == MSG_END)
	    break;

    handle(msg);
  }
  win->printw(name + ".run() ends.\n");
};
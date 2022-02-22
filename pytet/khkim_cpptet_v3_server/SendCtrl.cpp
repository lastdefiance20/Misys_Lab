#include <SendCtrl.h>

SendCtrl::SendCtrl(Window *w, string n, int sock)
{
    win = w;
    name = n;
    socket = sock;
};

void SendCtrl::handle(Msg *msg)
{
    char key = msg->key;// 쓰기 버퍼에 문자열 입력
    char w_buff[256];
    w_buff[0] = key;
    int write_chk = write(socket, w_buff, 1);
}
#include <SendController.h>

SendController::SendController(string name, WINDOW *win){
    pname = name;
    prwin = win;
};

void SendController::addclient(int sock, int sock2){
    sock_client = sock;
    sock_client2 = sock2;
}

void SendController::update(char key){
    m.lock();
    keys.push(key);
    cv.notify_all();
    m.unlock();
}

char SendController::read(){
    unique_lock<mutex> lk(m);
    if(keys.empty()) cv.wait(lk, [&]{return !keys.empty();});
    char key = keys.front();
    keys.pop();
    lk.unlock();
    return key;
}

void SendController::setserver(){
    isServerSender = true;
}

void SendController::setmain(){
    isServerSender = false;
}

void SendController::run(){
    if(isServerSender == true){
        char d = 'v';
        char buff[256];
        buff[0] = d;
        int write_chk = write(sock_client, buff, strlen(buff)); // 작성 길이만큼 write(전송)
    }
    
    while(isGameDone == false){
        char key = read();// 쓰기 버퍼에 문자열 입력
        char w_buff[256];
        w_buff[0] = key;

        if((key == 'q')&&(isServerSender == true)){
            char key2 = read();
            if (key2 == 'w'){
                w_buff[1] = '1';
                int write_chk = write(sock_client, w_buff, 2);
                w_buff[1] = '2';
                write_chk = write(sock_client2, w_buff, 2);
            }
            else if(key2 == '=') break;
        }
        else if((key == '=')&&(isServerSender == true)) break;
        else{
            int write_chk = write(sock_client, w_buff, 1); // 작성 길이만큼 write(전송)
        }
    }

    string breaked = " breaked\n";
    pname.append(breaked);
    printMsg(pname, prwin);

    /*
    if(isServerSender == true){
        char d = 'v';
        char buff[256];
        buff[0] = d;
        int write_chk = write(sock_client, buff, strlen(buff)); // 작성 길이만큼 write(전송)
    }
    */
    
}
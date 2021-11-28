#include <SendController.h>

SendController::SendController(string name, WINDOW *win){
    pname = name;
    prwin = win;
};

void SendController::addclient(int sock){
    sock_client = sock;
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
        cout<<"send : "<<key<<endl;

        if(key == 'q'){
            if(isServerSender == true){
                char key2 = read();
                w_buff[1] = key2;
                cout<<"sendd > "<<key2<<endl;
            }
        }

        if(key == '|'){
            break;
        }

        int write_chk = write(sock_client, w_buff, strlen(w_buff)); // 작성 길이만큼 write(전송)
        if(write_chk == -1){
            //printMsg("write error");
            break;
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
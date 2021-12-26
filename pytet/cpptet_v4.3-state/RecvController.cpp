#include <RecvController.h>
#include <Model.h>
#include <SendController.h>

RecvController::RecvController(string name, WINDOW *win){
    pname = name;
    prwin = win;
};

void RecvController::addclient(int sock){
    sock_client = sock;
}

void RecvController::addObserver(Model* observer){
    Mobservers[nMobservers] = observer;
    nMobservers++;
}

void RecvController::addObserver(SendController* observer){
    Sobservers[nSobservers] = observer;
    nSobservers++;
}

void RecvController::notifyObservers(char key){
    for(int i=0; i<nMobservers; i++){
        Mobservers[i]->update(key);
    }
    for(int i=0; i<nSobservers; i++){
        Sobservers[i]->update(key);
    }
}

void RecvController::notifyMObservers(char key){
    for(int i=0; i<nMobservers; i++){
        Mobservers[i]->update(key);
    }
}

void RecvController::setserver(){
    isServerRecv = true;
}

void RecvController::run(){
    char r_buff[256];
    while(isRecvDone == false){
        //get key from server
        memset(r_buff, 0, 256);
        int read_chk = read(sock_client, r_buff, 1); // 읽기 버퍼사이즈-1 만큼 read(읽기)
        if(read_chk == -1){
            //printMsg("read error");
            break;
        }else{
            r_buff[strlen(r_buff)] = '\n';
        }
        char key = r_buff[0];

        //q를 recv할 경우 isgamedone true
        if(key == 'q'){
            notifyMObservers(key);
            if(isServerRecv == false){
                //printMsg("qrecv", prwin);
                int read_chk = read(sock_client, r_buff, 1); // 읽기 버퍼사이즈-1 만큼 read(읽기)
                if(read_chk == -1){
                    //printMsg("read error");
                    break;
                }else{
                    r_buff[strlen(r_buff)] = '\n';
                }
                key = r_buff[0];
                notifyMObservers(key);
                break;
            }
        }
        else if(key == '='){
            notifyMObservers(key);
        }
        else{
            notifyObservers(key);
        }
        if(isGameDone == true){
            if(isServerRecv == false) isRecvDone = true;
            else{ //recv 받을때까지 종료x
                if(key == '|') isRecvDone = true;
            }
        }
    }
/*
    if(isServerRecv == true){
        //get key from server
        memset(r_buff, 0, 256);
        int read_chk = read(sock_client, r_buff, 1); // 읽기 버퍼사이즈-1 만큼 read(읽기)
        if(read_chk == -1){
            //printMsg("read error");
        }else{
            r_buff[strlen(r_buff)] = '\n';
        }
        char key = r_buff[0];

        //|를 recv할 경우 isgamedone true
        notifyObservers(key);
    }
*/
    string breaked = " breaked\n";
    pname.append(breaked);
    printMsg(pname, prwin);
    //notifyObservers('q');
}
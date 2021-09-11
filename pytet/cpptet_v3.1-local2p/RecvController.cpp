#include <RecvController.h>
#include <Model.h>

void RecvController::addclient(int sock){
    sock_client = sock;
}

void RecvController::addObserver(Model* observer){
    Mobservers[nMobservers] = observer;
    nMobservers++;
}

void RecvController::notifyObservers(char key){
    for(int i=0; i<nMobservers; i++){
        Mobservers[i]->update(key);
    }
}

void RecvController::run(){
    char r_buff[256];
    while(isGameDone == false){
        //get key from server
        memset(r_buff, 0, 256);
        int read_chk = read(sock_client, r_buff, sizeof(r_buff)-1); // 읽기 버퍼사이즈-1 만큼 read(읽기)
        if(read_chk == -1){
            //printMsg("read error");
            break;
        }else{
            r_buff[strlen(r_buff)] = '\n';
        }
        char key = r_buff[0];

        //q를 recv할 경우 isgamedone true
        if(key == 'q'){
            isGameDone = true;
        }
        notifyObservers(key);
    }
    notifyObservers('q');
}
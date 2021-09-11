#include <Model.h>
#include <View.h>
#include <SendController.h>

void Model::addObserver(View* observer){
    observers[nobservers] = observer;
    nobservers++;
};

void Model::addObserver(Model* Mobserver){
    Mobservers[Mnobservers] = Mobserver;
    Mnobservers++;
};

void Model::addObserver(SendController* Sobserver){
    Sobservers[Snobservers] = Sobserver;
    Snobservers++;
};

void Model::notifyObservers(Matrix lines){
    for(int i=0; i<Mnobservers; i++){
        Mobservers[i]->update(lines);
    }
};

void Model::notifyObservers(CTetris* board){
    for(int i=0; i<nobservers; i++){
        observers[i]->update(board);
    }
};
    
void Model::notifyObservers(char key){
    for(int i=0; i<Snobservers; i++){
        Sobservers[i]->update(key);
    }
};

Model::Model(string name){
    pname = name;
};

//key를 update할 경우
void Model::update(char key){
    struct keyBox CB;
    CB.key = key;
    CB.iskey = true;
    m.lock();
    CBs.push_back(CB);
    cv.notify_all();
    m.unlock();
};

//지운 line을 update할 경우
void Model::update(Matrix lines){
    struct keyBox CB;
    CB.lines = lines;
    CB.iskey = false;
    m.lock();
    CBs.push_back(CB);
    cv.notify_all();
    m.unlock();
};

void Model::setkeypad(map<char,char> keypad_ex){
    keypad = keypad_ex;
}

void Model::setmainmodel(){
    isMainModel = true;
}

keyBox Model::read(){
    unique_lock<mutex> lk(m);
    //keys에 key가 들어올때까지 대기, sleep 상태에서는 뮤택스 unlocked
    if(CBs.empty()) cv.wait(lk, [&]{return !CBs.empty();});
    struct keyBox CB = CBs.front();
    CBs.pop_front();
    lk.unlock();
    return CB;
}

keyBox Model::check(){
    unique_lock<mutex> lk(m);
    //keys에 key가 들어올때까지 대기, sleep 상태에서는 뮤택스 unlocked
    if(CBs.empty()) cv.wait(lk, [&]{return !CBs.empty();});
    struct keyBox CB = CBs.front();
    lk.unlock();
    return CB;
}

keyBox Model::swap(){
    unique_lock<mutex> lk(m);
    if(CBs.empty()) cv.wait(lk, [&]{return !CBs.empty();});
    struct keyBox TempCB = CBs.front();
    CBs.pop_front();

    if(CBs.empty()) cv.wait(lk, [&]{return !CBs.empty();});
    struct keyBox CB = CBs.front();
    CBs.pop_front();

    CBs.push_front(TempCB);
    lk.unlock();
    return CB;
}

void Model::run(){
    CTetris *board = new CTetris(20, 10); //change
    TetrisState state;
    char key;
    struct keyBox CB;

    if(isMainModel==true){
        srand((unsigned int)time(NULL));
        key = (char)('0' + rand() % 7); //change
        notifyObservers(key);
    }
    else{
        key = read().key;
    }

    CB.iskey = true;
    CB.key = key;
    state = board->accept(CB);
    notifyObservers(board);
    
    while(isGameDone == false) {
        //NewBlockDelR = 줄을 지워서 temp에 저장했을때
        struct keyBox CB = read();
        if(CB.iskey==true){
            key = CB.key;
            if(isMainModel==true) notifyObservers(key);

            //keypad에 존재하면 딕셔너리를 이용해 알맞는 key로 변환
            if(keypad.find(key)!=keypad.end()){
                CB.key = keypad[key];
                state = board->accept(CB);
                if (state == NewBlock) {
                    if(isMainModel==true){
                        key = (char)('0' + rand() % 7); //change
                    }
                    else{
                        struct keyBox CBB;
                        CBB = check();
                        if(CBB.iskey==true){
                            key = read().key;
                        }
                        else{
                            key = swap().key;
                        }
                    }

                    CB.key = key;
                    state = board->accept(CB);

                    if(isMainModel==true) notifyObservers(key);

                    if (state == Finished) {
                        notifyObservers(board);
                        isGameDone = true;
                        break;
                    }
                }

                if (state == NewBlockDelR) {
                    //printMsg("timetodel");
                    delRect = board->getDelRect();
                    notifyObservers(delRect);
                    state = Running;
                }

                notifyObservers(board);
                cout << endl;
                if(key == 'q'){
                    isGameDone = true;
                    break;
                }
            }
        }
        else{
            state = board->accept(CB);
            notifyObservers(board);
            //printMsg("getdel");
        }
    }
    //view가 빠져나올수 있도록 board를 전달해준다
    notifyObservers(board);
    //진사람 출력(먼저 보드가 찬사람)

    /*
    if (returnName==false){
    returnName=true;
    loser=pname;
    }
    string breaked = " breaked\n";
    pname.append(breaked);
    printMsg(pname);
    */

    //cout<<board->currCBlk<<endl;
    delete board;
    //cout<<"deleted really"<<endl;
};
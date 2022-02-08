#include "ModelView.h"

GameState stateFromInteger(int value){
        switch(value){
            case -1: 
                return GameState::Error;
            case 0: 
                return GameState::Initial;
            case 1: 
                return GameState::Running;
            case 2: 
                return GameState::Waiting;
            case 3: 
                return GameState::Finished;
        }
}
int stateFromEnum(GameState state){
    switch(state){
        case GameState::Error: 
            return -1;
        case GameState::Initial: 
            return 0;
        case GameState::Running: 
            return 1;
        case GameState::Waiting: 
            return 2;
        case GameState::Finished: 
            return 3;
    }
}
int cmdFromEnum(GameCommand cmd){
    switch(cmd){
        case GameCommand::NOP: 
            return -1;
        case GameCommand::Quit: 
            return 0;
        case GameCommand::Start: 
            return 1;
        case GameCommand::Update: 
            return 2;
        case GameCommand::Accept: 
            return 3;
    }
}

View::View(string Name){ 
    name=Name;
}
void View::addWindow(WINDOW* window){
    win=window; 
}

void View::updateView(Matrix* screen){ 
    std::unique_lock<std::mutex> lk(m);
    Screens.push(screen);
    cv.notify_all();
    lk.unlock();
}

Matrix* View::read(){ 
    std::unique_lock<std::mutex> lk(m); 
    
    while(Screens.size()<1){
        cv.wait(lk);
    }
    Matrix* obj=Screens.front();
    Screens.pop();
    lk.unlock();
    return obj;
}
void View::printWindow(WINDOW *window, Matrix& screen){ 
  int **array=screen.get_array();
  int dw=CTetris::iScreenDw; 
  int dy=screen.get_dy(); 
  int dx=screen.get_dx();
  wclear(window);
  for(int y=0;y<(dy-dw);y++){
    for(int x=dw;x<dx-dw;x++){ 
        wattron(window,COLOR_PAIR(7));
        if(array[y][x]==0)
            waddstr(window,"□");
        else{
            wattron(window, COLOR_PAIR(array[y][x]));
            waddstr(window,"■");
        }
    }
    waddstr(window,"\n");
  }
  mu.lock(); 
  wrefresh(window); 
  mu.unlock(); 
}
void View::run(){
    while(!isGameDone){ 
        Matrix* screen=read(); 
        if(screen==nullptr) break;
        if(isGameDone) break;
        printWindow(win, *screen);
    }
    sleep(1); 
}

Model::Model(string Name, bool ismine, bool isserver){
    name=Name; 
    isMine=ismine;
    isServer=isserver;
}
void Model::addKeypad(map<char,char>& keypad){ 
    Keypad=keypad; 
}

void Model::addObserverView(View* view){
    observers_view.push_back(view); 
}
void Model::notifyObserversView(Matrix* screen){ 
    for(int i=0;i<observers_view.size();i++){ 
        observers_view[i]->updateView(screen); 
    }
}

void Model::updateKey(char key){ 
    struct keyBox CB;
    CB.key = key;
    CB.iskey = true;
    m.lock();
    CBs.push_back(CB);
    cv.notify_all();
    m.unlock();
}
void Model::addObserverKey(KeyObserver* send){
    observers_send.push_back(send);
}
void Model::notifyObserversKey(char key){
    for(int i=0;i<observers_send.size();i++){
        observers_send[i]->updateKey(key); 
    }
}

void Model::acceptObserver(KeyObserver* Observer){
    result=Observer;
}

void Model::updateDel(Matrix lines){
    struct keyBox CB;
    CB.lines = lines;
    CB.iskey = false;
    m.lock();
    CBs.push_back(CB);
    cv.notify_all();
    m.unlock();
}
void Model::addObserverDel(Model* model){
    models.push_back(model); 
}
void Model::notifyObserversDel(Matrix delRect){ 
    for(int i=0;i<models.size();i++){
        models[i]->updateDel(delRect);
    }
}

keyBox Model::Read(){
    unique_lock<mutex> lk(m);
    //keys에 key가 들어올때까지 대기, sleep 상태에서는 뮤택스 unlocked
    if(CBs.empty()) cv.wait(lk, [&]{return !CBs.empty();});
    struct keyBox CB = CBs.front();
    CBs.pop_front();
    lk.unlock();
    return CB;
}

void Model::printMsg(string msg){ 
  const char* c=msg.c_str(); 
  WINDOW *win0; 
  win0=newwin(3, 70, 21, 0);
  wclear(win0);
  wprintw(win0,c);
  mu.lock();
  wrefresh(win0); 
  mu.unlock();
}

void Model::quitGame(){
    keyBox obj;
    if(isMine) {
        notifyObserversKey('q');
        return;
    }
    if(isServer) notifyObserversKey('q');
    else executeCommand(GameCommand::Accept, obj);
    isGameDone=true;
}
void Model::startGame(){
    keyBox obj;
    TetrisState state;
    if(isMine){
        srand((unsigned int)time(NULL));
        char Key = (char)('0' + rand() % MAX_BLK_TYPES);
        obj.key=Key;
        notifyObserversKey(obj.key);
    }
    else{
        obj=Read();
        notifyObserversKey(obj.key);
    }
    state=board->accept(obj); 
    notifyObserversView(&(board->oCScreen));
}

void Model::updateModel(keyBox obj){
    TetrisState state=board->accept(obj); //Tetris::accept함수 이용해 obj 적용 후 게임 상태 반환받기
    if(obj.iskey == false){
        cout<<"addline\n";
        return;
    }
    if(state!=Finished) notifyObserversKey(obj.key); //반환받은 state가 Finished가 아니면 KeyObserver들에게 key값 전달 
    notifyObserversView(&(board->oCScreen)); //테트리스 상태를 출력하기 위해 OutScreenObserver들에게 출력할 screen 전달

    if((state!=NewBlockDelR)&&(state!=NewBlock)) return;//return state; //state가 Finished / Running 이라면 해당 상태 반환
    //아니면 NewBlockWDel / NewBlockWODel 상태이므로 일단 새 블록 생성
    cout<<state<<endl;
    
    if(isMine){ //만약 사용자의 키입력을 받는 객체라면
        srand((unsigned int)time(NULL));  //랜덤으로 블록 키 값 받아서
        char Key = (char)('0' + rand() % MAX_BLK_TYPES);
        obj.key=Key; //obj의 key값으로 설정
    }
    else{ //아니면(Recv객체를 통해 키입력을 받는 객체라면)
        obj=Read(); //Read함수를 통해 obj 받아오기
    }
    state=board->accept(obj); //Tetris::accept함수 이용해 재설정된 obj 적용 후 게임 상태 반환받기
    cout<<state<<endl;
    
    if(state!=Finished) notifyObserversKey(obj.key); //반환받은 state가 Finished가 아니면 KeyObserver들에게 key값 전달 
    if(state==NewBlockDelR){
        cout<<"updateline\n";
        notifyObserversDel(board->getDelRect()); //getDelRect로 테트리스 객체의 delRect를 얻어와서 delRectObserver에 전달
        state = Running;
    }

    notifyObserversView(&(board->oCScreen)); //마지막으로 다시 한 번 현재 화면 출력하기 위해 OutScreenObserver들에게 출력할 screen 전달
    if(state==Finished) {
        keyBox objj;
        objj.key = 'q';
        executeCommand(GameCommand::Quit, objj);
    }
    return;
    //return state; //현재 상태 반환
}

void Model::acceptResult(){
    TetrisState state;
    keyBox obj;
    if(!isMine&&!isServer){
        while(true){
            obj=Read();
            if(obj.key=='L'){
                printMsg("You Lose!!");
                char key='O';
                result->updateKey(key);
                isGameDone=true;
                break;
            }
            else if(obj.key=='W'){
                printMsg("You Win!!");
                char key='O';
                result->updateKey(key);
                isGameDone=true;
                break;
            }
        }
    }
}
void Model::executeCommand(GameCommand cmd, keyBox obj){
    GameState newState=stateFromInteger(stateTransMatrix[stateFromEnum(gamestate)][cmdFromEnum(cmd)]);
    
    gamestate=newState;
    switch(cmd){
        case GameCommand::Quit:
            quitGame();
            break;
        case GameCommand::Start:
            startGame();
            break;
        case GameCommand::Update:
            updateModel(obj);
            break;
        case GameCommand::Accept:
            acceptResult();
            break;
    }
}


void Model::run(){
    keyBox CB;
    GameCommand cmd;
    gamestate=GameState::Initial;
    cmd=GameCommand::Start;
    executeCommand(cmd, CB);
    //SexecuteCommand(ServerCommand::Start, obj);
    while(!isGameDone){
        CB=Read();
        if(Keypad.find(CB.key)==Keypad.end()) continue;
        CB.key=Keypad[CB.key];

        if(CB.key=='q') {
            cmd=GameCommand::Quit;
            executeCommand(cmd, CB);
            if(isMine) break;
            if(isServer) break;
        }
        else {
            cmd=GameCommand::Update;
            executeCommand(cmd, CB);
            //SexecuteCommand(ServerCommand::Update, obj);
        }     
        if(gamestate==GameState::Finished) break;
    }
    delete board;
    sleep(1);
    notifyObserversView(nullptr);
    if(!isMine) notifyObserversKey(0);
    notifyObserversKey(0);
    if(isServer)
        cout<<name<< "is terminated"<<endl;
}


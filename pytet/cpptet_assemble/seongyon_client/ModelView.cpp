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
    std::unique_lock<std::mutex> lk(m);
    Obj obj(key);
    objs.push(obj);
    cv.notify_all();
    lk.unlock();
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

void Model::updateDel(Matrix delRect){
    std::unique_lock<std::mutex> lk(m);
    Obj obj(delRect);
    objs.push(obj); 
    cv.notify_all();
    lk.unlock();
}
void Model::addObserverDel(Model* model){
    models.push_back(model); 
}
void Model::notifyObserversDel(Matrix delRect){ 
    for(int i=0;i<models.size();i++){
        models[i]->updateDel(delRect);
    }
}

Obj Model::Read(){ 
    std::unique_lock<std::mutex> lk(m);
    while(objs.size()<1){ 
        cv.wait(lk);
    }
    Obj obj=objs.front();
    objs.pop(); 
    lk.unlock();
    return obj; 
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
    Obj obj;
    if(isMine) {
        notifyObserversKey('q');
        return;
    }
    if(isServer) notifyObserversKey('q');
    else executeCommand(GameCommand::Accept, obj);
    isGameDone=true;
}
void Model::startGame(){
    Obj obj;
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

void Model::updateModel(Obj obj){
    TetrisState state=board->accept(obj); //Tetris::accept함수 이용해 obj 적용 후 게임 상태 반환받기 
    if(state!=Finished) notifyObserversKey(obj.key); //반환받은 state가 Finished가 아니면 KeyObserver들에게 key값 전달 
    notifyObserversView(&(board->oCScreen)); //테트리스 상태를 출력하기 위해 OutScreenObserver들에게 출력할 screen 전달

    if((state!=NewBlockWDel)&&(state!=NewBlockWODel)) return;//return state; //state가 Finished / Running 이라면 해당 상태 반환
    //아니면 NewBlockWDel / NewBlockWODel 상태이므로 일단 새 블록 생성
    
    if(isMine){ //만약 사용자의 키입력을 받는 객체라면
        srand((unsigned int)time(NULL));  //랜덤으로 블록 키 값 받아서
        char Key = (char)('0' + rand() % MAX_BLK_TYPES);
        obj.key=Key; //obj의 key값으로 설정
    }
    else{ //아니면(Recv객체를 통해 키입력을 받는 객체라면)
        obj=Read(); //Read함수를 통해 obj 받아오기
    }
    state=board->accept(obj); //Tetris::accept함수 이용해 재설정된 obj 적용 후 게임 상태 반환받기 
    
    if(state!=Finished) notifyObserversKey(obj.key); //반환받은 state가 Finished가 아니면 KeyObserver들에게 key값 전달 
    if(state==NewBlockWDel){ //만약 state가 NewBlockWDel이면
        notifyObserversDel(board->getDelRect()); //getDelRect로 테트리스 객체의 delRect를 얻어와서 delRectObserver에 전달
    }
    notifyObserversView(&(board->oCScreen)); //마지막으로 다시 한 번 현재 화면 출력하기 위해 OutScreenObserver들에게 출력할 screen 전달
    if(state==Finished) {
        Obj objj('q');
        executeCommand(GameCommand::Quit, objj);
    }
    return;
    //return state; //현재 상태 반환
}

void Model::acceptResult(){
    TetrisState state;
    Obj obj;
    if(!isMine&&!isServer){
        while(true){
            obj=Read();
            if(obj.key=='1'){
                printMsg("You Lose!!");
                char key='|';
                result->updateKey(key);
                isGameDone=true;
                break;
            }
            else if(obj.key=='2'){
                printMsg("You Win!!");
                char key='|';
                result->updateKey(key);
                isGameDone=true;
                break;
            }
        }
    }
}
void Model::executeCommand(GameCommand cmd, Obj obj){
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
    Obj obj;
    GameCommand cmd;
    gamestate=GameState::Initial;
    cmd=GameCommand::Start;
    executeCommand(cmd, obj);
    //SexecuteCommand(ServerCommand::Start, obj);
    while(!isGameDone){
        obj=Read();
        if(!obj.key) break;
        if(Keypad.find(obj.key)==Keypad.end()) continue;
        obj.key=Keypad[obj.key];

        if(obj.key=='q') {
            cmd=GameCommand::Quit;
            executeCommand(cmd, obj);
            if(isMine) break;
            if(isServer) break;
        }
        else {
            cmd=GameCommand::Update;
            executeCommand(cmd, obj);
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


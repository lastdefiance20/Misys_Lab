#include "ModelView.h"

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

Obj Model::read(){ 
    std::unique_lock<std::mutex> lk(m);
    while(objs.size()<1){ 
        cv.wait(lk);
    }
    Obj obj=objs.front();
    objs.pop(); 
    lk.unlock();
    return obj; 
}
TetrisState Model::processKey(CTetris* board, Obj obj){ 
    TetrisState state=board->accept(obj);
    if(state==Finished){
        notifyObserversKey('q');
        isGameDone=true;
    }
    else notifyObserversKey(obj.key);
    //notifyObserversKey(obj.key);
    notifyObserversView(&(board->oCScreen)); 
    if((state!=NewBlockWDel)&&(state!=NewBlockWODel)) return state; 
    if(isServer) {
        if(state==Running) cout<<name <<" : Running"<<endl;
        else if(state==NewBlockWDel) cout<<name <<" : NewBlockWDel"<<endl;
        else if(state==NewBlockWODel) cout<<name <<" : NewBlockWODel"<<endl;
        else if(state==Finished) cout<<name <<" : Finished"<<endl;
    }
    if(isMine){
        srand((unsigned int)time(NULL)); 
        char Key = (char)('0' + rand() % MAX_BLK_TYPES);
        obj.key=Key;
        //notifyObserversKey(obj.key);
    }
    else{
        obj=read();
        if(isServer) cout<<name<<" is applied "<<obj.key<<endl;
        //notifyObserversKey(obj.key);
    }
    state=board->accept(obj);
    
    if(state==Finished) {
        notifyObserversKey('q');
        isGameDone=true;
    }
    else notifyObserversKey(obj.key); 
    if(state==NewBlockWDel){ 
        notifyObserversDel(board->getDelRect());
        //state=Running;
    }
    notifyObserversView(&(board->oCScreen)); 
    return state; 
}
void Model::run(){
    CTetris *board=new CTetris(20,15); 
    TetrisState state;
    Obj obj;

    if(isMine){
        srand((unsigned int)time(NULL));
        char Key = (char)('0' + rand() % MAX_BLK_TYPES);
        obj.key=Key;
        notifyObserversKey(obj.key);
    }
    else{
        obj=read();
        notifyObserversKey(obj.key);
    }
    state=board->accept(obj); 
    notifyObserversView(&(board->oCScreen));
    
    while(!isGameDone){ 
        obj=read();
        if(!obj.key) break;
        if(Keypad.find(obj.key)==Keypad.end()) continue;
        obj.key=Keypad[obj.key];
        if(obj.key=='q') {
            notifyObserversKey(obj.key);
            state=Finished;
            isGameDone=true;
        }
        else state=processKey(board, obj);
        /*if(state==Finished){
            notifyObserversKey('q');
            isGameDone=true;
        }*/
    }
    delete board;
    sleep(1);
    notifyObserversView(nullptr);
    notifyObserversKey(0);
    if(isServer)
        cout<<name<< "is terminated"<<endl;
}
#include <Model.h>
#include <View.h>

void Model::addObserver(View* observer){
    observers[nobservers] = observer;
    nobservers++;
};

void Model::notifyObservers(CTetris* board){
    for(int i=0; i<nobservers; i++){
        observers[i]->update(board);
    }
};

Model::Model(string name){
    pname = name;
};

//key를 update할 경우
void Model::update(Matrix key){
    m.lock();
    MatrixKeys.push_back(key);
    cv.notify_all();
    m.unlock();
};

Matrix Model::read(){
    unique_lock<mutex> lk(m);
    //keys에 key가 들어올때까지 대기, sleep 상태에서는 뮤택스 unlocked
    if(MatrixKeys.empty()) cv.wait(lk, [&]{return !MatrixKeys.empty();});
    Matrix key = MatrixKeys.front();
    MatrixKeys.pop_front();
    lk.unlock();
    return key;
}

Matrix Model::makeScreen(CTetris *board){
    int dy = board->oCScreen.get_dy();
    int dx = board->oCScreen.get_dx();
    int dw = board->iScreenDw;
    int **array = board->oCScreen.get_array();
    Matrix Screen = Matrix(array);
    Matrix PScreen = Screen.clip(0, dw, dy-dw, dx-dw);
    return PScreen;
}

void Model::run(){
    CTetris *board = new CTetris(20, 10); //change
    TetrisState state;
    Matrix key;
    key = read();
    state = board->accept(key);
    notifyObservers(makeScreen(board));
    
    while(isGameDone == false) {
        key = read();
        state = (board->accept(key)).sum();
        if (state == NewBlock) {
            key = read();
            state = (board->accept(key)).sum();
            if (state == Finished) {
                notifyObservers(board);
                isGameDone = true;
                break;
            }
        }
        notifyObservers(makeScreen(board));
        if(char(key.sum()) == 'q'){
            isGameDone = true;
            break;
        }
    }
    notifyObservers(makeScreen(board));
    delete board;
};
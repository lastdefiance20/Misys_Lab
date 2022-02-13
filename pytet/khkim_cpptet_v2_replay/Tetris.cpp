#include "Tetris.h"
#include <cmath>

//정의 해주기~
int Tetris::iScreenDw=0;
Matrix* Tetris::setOfBlockObjects;
int Tetris::nBlockTypes=0;
int Tetris::nBlockDegrees=0;
int Tetris::num_allocated_objects=0;

void Tetris::init(Array2D setOfBlockArrays[], int MAX_BLK_TYPES, int MAX_BLK_DEGREES){
    nBlockTypes = MAX_BLK_TYPES;
    nBlockDegrees = MAX_BLK_DEGREES;

    setOfBlockObjects = new Matrix[nBlockTypes*nBlockDegrees];

    int maxnum = 0;
    
    for(int i = 0; i < nBlockTypes; i++){
        for(int j = 0; j < nBlockDegrees; j++){
            int row = setOfBlockArrays[i*nBlockDegrees+j].size();
            if(row>maxnum)
                maxnum = row;
            int col = setOfBlockArrays[i*nBlockDegrees+j][0].size();
            if(col>maxnum)
                maxnum = col;
            setOfBlockObjects[i*nBlockDegrees+j] = new Matrix(setOfBlockArrays[i*nBlockDegrees+j], col, row);
        }
    }

    int arrayBlk_maxsize = maxnum;

    iScreenDw = arrayBlk_maxsize;
};

Matrix Tetris::createArrayscreen(){
    arrayScreenDx = iScreenDw*2 + iScreenDx;
    arrayScreenDy = iScreenDy + iScreenDw;
    //cout<<arrayScreenDx<<"/"<<arrayScreenDy<<"/"<<iScreenDw<<"/"<<iScreenDy<<"/"<<iScreenDx<<endl;
    int *arrScreen = new int[(arrayScreenDx)*(arrayScreenDy)];

    for(int y = 0; y < iScreenDy; y++){
        for(int x = 0; x < iScreenDw; x++){
            arrScreen[y*arrayScreenDx+x] = 1;
        }
        for(int x = 0; x < iScreenDx; x++){
            arrScreen[y*arrayScreenDx+iScreenDw + x] = 0;
        }
        for(int x = 0; x < iScreenDw; x++){
            arrScreen[y*arrayScreenDx+iScreenDw + iScreenDx + x] = 1;
        }
    }

    for(int y = 0; y < iScreenDw; y++){
        for(int x = 0; x < arrayScreenDx; x++){
            arrScreen[iScreenDy*arrayScreenDx+y*arrayScreenDx+x] = 1;
        }
    }

    arrayScreen = Matrix(arrScreen, arrayScreenDy ,arrayScreenDx);
    //cout<<"ArrayScreen"<<endl;
    //arrayScreen.print();
    return arrayScreen;
};

Tetris::Tetris(int Dy, int Dx){
    iScreenDy = Dy;
    iScreenDx = Dx;
    idxBlockDegree = 0;
    arrayScreen = createArrayscreen();
    iScreen = Matrix(arrayScreen);
    oScreen = Matrix(iScreen);
    num_allocated_objects += 1;
    state = TetrisState(NewBlock);
};

Msg Tetris::accept(Msg *msg){
    Matrix tempBlk;

    if(msg->key==false){
        Matrix delRect = msg->mat;

        addDeleteLines(delRect);
        iScreen = Matrix(oScreen);

        if(checkBlock(delRect) == false){
            state = TetrisState(Finished);
        }
        return Msg(MSG_KEY, state, NULL);
    }

    char key = msg->key;

    int keynum = key - '0';

    if(state == NewBlock){
        if ((keynum >= 0)&&(keynum <= 6)){
            deleteFullLines();
            //블럭을 새로 꺼낼때 stack에 저장된 지워진 줄을 추가한다
            iScreen = Matrix(oScreen);
            idxBlockType = keynum;
            //cout<<idxBlockType<<endl;
            idxBlockDegree = 0;
            currBlk = setOfBlockObjects[idxBlockType*nBlockDegrees + idxBlockDegree];
            top = 0;
            left = iScreenDw + (int)iScreenDx/2 - (int)currBlk.get_dx()/2;
            tempBlk = iScreen.clip(top, left, top+currBlk.get_dy(), left+currBlk.get_dx());
            tempBlk = tempBlk.add(&currBlk);
            cout<<endl;

            oScreen = Matrix(iScreen);
            oScreen.paste(&tempBlk, top, left);
            //지워진 줄이 있으면(observer에게 notify시켜야함)
            if(checkdel>0){
                //state = TetrisState(NewBlockDelR);
                state = TetrisState(Running);
                checkdel = 0;
            }
            else{
                state = TetrisState(Running);
            }
            if (tempBlk.anyGreaterThan(1)) state = TetrisState(Finished);

            return Msg(MSG_KEY, state, NULL);
        }
        else
            return Msg(MSG_KEY, state, NULL);
    }
    else if (key == 'q') state = TetrisState(Finished);
    else if (key == 'a') left -= 1;
    else if (key == 'd') left += 1;
    else if (key == 's') top += 1;
    else if (key == 'w'){
        idxBlockDegree = (idxBlockDegree + 1) % nBlockDegrees;
        currBlk = setOfBlockObjects[idxBlockType*nBlockDegrees + idxBlockDegree];
    }
    else if (key == ' '){
        while (1){
            if (tempBlk.anyGreaterThan(1) != false)
                break;
            top += 1;
            tempBlk = iScreen.clip(top, left, top+currBlk.get_dy(), left+currBlk.get_dx());
            tempBlk = tempBlk.add(&currBlk);
        }
    }
    else cout<<"Wrong key!!!"<<endl;

    tempBlk = iScreen.clip(top, left, top+currBlk.get_dy(), left+currBlk.get_dx());
    tempBlk = tempBlk.add(&currBlk);

    if (tempBlk.anyGreaterThan(1)){
        if (key == 'a') left += 1;
        else if (key == 'd') left -= 1;
        else if (key == 's'){
            top -= 1;
            state = TetrisState(NewBlock);
        }
        else if (key == 'w'){
            idxBlockDegree = (idxBlockDegree - 1) % nBlockDegrees;
            currBlk = setOfBlockObjects[idxBlockType*nBlockDegrees + idxBlockDegree];
        }
        else if (key == ' '){
            top -= 1;
            state = TetrisState(NewBlock);
        }

        tempBlk = iScreen.clip(top, left, top+currBlk.get_dy(), left+currBlk.get_dx());
        tempBlk = tempBlk.add(&currBlk);
    }

    oScreen = Matrix(iScreen);
    oScreen.paste(&tempBlk, top, left);
    //지워진 줄이 있으면(observer에게 notify시켜야함)
    if(checkdel>0){
        state = TetrisState(NewBlockDelR);
        checkdel = 0;
    }
    return Msg(MSG_KEY, state, NULL);
};

int Tetris::deleteFullLines(){
    for(int y = 1; y<=iScreenDy-noDeleteLine; y++){
        Matrix tempScreen = oScreen.clip(0, 0, y-1, iScreenDw*2+iScreenDx);
        Matrix line = oScreen.clip(y-1, 0, y, iScreenDw*2+iScreenDx);
        
        if(line.sum()==(iScreenDw*2+iScreenDx)){
            //tempScreen.print();
            oScreen.paste(&tempScreen,1,0);
            //oScreen.print();
        }
    }
};

//지울 line이 있는지 확인한다. 찾으면 temp에 저장하고 true를 반환하며 아니면 false를 반환한다.
int Tetris::checkDeleteLines(){
    int checked = 0;
    int rtchecked = 0;
    for(int y = 1; y<=iScreenDy-noDeleteLine; y++){
        Matrix line = oScreen.clip(y-1, 0, y, iScreenDw*2+iScreenDx);
        
        if(line.sum()==(iScreenDw*2+iScreenDx)){
            checked++;
        }
    }

    rtchecked = checked;

    if(checked>0){
        tempDeleteLine = oScreen.clip(0, 0, checked, iScreenDw*2+iScreenDx);
        for(int y = 1; y<=iScreenDy-noDeleteLine; y++){
            Matrix line = oScreen.clip(y-1, 0, y, iScreenDw*2+iScreenDx);
        
            if(line.sum()==(iScreenDw*2+iScreenDx)){
                checked--;
                tempDeleteLine.paste(&line, checked, 0);
            }
        }
    }
    return rtchecked;
}

//delRect를 받아 board 밑에 추가가 가능하다면 추가한 후 true를 반환하며, 불가능하다면 false를 반환한다.
void Tetris::addDeleteLines(Matrix delRect){
    int delRectdy = delRect.get_dy();
    int delRectdx = delRect.get_dx();
    int checked = 0;

    oScreen = Matrix(iScreen);

    int y = iScreenDy;
    Matrix delRectNormal(delRectdy, delRectdx, 1);

    Matrix tempScreen = oScreen.clip(delRectdy, 0, y, iScreenDw*2+iScreenDx);
    
    oScreen.paste(&tempScreen,0,0);
    oScreen.paste(&delRectNormal,y-delRectdy,0);
    iScreen = Matrix(oScreen);
}

bool Tetris::checkBlock(Matrix delRect){
    Matrix tempBlk;
    int delRectdy = delRect.get_dy();

    tempBlk = iScreen.clip(top, left, top+currBlk.get_dy(), left+currBlk.get_dx());
    tempBlk = tempBlk.add(&currBlk);

    if(tempBlk.anyGreaterThan(1) == true){
        for(int y = 1; y<=delRectdy; y++){
            if(top != 0){
                top--;
            }
            else{
                break;
            }
        }

        tempBlk = iScreen.clip(top, left, top+currBlk.get_dy(), left+currBlk.get_dx());
        tempBlk = tempBlk.add(&currBlk);
    
        if(tempBlk.anyGreaterThan(1) == true){
            return false;
        }

        while (1){
            if (tempBlk.anyGreaterThan(1) != false)
                break;
            top += 1;
            tempBlk = iScreen.clip(top, left, top+currBlk.get_dy(), left+currBlk.get_dx());
            tempBlk = tempBlk.add(&currBlk);
        }

        top--;
    }

    return true;
}

Tetris::~Tetris(){
    num_allocated_objects -= 1;
    if(num_allocated_objects == 0){
        delete [] setOfBlockObjects;
        //cout<<"deleted Tetris"<<endl;
    }
};

#include "CTetris.h"

Matrix* CTetris::setOfCBlockObjects;

//use for delete setOfCBlockObjects one time
int CTetris::num_allocated_Cobjects=0;

//initialize tetris block
void CTetris::init(Array2D setOfBlockArrays[], int MAX_BLK_TYPES, int MAX_BLK_DEGREES){
    Tetris::init(setOfBlockArrays, MAX_BLK_TYPES, MAX_BLK_DEGREES);
    setOfCBlockObjects = new Matrix[nBlockTypes*nBlockDegrees];

    for(int i = 0; i < nBlockTypes; i++){
        for(int j = 0; j < nBlockDegrees; j++){
            setOfCBlockObjects[i*nBlockDegrees+j] = setOfBlockObjects[i*nBlockDegrees+j];
            setOfCBlockObjects[i*nBlockDegrees+j].mulc(i+1);
        }
    }
};

CTetris::CTetris(int Dy, int Dx):Tetris(Dy, Dx){
    arrayScreen = createArrayscreen();
    iCScreen = Matrix(arrayScreen);
    oCScreen = Matrix(iCScreen);
    num_allocated_Cobjects += 1;
};

Msg CTetris::accept(Msg *msg){
    if(msg->what == MSG_MAT){
        //지워진 line이 update되어 accept 되었을경우 stack에 추가한다.
        Matrix delRect = msg->mat;
        //delRect 추가
        addDeleteLines(delRect);
        iCScreen = Matrix(oCScreen);
        msg->key = 'v';
    }

    char key = msg->key;
    Matrix tempBlk;
    int keynum = key - '0';

    if ((keynum >= 0)&&(keynum <= 6)){
        checkdel = checkDeleteLines();
        deleteFullLines();
        iCScreen = Matrix(oCScreen);
        //블럭을 새로 꺼낼때 stack에 저장된 지워진 줄을 추가한다
    }

    Msg stateMsg = Tetris::accept(msg);

    currCBlk = setOfCBlockObjects[idxBlockType*nBlockDegrees + idxBlockDegree];

    tempBlk = iCScreen.clip(top, left, top+currCBlk.get_dy(), left+currCBlk.get_dx());
    tempBlk = tempBlk.add(&currCBlk);

    oCScreen = Matrix(iCScreen);
    oCScreen.paste(&tempBlk, top, left);

    if (TetrisState(stateMsg.key) == NewBlockDelR)//add function for get delrect faster
        checkdel = checkDeleteLines();

    return stateMsg;
};

void CTetris::deleteFullLines(){
    for(int y = 1; y<=iScreenDy-noDeleteLine; y++){
        Matrix tempScreen = oScreen.clip(0, 0, y-1, iScreenDw*2+iScreenDx);
        Matrix line = oScreen.clip(y-1, 0, y, iScreenDw*2+iScreenDx);
        
        if(line.sum()==(iScreenDw*2+iScreenDx)){
            Matrix CtempScreen = oCScreen.clip(0, 0, y-1, iScreenDw*2+iScreenDx);
            oScreen.paste(&tempScreen,1,0);
            oCScreen.paste(&CtempScreen,1,0);
        }
    }
};

//지울 line이 있는지 확인한다. 찾으면 temp에 저장한다.
int CTetris::checkDeleteLines(){
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
        tempCDeleteLine = oScreen.clip(0, 0, checked, iScreenDw*2+iScreenDx);
        for(int y = 1; y<=iScreenDy-noDeleteLine; y++){
            Matrix line = oScreen.clip(y-1, 0, y, iScreenDw*2+iScreenDx);
        
            if(line.sum()==(iScreenDw*2+iScreenDx)){
                checked--;
                Matrix Cline = oCScreen.clip(y-1, 0, y, iScreenDw*2+iScreenDx);
                tempDeleteLine.paste(&line, checked, 0);
                tempCDeleteLine.paste(&Cline, checked, 0);
            }
        }
    }

    return rtchecked;
}

//delRect를 받아 board 추가한다.
void CTetris::addDeleteLines(Matrix delRect){
    oCScreen = Matrix(iCScreen);

    int delRectdy = delRect.get_dy();
    int delRectdx = delRect.get_dx();
    noDeleteLine += delRectdy;

    int y = iScreenDy;
    Matrix delRectNormal(delRectdy, delRectdx, 1);

    Matrix CtempScreen = oCScreen.clip(delRectdy, 0, y, iScreenDw*2+iScreenDx);

    oCScreen.paste(&CtempScreen,0,0);
    oCScreen.paste(&delRect,y-delRectdy,0);

    iCScreen = Matrix(oCScreen);
}


//line을 원하면 temp에 저장된 line을 반환해준다
Matrix CTetris::getData(TetrisState state){
    if(state == NewBlockDelR){
        return tempCDeleteLine;
    }
    else{
        Matrix matScreen = oCScreen.clip(0, iScreenDw, iScreenDy, iScreenDw+iScreenDx);
        return matScreen;
    }
}

//delete setOfCBlockObjects
CTetris::~CTetris(){
    num_allocated_Cobjects -= 1;
    if(num_allocated_Cobjects == 0){
        delete [] setOfCBlockObjects;
    }
};
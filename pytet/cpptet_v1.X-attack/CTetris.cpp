#include "CTetris.h"

Matrix* CTetris::setOfCBlockObjects;

void CTetris::init(int **setOfBlockArrays, int MAX_BLK_TYPES, int MAX_BLK_DEGREES){
    Tetris::init(setOfBlockArrays, MAX_BLK_TYPES, MAX_BLK_DEGREES);
    setOfCBlockObjects = new Matrix[nBlockTypes*nBlockDegrees];

    for(int i = 0; i < nBlockTypes; i++){
        for(int j = 0; j < nBlockDegrees; j++){
            setOfCBlockObjects[i*nBlockDegrees+j] = setOfBlockObjects[i*nBlockDegrees+j];
            setOfCBlockObjects[i*nBlockDegrees+j].mulc(i+1);
        }
    }
};

void CTetris::deletestatic(){
    delete [] setOfCBlockObjects;
    //cout<<"deleted Tetris C"<<endl;
    Tetris::deletestatic();
};

CTetris::CTetris(int iScreenDyy, int iScreenDxx):Tetris(iScreenDyy, iScreenDxx){
    arrayScreen = createArrayscreen();
    iCScreen = Matrix(arrayScreen);
    oCScreen = Matrix(iCScreen);
};

TetrisState CTetris::accept(char key){
    Matrix tempBlk;
    int keynum = key - '0';

    if ((keynum >= 0)&&(keynum <= 6)){
        if (justStarted == false) {
            checkdel = checkDeleteLines();
            deleteFullLines();
        }
        iCScreen = Matrix(oCScreen);
        //블럭을 새로 꺼낼때 stack에 저장된 지워진 줄을 추가한다
        while(!delRects.empty()){
            Matrix delRect = delRects.top();
            delRects.pop();
            bool notcrash = addDeleteLines(delRect);
        }
        iCScreen = Matrix(oCScreen);
    }

    state = Tetris::accept(key);

    currCBlk = setOfCBlockObjects[idxBlockType*nBlockDegrees + idxBlockDegree];

    tempBlk = iCScreen.clip(top, left, top+currCBlk.get_dy(), left+currCBlk.get_dx());
    tempBlk = tempBlk.add(&currCBlk);

    oCScreen = Matrix(iCScreen);
    oCScreen.paste(&tempBlk, top, left);

    return state;
};

//지워진 line이 update되어 accept 되었을경우 stack에 추가한다.
TetrisState CTetris::accept(Matrix deletedLines){
    delRects.push(deletedLines);
}

void CTetris::deleteFullLines(){
    //iscreenDy = screen y num
    //iscreenDx = screen x num
    //iscreenDw = padding
    /*
    Dw   /   Dx    / Dw
    1 1 1 0 0 0 0 0 1 1 1   /Dy
    */

    //oScreen.print()
    //oCScreen.print()
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

//지울 line이 있는지 확인한다. 찾으면 temp에 저장하고 true를 반환하며 아니면 false를 반환한다.
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

//line을 원하면 temp에 저장된 line을 반환해준다
Matrix CTetris::getDelRect(){
    return tempCDeleteLine;
}

//delRect를 받아 board 밑에 추가가 가능하다면 추가한 후 true를 반환하며, 불가능하다면 false를 반환한다.
bool CTetris::addDeleteLines(Matrix delRect){
    int delRectdy = delRect.get_dy();
    int delRectdx = delRect.get_dx();
    noDeleteLine += delRectdy;
    int checked = 0;
    for(int y = 1; y<=delRectdy; y++){
        Matrix line = oScreen.clip(y-1, 0, y, iScreenDw*2+iScreenDx);
        
        if(line.sum()!=(iScreenDw*2)){
            checked++;
        }
    }

    if(checked>0) return false;
    else{
        int y = iScreenDy;
        Matrix delRectNormal(delRectdy, delRectdx, 1);

        Matrix tempScreen = oScreen.clip(delRectdy, 0, y, iScreenDw*2+iScreenDx);
        Matrix CtempScreen = oCScreen.clip(delRectdy, 0, y, iScreenDw*2+iScreenDx);
        
        oScreen.paste(&tempScreen,0,0);
        oScreen.paste(&delRectNormal,y-delRectdy,0);
        oCScreen.paste(&CtempScreen,0,0);
        oCScreen.paste(&delRect,y-delRectdy,0);
        return true;
    }
}

CTetris::~CTetris(){
};
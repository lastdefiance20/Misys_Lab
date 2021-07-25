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

CTetris::CTetris(int iScreenDyy, int iScreenDxx):Tetris(iScreenDyy, iScreenDxx){
    arrayScreen = createArrayscreen();
    iCScreen = Matrix(arrayScreen);
    oCScreen = Matrix(iCScreen);
};

void CTetris::deleteonce(){
    delete [] setOfCBlockObjects;
}

CTetris::~CTetris(){
    delete [] setOfCBlockObjects;
}

TetrisState CTetris::accept(char key){
    Matrix tempBlk;
    int keynum = key - '0';

    if ((keynum >= 0)&&(keynum <= 6)){
        if (justStarted == false) deleteFullLines();
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
    for(int y = 1; y<=iScreenDy; y++){
        Matrix tempScreen = oScreen.clip(0, 0, y-1, iScreenDw*2+iScreenDx);
        Matrix line = oScreen.clip(y-1, 0, y, iScreenDw*2+iScreenDx);
        
        if(line.sum()==(iScreenDw*2+iScreenDx)){
            Matrix CtempScreen = oCScreen.clip(0, 0, y-1, iScreenDw*2+iScreenDx);
            oScreen.paste(&tempScreen,1,0);
            oCScreen.paste(&CtempScreen,1,0);
        }
    }
};
#include "Tetris.h"
#include <cmath>

//정의 해주기~
int Tetris::iScreenDw=0;
Matrix* Tetris::setOfBlockObjects;
int Tetris::nBlockTypes=0;
int Tetris::nBlockDegrees=0;

void Tetris::init(int **setOfBlockArrays, int MAX_BLK_TYPES, int MAX_BLK_DEGREES){
    nBlockTypes = MAX_BLK_TYPES;
    nBlockDegrees = MAX_BLK_DEGREES;

    //Tetris.setOfBlockObjects = [[0] * Tetris.nBlockDegrees for _ in range(Tetris.nBlockTypes)]

    try{
        setOfBlockObjects = new Matrix[nBlockTypes*nBlockDegrees]; //에러구간 1
        //Matrix *setOfBlockObjects[28];
        }
    catch(std::bad_alloc &b) {

        puts("에러 발생 111");

        b.what();
    }

    /*
    mat = new int*[r];
	for (int i = 0; i < r; i++)
		mat[i] = new int[c];
    [출처] [c++]동적 할당, 2차원 배열 동적 할당. new, delete|작성자 빵집청년
    */

    /*
    for(int i = 0; i < nBlockTypes; i++){
        if(arrayBlk_maxsize <= len(setOfBlockArrays[i][0])){
            arrayBlk_maxsize = len(setOfBlockArrays[i][0]);
        }
    }
    */

    int maxnum = 4;
    
    for(int i = 0; i < nBlockTypes; i++){
        for(int j = 0; j < nBlockDegrees; j++){
            int n = 0;
            while(1){
                n += 1;
                if(setOfBlockArrays[i*nBlockDegrees+j][n] == -1) break;
                //cout<<setOfBlockArrays[i*nBlockDegrees+j][n]<<endl;
            }
            //cout<<n<<endl;
            /*
            n = sqrt(n);
            if(n > maxnum) maxnum = n;
            cout<<n<<maxnum<<endl;
            */

            if(n == 16) n = 4;
            else if(n == 4) n = 2;
            else n = 3;
            //cout<<n<<endl;
            setOfBlockObjects[i*nBlockDegrees+j] = new Matrix(setOfBlockArrays[i*nBlockDegrees+j], n, n);
        }
    }

    int arrayBlk_maxsize = maxnum;

    iScreenDw = arrayBlk_maxsize;
};

Matrix Tetris::createArrayscreen(){
    arrayScreenDx = iScreenDw*2 + iScreenDx;
    arrayScreenDy = iScreenDy + iScreenDw;
    //cout<<arrayScreenDx<<"/"<<arrayScreenDy<<"/"<<iScreenDw<<"/"<<iScreenDy<<"/"<<iScreenDx<<endl;
    //arrayScreen = [[0] * self.arrayScreenDx for _ in range(self.arrayScreenDy)]
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
    arrayScreen.print();
    return arrayScreen;
};

Tetris::Tetris(int iScreenDyy, int iScreenDxx){
    iScreenDy = iScreenDyy;
    iScreenDx = iScreenDxx;
    idxBlockDegree = 0;
    arrayScreen = createArrayscreen();
    iScreen = Matrix(arrayScreen);
    oScreen = Matrix(iScreen);
    justStarted = true;
};

TetrisState Tetris::accept(char key){
    state = TetrisState(Running);
    Matrix tempBlk;

    int keynum = key - '0';

    if ((keynum >= 0)&&(keynum <= 6)){
        if (justStarted == false) deleteFullLines();
        iScreen = Matrix(oScreen);
        idxBlockType = keynum;
        //cout<<idxBlockType<<endl;
        idxBlockDegree = 0;
        currBlk = setOfBlockObjects[idxBlockType*nBlockDegrees + idxBlockDegree];
        top = 0;
        left = iScreenDw + (int)iScreenDx/2 - (int)currBlk.get_dx()/2;
        tempBlk = iScreen.clip(top, left, top+currBlk.get_dy(), left+currBlk.get_dx());
        tempBlk = tempBlk.add(&currBlk);
        justStarted = false;
        cout<<endl;

        if (tempBlk.anyGreaterThan(1)) state = TetrisState(Finished);
        oScreen = Matrix(iScreen);
        oScreen.paste(&tempBlk, top, left);
        return state;
    }
    else if (key == 'q') cout<<"q"<<endl;
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

    return state;
};

int Tetris::deleteFullLines(){
    for(int y = 1; y<=iScreenDy; y++){
        Matrix tempScreen = oScreen.clip(0, 0, y-1, iScreenDw*2+iScreenDx);
        Matrix line = oScreen.clip(y-1, 0, y, iScreenDw*2+iScreenDx);
        
        if(line.sum()==(iScreenDw*2+iScreenDx)){
            tempScreen.print();
            oScreen.paste(&tempScreen,1,0);
            oScreen.print();
        }
    }
};

Tetris::~Tetris(){
    delete [] setOfBlockObjects;
};
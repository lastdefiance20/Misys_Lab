#pragma once
#include "Matrix.h"
#include <stack>

extern "C"{
enum TetrisState{Running, NewBlockDelR, NewBlock, Finished};
//end of enum TetrisSate()

//undefined reference to 에러 (static 변수를 사용시)
//h 라이브러리를 사용하기 때문에 static 변수가 잘못 정의됨?
//https://5kyc1ad.tistory.com/343

//static 지워버렸는데 잘?
class Tetris {
public:
    static int nBlockTypes;
    static int nBlockDegrees;
    static Matrix *setOfBlockObjects;
    static int iScreenDw;
    int iScreenDy;
    int iScreenDx;
    int idxBlockDegree;
    int idxBlockType;
    TetrisState state;
    int top;
    int left;
    Matrix currBlk;
    Matrix iScreen;
    Matrix oScreen;
    bool justStarted;
    int arrayScreenDx;
    int arrayScreenDy;
    Matrix arrayScreen;
    int noDeleteLine = 0;
    int checkdel = 0;
    stack<Matrix> delRects;
    Matrix tempDeleteLine;

    static void init(int **setOfBlockArrays, int MAX_BLK_TYPES, int MAX_BLK_DEGREES);
    static void deletestatic();
    Matrix createArrayscreen();
    Tetris(int iScreenDyy, int iScreenDxx);
    TetrisState accept(char key);
    TetrisState accept(Matrix deletedLines);
    int deleteFullLines();
    int checkDeleteLines();
    bool addDeleteLines(Matrix delRect);
    ~Tetris();
};
}

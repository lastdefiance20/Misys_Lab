#pragma once
#include "Matrix.h"
#include "MsgPubSub.h"
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
    static int num_allocated_objects;

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
    int arrayScreenDx;
    int arrayScreenDy;
    Matrix arrayScreen;
    int noDeleteLine = 0;
    int checkdel = 0;
    stack<Matrix> delRects;
    Matrix tempDeleteLine;

    static void init(Array2D setOfBlockArrays[], int MAX_BLK_TYPES, int MAX_BLK_DEGREES);
    Matrix createArrayscreen();
    Tetris(int iScreenDyy, int iScreenDxx);
    Msg accept(Msg *msg);
    int deleteFullLines();
    int checkDeleteLines();
    void addDeleteLines(Matrix delRect);
    bool checkBlock(Matrix delRect);
    ~Tetris();
};
}

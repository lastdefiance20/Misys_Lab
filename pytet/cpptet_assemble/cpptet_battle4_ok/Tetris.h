#pragma once
#include "Matrix.h"
#include <queue>

enum TetrisState { Running, NewBlockWDel, NewBlockWODel, Finished };
struct Obj{
    char key=-1;
    Matrix delRect;
    Obj(char Key){
        key=Key;
    }
    Obj(Matrix DelRect){
        delRect=DelRect;
    }
    Obj(){}
};
class Tetris {
    protected:
        static int nBlockTypes;
        static int nBlockDegrees;
        static Matrix** setOfBlockObjects;
        Matrix iScreen;
        int iScreenDx;
        int iScreenDy;
        int* arrayScreen;
        bool justStarted;
        int idxBlockDegree;
        int idxBlockType;
        TetrisState state;
        int top;
        int left;
        Matrix currBlk;
    public:
        Matrix oScreen;
        static int iScreenDw;
        Tetris();
        Tetris(int Dy, int Dx);
        ~Tetris();
        static void init(int *setOfBlockArrays[], int MAX_BLK_TYPES, int MAX_BLK_DEGREES);
        void createArrayScreen(int* arrayScreen);
        Matrix getDelRect();
        TetrisState accept(Obj obj);
        void deleteFullLines();
        std::queue<Matrix> delLines;
        int nodelete=0;
        static int num;
};

/*
int Tetris::iScreenDw=0;
Matrix** Tetris::setOfBlockObjects;
int Tetris::nBlockTypes=0;
int Tetris::nBlockDegrees=0;
*/

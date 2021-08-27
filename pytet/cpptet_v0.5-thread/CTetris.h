#pragma once
#include "Tetris.h"
#include "Matrix.h"

extern "C"{
class CTetris : public Tetris {
public:
    static Matrix *setOfCBlockObjects;
    Matrix iCScreen;
    Matrix oCScreen;
    Matrix currCBlk;

    static void init(int **setOfBlockArrays, int MAX_BLK_TYPES, int MAX_BLK_DEGREES);
    CTetris(int iScreenDyy, int iScreenDxx);
    TetrisState accept(char key);
    void deleteFullLines();
    ~CTetris();
};
}

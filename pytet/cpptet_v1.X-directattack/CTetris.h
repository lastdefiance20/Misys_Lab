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
    Matrix tempCDeleteLine;

    static void init(int **setOfBlockArrays, int MAX_BLK_TYPES, int MAX_BLK_DEGREES);
    static void deletestatic();
    CTetris(int iScreenDyy, int iScreenDxx);
    TetrisState accept(char key);
    TetrisState accept(Matrix deletedLines);
    void deleteFullLines();
    int checkDeleteLines();
    Matrix getDelRect();
    bool addDeleteLines(Matrix delRect);
    ~CTetris();
};
}

#pragma once
#include "Tetris.h"
#include "Matrix.h"

extern "C"{
class CTetris : public Tetris {
public:
    static Matrix *setOfCBlockObjects;
    static int num_allocated_Cobjects;

    Matrix iCScreen;
    Matrix oCScreen;
    Matrix currCBlk;
    Matrix tempCDeleteLine;

    static void init(int **setOfBlockArrays, int MAX_BLK_TYPES, int MAX_BLK_DEGREES);
    CTetris(int Dy, int Dx);
    Msg accept(Msg *msg);
    void deleteFullLines();
    int checkDeleteLines();
    Matrix getDelRect();
    void addDeleteLines(Matrix delRect);
    Matrix getScreen();
    ~CTetris();
};
}

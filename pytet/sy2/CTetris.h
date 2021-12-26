#pragma once
#include "Tetris.h"

class CTetris : public Tetris {
    public:
        static Matrix** setOfCBlockObjects;
        Matrix iCScreen;
        Matrix oCScreen; 
        Matrix currCBlk;
        std::queue<Matrix> delLines;
        CTetris();
        CTetris(int Dy, int Dx);
        ~CTetris();
        static void init(int *setOfBlockArrays[], int MAX_BLK_TYPES, int MAX_BLK_DEGREES);
        Matrix getDelRect();
        TetrisState accept(Obj obj);
        void deleteFullLines();
        //static int num;
        //int nodelete=0;
};
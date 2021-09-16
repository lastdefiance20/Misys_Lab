#include "CTetris.h"
#include<math.h>
Matrix** CTetris::setOfCBlockObjects;
CTetris::CTetris():Tetris(){
    iCScreen=Matrix(iScreen);
    oCScreen=Matrix(iCScreen);
    Tetris::num++;
}
CTetris::CTetris(int Dy, int Dx):Tetris(Dy,Dx){
    iCScreen=Matrix(iScreen);
    oCScreen=Matrix(iCScreen);
    Tetris::num++;
}
CTetris::~CTetris(){
    Tetris::num--;
    if(Tetris::num==0){
        for(int i=0;i<nBlockTypes;i++)
            delete [] setOfCBlockObjects[i];
        delete [] setOfCBlockObjects;
    }
}
void CTetris::init(int *setOfBlockArrays[], int MAX_BLK_TYPES, int MAX_BLK_DEGREES){
    Tetris::init(setOfBlockArrays,MAX_BLK_TYPES,MAX_BLK_DEGREES);
    nBlockDegrees=MAX_BLK_DEGREES;
    nBlockTypes=MAX_BLK_TYPES;
    setOfCBlockObjects=new Matrix*[MAX_BLK_TYPES];
    for(int i=0;i<MAX_BLK_TYPES;i++)
    {
        setOfCBlockObjects[i]=new Matrix[MAX_BLK_DEGREES];
        for(int j=0;j<MAX_BLK_DEGREES;j++)
        {
            int len=0;
            int where=0;
            for(int k=0;k!=-1;k=setOfBlockArrays[i*MAX_BLK_DEGREES+j][where++]){
                len++;
                if(k==1) setOfBlockArrays[i*MAX_BLK_DEGREES+j][where-1]=i+1;
            }
            len--;
            len=sqrt(len);
            if(iScreenDw<=len) iScreenDw=len;
            setOfCBlockObjects[i][j]=Matrix(setOfBlockArrays[i*MAX_BLK_DEGREES+j], len, len);
        }
    }
}
Matrix CTetris::getDelRect(){
    int size=delLines.size(); //테트리스 객체의 delLines 길이를 size로 저장
    Matrix delRect(size,15); //y축 길이가 size이고 x축은 full size인 delRect 생성 (이 delRect는 값이 모두 0으로 되어있는 기본 Matrix형태)
    for(int i=0;i<size;i++){
        delRect.paste(&(delLines.front()),i,0); //delLines에서 하나씩 가져와서 delRect에 차례로 붙이기
        delLines.pop(); //(붙인 후 삭제)
    }
    return delRect; //완성된 delRect 반환
}
TetrisState CTetris::accept(Obj obj){
    state=Running;
    Matrix tempBlk;
    if(iCScreen.get_dy()==0) {return Finished;}
    if(obj.key==-1){
        Matrix upBlk=iCScreen.clip(obj.delRect.get_dy(), 4, 20, 19);
        iCScreen.paste(&upBlk,0,4); //iScreen의 맨 위에 upBlk 붙여넣기
        iCScreen.paste(&obj.delRect,20-obj.delRect.get_dy(),4);
    }
    else if(obj.key>='0'&&obj.key<='6')
    {
        idxBlockType=obj.key-'0';
        if(justStarted==false){
            deleteFullLines();
        }
        iCScreen=Matrix(oCScreen);
    }
    state=Tetris::accept(obj);
    currCBlk=Matrix(setOfCBlockObjects[idxBlockType][idxBlockDegree]);
    tempBlk=iCScreen.clip(top,left,top+currCBlk.get_dy(),left+currCBlk.get_dx());
    tempBlk=tempBlk.add(&currCBlk);
    oCScreen=Matrix(iCScreen);
    oCScreen.paste(&tempBlk,top,left);
    return state;
}
void CTetris::deleteFullLines(){
    for(int y=0;y<iScreenDy-nodelete;y++) //반복문통해 iScreen의 모든 줄을 돌며 채워진 줄이 있는지 확인
    {
        iScreen=Matrix(oScreen);
        iCScreen=Matrix(oCScreen);
        int top=y;
        int left=iScreenDw;
        bool isDelete=false;
        Matrix tempBlk=iScreen.clip(top,left,top+1,left+iScreenDx); //한줄씩 tempBlk으로 잘라서 확인
        if(tempBlk.sum()==iScreenDx){
            isDelete=true;
            Matrix Blk=iCScreen.clip(top,left,top+1,left+iScreenDx);
            delLines.push(Blk);
        }
        if(isDelete==true) //해당 줄이 채워졌으면
        {
            Matrix CtempBlk; 
            CtempBlk=iCScreen.clip(0,left,top,left+iScreenDx);
            //iCSreen의 가장 위부터 해당 줄 바로 위까지 잘라서 CtempBlk에 저장
            oCScreen=Matrix(iCScreen);
            oCScreen.paste(&CtempBlk,1,left); //oCScreen의 두번째 줄이 top이 되도록 CtempBlk 붙여넣기
        } 
    }
    return;
}
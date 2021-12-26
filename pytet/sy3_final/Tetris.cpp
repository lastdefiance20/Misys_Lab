#include "Tetris.h"
// #include "Matrix.h"
#include<cmath>

int Tetris::iScreenDw=0;
Matrix** Tetris::setOfBlockObjects;
int Tetris::nBlockTypes=0;
int Tetris::nBlockDegrees=0;
int Tetris::num=0;
Tetris::Tetris(){
    iScreenDx=0;
    iScreenDy=0;
    arrayScreen=new int[(iScreenDx+iScreenDw*2)*(iScreenDy+iScreenDw)];
    createArrayScreen(arrayScreen);
    iScreen=Matrix(arrayScreen,iScreenDy+iScreenDw,iScreenDx+2*iScreenDw);
    oScreen=Matrix(iScreen);
    idxBlockDegree=0;
    justStarted=true;
}
Tetris::Tetris(int Dy, int Dx){
    iScreenDy=Dy;
    iScreenDx=Dx;
    arrayScreen=new int[(iScreenDx+iScreenDw*2)*(iScreenDy+iScreenDw)];
    createArrayScreen(arrayScreen);
    iScreen=Matrix(arrayScreen,iScreenDy+iScreenDw,iScreenDx+2*iScreenDw);
    oScreen=Matrix(iScreen);
    idxBlockDegree=0;
    justStarted=true;
}
Tetris::~Tetris(){
    if(num==0){
        for(int i=0;i<nBlockTypes;i++)
            delete [] setOfBlockObjects[i];
        delete [] setOfBlockObjects;
    }
    delete [] arrayScreen;
}

void Tetris::init(int *setOfBlockArrays[], int MAX_BLK_TYPES, int MAX_BLK_DEGREES){
    nBlockDegrees=MAX_BLK_DEGREES;
    nBlockTypes=MAX_BLK_TYPES;
    setOfBlockObjects=new Matrix*[MAX_BLK_TYPES];
    for(int i=0;i<MAX_BLK_TYPES;i++)
    {
        setOfBlockObjects[i]=new Matrix[MAX_BLK_DEGREES];
        for(int j=0;j<MAX_BLK_DEGREES;j++)
        {
            int len=0;
            int where=0;
            for(int k=0;k!=-1;k=setOfBlockArrays[i*MAX_BLK_DEGREES+j][where++])
                len++;
            len--;
            len=sqrt(len);
            if(iScreenDw<=len) iScreenDw=len;
            setOfBlockObjects[i][j]=Matrix(setOfBlockArrays[i*MAX_BLK_DEGREES+j], len, len);
        }
    }
}
void Tetris::createArrayScreen(int* arrayScreen){
    int arrayScreenDx=iScreenDw*2+iScreenDx;
    int arrayScreenDy=iScreenDy+iScreenDw;
    
    for(int y=0;y<iScreenDy;y++)
    {
        for(int x=0;x<iScreenDw;x++)
            arrayScreen[y*arrayScreenDx+x]=1;
        for(int x=iScreenDw;x<iScreenDw+iScreenDx;x++)
            arrayScreen[y*arrayScreenDx+x]=0;
        for(int x=iScreenDw+iScreenDx;x<iScreenDw+2*iScreenDx;x++)
            arrayScreen[y*arrayScreenDx+x]=1;
    }
    for(int y=iScreenDy;y<arrayScreenDy;y++)
    {
        for(int x=0;x<arrayScreenDx;x++)
            arrayScreen[y*arrayScreenDx+x]=1;
    }
}
Matrix Tetris::getDelRect(){
    int size=delLines.size(); //테트리스 객체의 delLines 길이를 size로 저장
    Matrix delRect(size,15); //y축 길이가 size이고 x축은 full size인 delRect 생성 (이 delRect는 값이 모두 0으로 되어있는 기본 Matrix형태)
    for(int i=0;i<size;i++){
        delRect.paste(&(delLines.front()),i,0); //delLines에서 하나씩 가져와서 delRect에 차례로 붙이기
        delLines.pop(); //(붙인 후 삭제)
    }
    return delRect; //완성된 delRect 반환
}
TetrisState Tetris::accept(Obj obj){
    state=Running; //상태를 Running으로 설정
    Matrix tempBlk;
    
    if(iScreen.get_dy()==0){return Finished;}
    if(obj.key==-1){ //전달받은 obj의 키 값이 -1이면 delRect를 사용한다는 의미임
        int **arr=obj.delRect.get_array();
        int dy=obj.delRect.get_dy();
        int dx=obj.delRect.get_dx();
        for(int i=0;i<dy;i++){
            for(int j=0;j<dx;j++){
                arr[i][j]=1;
            }
        }
        Matrix delRect(*arr,dy,dx);
        obj.delRect=delRect;
        nodelete+=obj.delRect.get_dy(); //nodelete(지워지지 않아야하는 줄의 수)에 obj의 delRect의 y길이(높이) 더해줌
        
        Matrix upBlk=iScreen.clip(obj.delRect.get_dy(), 4, 20, 19); //현재 iScreen에서 delRect가 들어가고 위로 올라올 블록을 잘라서 upBlk에 저장

        iScreen.paste(&upBlk,0,4); //iScreen의 맨 위에 upBlk 붙여넣기
        iScreen.paste(&obj.delRect,20-obj.delRect.get_dy(),4); //iScreen에 delRect 붙여넣기
        //이렇게 iScreen 업데이트하고 지금 iScreen에는 currBlk이 안붙어있기 때문에
        tempBlk=iScreen.clip(top,left,top+currBlk.get_dy(),left+currBlk.get_dx()); //현재 iScreen에서 currBlk이 들어갈 자리 잘라서 tempBlk에 저장
        tempBlk=tempBlk.add(&currBlk); //tempBlk에 currBlk 붙여넣기
        while(tempBlk.anyGreaterThan(1)){ //tempBlk에 1보다 큰 수가 있다면
            if(top==0) break; //근데 top이 0이면 반복문 탈출 
            top=top-1; //currBlk을 위로 한 칸
            tempBlk=iScreen.clip(top,left,top+currBlk.get_dy(),left+currBlk.get_dx()); //현재 iScreen에서 currBlk이 들어갈 자리 잘라서 tempBlk에 저장
            tempBlk=tempBlk.add(&currBlk); //tempBlk에 currBlk 붙여넣기
        }
        if(tempBlk.anyGreaterThan(1)) //만약 아직도 tempBlk에 1보다 큰 수가 있다면
            state=Finished; //상태를 Finished로 설정
        oScreen=Matrix(iScreen); //iScreen을 copy해서 oScreen으로 설정
        oScreen.paste(&tempBlk,top,left); //oScreen에 tempBlk 붙여넣기
        return state; //현재 상태 반환하며 함수 종료
    }
    else if(obj.key>='0'&&obj.key<='6') //obj의 키 값이 새로운 블록의 값이면
    {
        if(justStarted==false){ //처음 시작할 때 새 블록 생성하는 경우가 아니라면
            deleteFullLines(); //deleteFullLines통해 지울 수 있는 줄은 지우기
            if(delLines.size()>0) state=NewBlockWDel; //delLines 길이가 0보다 크면(지운 줄이 있으면) 상태를 NewBlockWDel로 설정
        }
        iScreen=Matrix(oScreen); //가장 최근의 테트리스 게임 화면(oScreen)을 iScreen으로 설정 
        idxBlockDegree=0; 
        idxBlockType=obj.key-'0';
        currBlk=Matrix(setOfBlockObjects[idxBlockType][idxBlockDegree]); //currBlk을 새 블록으로 설정
        top=0; 
        left=iScreenDw+iScreenDx/2-currBlk.get_dx()/2;
        tempBlk=iScreen.clip(top,left,top+currBlk.get_dy(),left+currBlk.get_dx()); //현재 iScreen에서 currBlk이 들어갈 자리 잘라서 tempBlk에 저장
        //새 블록 등장인거니까 위치는 제일 위임
        tempBlk=tempBlk.add(&currBlk); //tempBlk에 currBlk 붙여넣기
        justStarted=false; 
        if(tempBlk.anyGreaterThan(1)) //만약 tempBlk에 1보다 큰 수가 있다면 (블록이 겹치는거임)
            state=Finished; //상태를 Finished로 설정 (제일 위에 붙였는데 블록이 겹친다면 게임 오버니까)
        oScreen=Matrix(iScreen); 
        oScreen.paste(&tempBlk,top,left); //가장 최근의 테트리스 게임화면(oScreen)에 tempBlk 붙여서 oScreen 업데이트
        return state; //현재 상태 반환하며 함수 종료
    }
    else if(obj.key=='q') //obj의 키 값이 q이면
        //std::cout<<"";//do not anything //걍 넘기기 (여기서 따로 다룰 필요 없음->어차피 Model::run에서 거름)
        return Finished;
    else if(obj.key=='a') //obj의 키 값이 a이면 
        left-=1; //left를 1 down (왼쪽으로 한칸)
    else if(obj.key=='d') //obj의 키 값이 d이면
        left+=1; //left를 1 up (오른쪽으로 한칸)
    else if(obj.key=='y') //obj의 키 값이 y이면
        top+=1; //top을 1 up (아래로 한칸)
    else if(obj.key=='w') //obj의 키 값이 w이면
    {
        idxBlockDegree=(idxBlockDegree+1)%nBlockDegrees;
        currBlk=Matrix(setOfBlockObjects[idxBlockType][idxBlockDegree]); //currBlk 90도 회전 시키기
    }
    else if(obj.key==' ') //obj의 키 값이 ' '이면
    {   
        while(!tempBlk.anyGreaterThan(1)) //tempBlk에 1보다 큰 수가 존재하지 않는 동안 반복
        {
            top+=1; //top을 1 up (아래로 한칸)
            tempBlk=iScreen.clip(top,left,top+currBlk.get_dy(),left+currBlk.get_dx()); //현재 iScreen에서 currBlk이 들어갈 자리 잘라서 tempBlk에 저장
            tempBlk=tempBlk.add(&currBlk); //tempBlk에 currBlk 붙여넣기
        } //일단 obj 키값이 ' '이면 여기서 한칸 오버해서 내려감
    }
    else //이외의 경우
        std::cout<<""; //그냥 넘어가기
    tempBlk=iScreen.clip(top,left,top+currBlk.get_dy(),left+currBlk.get_dx()); //현재 iScreen에서 currBlk이 들어갈 자리 잘라서 tempBlk에 저장
    tempBlk=tempBlk.add(&currBlk); //tempBlk에 currBlk 붙여넣기
    
    if(tempBlk.anyGreaterThan(1)) //만약 tempBlk에 1보다 큰 수가 존재한다면
    {
        if(obj.key=='a') //obj의 키 값이 a 였던 경우
            left+=1; //오른쪽으로 한칸
        else if(obj.key=='d') //d였던 경우
            left-=1; //왼쪽으로 한칸
        else if(obj.key=='y') //y였던 경우
        {
            top-=1; //위로 한칸
            state=NewBlockWODel; //상태를 NewBlockWODel로 설정 (밑이 닿아서 위로 갔다는 건 현재 currBlk이 이미 설치되었음을 의미)
        }
        else if(obj.key=='w') //w였던 경우
        {
            idxBlockDegree=(idxBlockDegree-1)%nBlockDegrees;
            currBlk=setOfBlockObjects[idxBlockType][idxBlockDegree]; //다시 반대로 90도 돌려놓기
        }
        else if(obj.key==' ') //' '였던 경우
        {
            top-=1; //아까 한칸 오버해서 내려온거 여기서 다시 올려주면서 해결
            state=NewBlockWODel; //상태를 NewBlockWODel로 설정 (사실 위에서 이미 설정하고 내려온거라 여기서 없어도 될 듯 함)
        }
        tempBlk=iScreen.clip(top,left,top+currBlk.get_dy(),left+currBlk.get_dx()); //이렇게 수정된 위치로 현재 iScreen에서 currBlk이 들어갈 자리 잘라서 tempBlk에 저장
        tempBlk=tempBlk.add(&currBlk); //tempBlk에 currBlk 붙여넣기
    }
    oScreen=Matrix(iScreen); //iScreen을 copy해서 oScreen으로 설정
    oScreen.paste(&tempBlk,top,left); //oScreen에 tempBlk 붙여서 oScreen 업데이트
    
    return state; //현재 상태 반환하며 함수 종료
}
void Tetris::deleteFullLines(){
    for(int y=0;y<iScreenDy-nodelete;y++)
    {
        iScreen=Matrix(oScreen);
        int top=y;
        int left=iScreenDw;
        bool isDelete=false;
        Matrix tempBlk=iScreen.clip(top,left,top+1,left+iScreenDx);
        if(tempBlk.sum()==iScreenDx){
            isDelete=true;
            delLines.push(tempBlk); //지운 줄 delLines에 추가
        }
        if(isDelete==true)
        {
            tempBlk=iScreen.clip(0,left,top,left+iScreenDx);
            oScreen=Matrix(iScreen);
            oScreen.paste(&tempBlk,1,left);
        } 
    }
    return;
}

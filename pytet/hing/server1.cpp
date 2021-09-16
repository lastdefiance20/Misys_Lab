#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "ModelView.h"
#include "SendRecv.h"

#define BUF_SIZE 1024

bool isGameDone=false;
std::mutex mu;
TetrisState state;

int main(int argc, char *argv[]) {
    // 파일 디스크립터를 위한 변수
    int serv_sock, clnt_sock1, clnt_sock2;
    char message[BUF_SIZE];
    int str_len, i;

    struct sockaddr_in serv_adr;
    struct sockaddr_in clnt_adr1;
    struct sockaddr_in clnt_adr2;
    socklen_t clnt_adr_sz;

    if (argc != 2) {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    // 1. socket 하나를 생성한다.
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1)
    {
        close(serv_sock);
        exit(1);        
    }

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));

    // 2. socket에 IP와 Port 번호를 할당한다.
    if (bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
    {
        close(serv_sock);
        exit(1);        
    }

    // 3. server socket(listen socket)을 통해 클라이언트의 접속 요청을 대기한다.
    //    5개의 수신 대기열(큐)을 생성한다.
    if (listen(serv_sock, 5) == -1)
    {
        close(serv_sock);
        exit(1);        
    }

    clnt_adr_sz=sizeof(clnt_adr1);

    while(1) {
        static bool isGameDone=false;
        // 4. 클라이언트 접속 요청을 수락한다. (클라이언트와 연결된 새로운 socket이 생성된다.)
        clnt_sock1 = accept(serv_sock, (struct sockaddr*)&clnt_adr1, &clnt_adr_sz);
        clnt_sock2 = accept(serv_sock, (struct sockaddr*)&clnt_adr2, &clnt_adr_sz);
        
        if (clnt_sock1 == -1 || clnt_sock2 == -1)
        {
            close(serv_sock);
            exit(1);        
        }
        else
            cout<<"Connected client\n";

        // 5. 클라이언트와 연결된 socket을 통해 데이터를 송수신한다.
        while((str_len=read(clnt_sock1, message, BUF_SIZE)) != 0){
            cout<<message[0]<<endl;
            write(clnt_sock2, message, str_len);
            if(message[0]=='q') write(clnt_sock1, message, str_len);
        }

        close(clnt_sock1);
        close(clnt_sock2);
    }

    close(serv_sock);
    return 0;
}
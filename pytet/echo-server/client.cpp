#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
using namespace std;

void usage(char *argv){
    cout << "Usage : " << argv << " [ip] [port]" << endl;
    cout << "Example) " << argv << " 192.168.0.12 1234" << endl;
}

int main(int argc, char *argv[]){
    if(argc != 3){ //인자가 3개가 아니면 usage를 출력
        usage(argv[0]);
        return -1;
    }

    struct sockaddr_in addr_server = {}; // 주소체계 구조체 선언

    memset(&addr_server, 0, sizeof(addr_server));
    addr_server.sin_family = AF_INET; // IPv4 통신, 기본 인터넷 프로토콜 지정
    addr_server.sin_addr.s_addr = inet_addr(argv[1]); // 첫번째 인자 IP 대입
    addr_server.sin_port = htons(atoi(argv[2])); // 두번째 인자 PORT 대입

    char w_buff[256]; // 쓰기용 버퍼 선언
    char r_buff[256]; // 읽기용 버퍼 선언

    int sock_client = socket(AF_INET, SOCK_STREAM, 0); // 소켓 생성(IPv4, TCP, 기본프로토콜)
    if(sock_client == -1){
        cout << "socket error" << endl;
        close(sock_client);
        exit(1);
    }

    if(connect(sock_client, (struct sockaddr*) &addr_server, sizeof(addr_server)) == -1){ // 연결 요청
        cout << "connect error" << endl;
        close(sock_client);
        exit(1);
    }

    cout << "connected" << endl;

    while(1){ // 연결 수락시 반복문
        memset(r_buff, 0, 256); // 읽기 버퍼 초기화
        cin >> w_buff; // 쓰기 버퍼에 문자열 입력
        if(strlen(w_buff)>255) break; // 버퍼 오버플로그 방지
        int write_chk = write(sock_client, w_buff, strlen(w_buff)); // 작성 길이만큼 write(전송)
        if(write_chk == -1){
            cout << "write error" << endl;
            break;
        }
        int read_chk = read(sock_client, r_buff, sizeof(r_buff)-1); // 읽기 버퍼사이즈-1 만큼 read(읽기)
        if(read_chk == -1){
            cout << "read error" << endl;
            break;
        }else{
            r_buff[strlen(r_buff)] = '\n';
            cout << "client received" << r_buff; // 버퍼 출력
        }
    }
    close(sock_client); // 연결 종료
    return 0;
}

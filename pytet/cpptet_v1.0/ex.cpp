#include <iostream>
#include <cstdlib>
#include <ctime>
#include <stdio.h>
#include <termios.h>

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <string>
#include <unistd.h>
#include <chrono>
#include <vector>

#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

#include <locale.h>
#include <ncursesw/curses.h>
#include<map>


using namespace std;

class Aa{
    public:
        int example;
        Aa(int x){
            example = x;
        }
        void getrun(int ex){
            example+=ex;
        }
        void run(){
            while(1){
                sleep(1);
                cout<<example<<endl;
            }
        }
};

class Bb{
    public:
        int ex;
        Aa* obsers[1];
        mutex mut;
        Bb(int x, mutex* m){
            ex = x;
            mut = m;
        }
        void addobser(Aa* obser){
            obsers[0] = obser;
        }
        void notifyobser(){
            mut->lock();
            obsers[0]->getrun(ex);
        }
        void run(){
            while(1){
                sleep(1);
                ex++;
                notifyobser();
            }
        }
};

void thread1(Aa* aclass){
    aclass->run();
}

void thread2(Bb* bclass){
    bclass->run();
}
 
int main()
{
    mutex* m;
    Aa aclass(10, &m);
    Aa cclass(10, &m);
    Bb bclass(10);
    
    vector<thread> threads;
    bclass.addobser(&aclass);

    threads.push_back(thread(thread1, &aclass));
    threads.push_back(thread(thread1, &cclass));
    threads.push_back(thread(thread2, &bclass));

    threads[0].join();
}

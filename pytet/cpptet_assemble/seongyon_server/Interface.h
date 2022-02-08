#pragma once
#include "CTetris.h"

class KeyObserver{
    public:
        virtual void updateKey(char key){};
};
class KeyPublisher{
    public:
        virtual void addObserverKey(KeyObserver* observer){};
        virtual void notifyObserversKey(char key){};
};
class OutScreenObserver{
    public:
        virtual void updateView(Matrix* screen){};
};
class OutScreenPublisher{
    public:
        virtual void addObserverView(OutScreenObserver* observer){};
        virtual void notifyObserversView(Matrix* screen){};
};
class delRectObserver{
    public:
        virtual void updateDel(Matrix delRect){};
};
class delRectPublisher{
    public:
        virtual void addObserverDel(delRectObserver* observer){};
        virtual void notifyObserversDel(Matrix delRect){};
};
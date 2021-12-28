#ifndef CLIENT_READ_H
#define CLIENT_READ_H

#include "../include/connectionHandler.h"
#include <thread>
#include <condition_variable>

class Read{
private:
    ConnectionHandler *connectionHandler;
    bool terminate;
    std::condition_variable& conditionV;

public:
    Read(ConnectionHandler &connection,std::condition_variable& conV);
    bool read();
};
#endif //CLIENT_READ_H

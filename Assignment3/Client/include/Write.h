#ifndef CLIENT_WRITE_H
#define CLIENT_WRITE_H


#include "../include/connectionHandler.h"
#include <thread>
#include <condition_variable>


class Write{
private:
    ConnectionHandler *connectionHandler;
    bool terminate;
    std::mutex& mut;
    std::condition_variable& cv;
public:
    Write(ConnectionHandler &connect , std::condition_variable& conV, std::mutex& mutex);
    bool write();
};
#endif //CLIENT_WRITE_H

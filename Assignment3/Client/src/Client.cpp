#include <stdlib.h>
#include "../include/connectionHandler.h"
#include "../include/Read.h"
#include "../include/Client.h"
#include "../include/Write.h"
#include <thread>
/**
* This code assumes that the server replies the exact text the client sent it (as opposed to the practical session example)
*/
int main (int argc, char *argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " host port" << std::endl << std::endl;
        return -1;
    }
    std::string host = argv[1];
    //std::string host= "127.0.0.1";
    short port = atoi(argv[2]);
    //short port = 7777;
    ConnectionHandler connectionHandler(host, port);
    if (!connectionHandler.connect()) {
        std::cerr << "Cannot connect to " << host << ":" << port << std::endl;
        return 1;
    }

    std::mutex mutex;
    std::condition_variable cv;
    Write w(connectionHandler,cv, mutex);
    Read r(connectionHandler,cv);
    std:: thread writer(&Write::write,&w);
    r.read();// main thread will read
    writer.join();


    return 0;
}

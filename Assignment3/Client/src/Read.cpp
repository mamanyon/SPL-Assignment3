#include "../include/Read.h"

Read::Read(ConnectionHandler &connection, std::condition_variable &con) :
        connectionHandler(), terminate(false), conditionV(con) {
    connectionHandler = &connection;
}

bool Read::read() {
    while (!terminate) {
        char received[4];
        if (!connectionHandler->getBytes(received, 4)) {
            std::cout << "Disconnected. Exiting... read \n" << std::endl;
            break;
        }
        short opcode = connectionHandler->bytesToShort(received, 0);
        short messageOpcode = connectionHandler->bytesToShort(received, 2);


        if (opcode == 13) {
            std::cout << "ERROR " << messageOpcode << std::endl;
           // break;
        }
        if (opcode == 12) {
            std::cout << "ACK " << messageOpcode << std::endl;
            std::string optional;
            if (messageOpcode == 4) {

                conditionV.notify_all();
                terminate = true;

                break;
            } else {
                if (messageOpcode == 6 | messageOpcode == 7 | messageOpcode == 8 | messageOpcode == 9 |
                    messageOpcode == 11) {
                    if (!connectionHandler->getLine(optional)) {
                        std::cout << "Disconnected. Exiting... read \n" << std::endl;
                        break;
                    }
                    std::cout << optional << std::endl;
                }
            }
        }
    }
    return false;
}



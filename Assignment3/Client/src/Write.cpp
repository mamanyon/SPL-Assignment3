
#include <mutex>
#include "../include/Write.h"

Write::Write(ConnectionHandler &connect, std::condition_variable &con, std::mutex &mutex) :
        connectionHandler(), terminate(false), mut(mutex), cv(con) {
    connectionHandler = &connect;
}

bool Write::write() {
    while (!terminate) {//bool
        const short bufsize = 1024;
        char buf[bufsize];
        std::cin.getline(buf, bufsize);
        std::string line(buf);


        std::string message;
        bool isThereMoreMessage = true;
        int indexMessage = 0;
        short opcode;
        bool sent;
        if (line == "LOGOUT") {
            opcode = 4;
            isThereMoreMessage = false;
            //message = line;
            terminate=true;
            sent = connectionHandler->sendFrame(opcode);
            std::unique_lock<std::mutex> lck(mut);
            cv.wait(lck);
        }
        if (line == "MYCOURSES") {
            opcode = 11;
            isThereMoreMessage = false;
           // message = line;
            sent = connectionHandler->sendFrame(opcode);
        }

        if (isThereMoreMessage) {


            // more message
            while (line[indexMessage] != ' ') {
                message += line[indexMessage];
                indexMessage++;
            }
            std::string restOfTheMessage = line.substr(indexMessage + 1);
            //1,2,3,8 same func
            if (message == "ADMINREG") {
                //rest of message is "username pass"
                sent = connectionHandler->sendFrame(restOfTheMessage, 1);
            } else if (message == "STUDENTREG") {
                //rest of message is "username pass"
                sent = connectionHandler->sendFrame(restOfTheMessage, 2);
            } else if (message == "LOGIN") {
                //rest of message is "username pass"
                sent = connectionHandler->sendFrame(restOfTheMessage, 3);
            } else if (message == "STUDENTSTAT") {
                sent = connectionHandler->sendFrame(restOfTheMessage, 8);
            } else
                //5 ,6 ,7, 9 ,10 same func getting op and course num .
            if (message == "COURSEREG") {
                //rest of message is coursenum
                std::stringstream courseNum(restOfTheMessage);
                short cNum = 0;
                courseNum >> cNum;  // converting string to short
                sent = connectionHandler->sendFrameShort(cNum, 5);
            } else if (message == "KDAMCHECK") {
                std::stringstream courseNum(restOfTheMessage);
                short cNum = 0;
                courseNum >> cNum;  // converting string to short
                sent = connectionHandler->sendFrameShort(cNum, 6);
            } else if (message == "COURSESTAT") {
                std::stringstream courseNum(restOfTheMessage);
                short cNum = 0;//
                courseNum >> cNum;  // converting string to short
                sent = connectionHandler->sendFrameShort(cNum, 7);
            } else if (message == "ISREGISTERED") {
                std::stringstream courseNum(restOfTheMessage);
                short cNum = 0;
                courseNum >> cNum;  // converting string to short
                sent = connectionHandler->sendFrameShort(cNum, 9);
            } else if (message == "UNREGISTER") {
                std::stringstream courseNum(restOfTheMessage);
                short cNum = 0;
                courseNum >> cNum;  // converting string to short
                sent = connectionHandler->sendFrameShort(cNum, 10);
            }


        }
        if (!sent) {
            std::cout << "Disconnected. Exiting... write \n" << std::endl;
            break;
        }

    }
    return false;
}




#include "../include/connectionHandler.h"
 
using boost::asio::ip::tcp;

using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::string;
 
ConnectionHandler::ConnectionHandler(string host, short port): host_(host), port_(port), io_service_(), socket_(io_service_){}
    
ConnectionHandler::~ConnectionHandler() {
    close();
}
 
bool ConnectionHandler::connect() {
    std::cout << "Starting connect to " 
        << host_ << ":" << port_ << std::endl;
    try {
		tcp::endpoint endpoint(boost::asio::ip::address::from_string(host_), port_); // the server endpoint
		boost::system::error_code error;
		socket_.connect(endpoint, error);
		if (error)
			throw boost::system::system_error(error);
    }
    catch (std::exception& e) {
        std::cerr << "Connection failed (Error: " << e.what() << ')' << std::endl;
        return false;
    }
    return true;
}
 
bool ConnectionHandler::getBytes(char bytes[], unsigned int bytesToRead) {
    size_t tmp = 0;
	boost::system::error_code error;
    try {
        while (!error && bytesToRead > tmp ) {
			tmp += socket_.read_some(boost::asio::buffer(bytes+tmp, bytesToRead-tmp), error);			
        }
		if(error)
			throw boost::system::system_error(error);
    } catch (std::exception& e) {
        std::cerr << "recv failed (Error: " << e.what() << ')' << std::endl;
        return false;
    }
    return true;
}

bool ConnectionHandler::sendBytes(const char bytes[], int bytesToWrite) {
    int tmp = 0;
	boost::system::error_code error;
    try {
        while (!error && bytesToWrite > tmp ) {
			tmp += socket_.write_some(boost::asio::buffer(bytes + tmp, bytesToWrite - tmp), error);
        }
		if(error)
			throw boost::system::system_error(error);
    } catch (std::exception& e) {
        std::cerr << "recv failed (Error: " << e.what() << ')' << std::endl;
        return false;
    }
    return true;
}
 
bool ConnectionHandler::getLine(std::string& line) {
    return getFrameAscii(line, '\0');
}

bool ConnectionHandler::sendLine(std::string& line) {
    return sendFrameAscii(line, '\n');
}
 

bool ConnectionHandler::getFrameAscii(std::string& frame, char delimiter) {
    char ch;
    // Stop when we encounter the null character.
    // Notice that the null character is not appended to the frame string.
    try {
	do{
		if(!getBytes(&ch, 1))
		{
			return false;
		}
		if(ch!='\0')  
			frame.append(1, ch);
	}while (delimiter != ch);
    } catch (std::exception& e) {
	std::cerr << "recv failed2 (Error: " << e.what() << ')' << std::endl;
	return false;
    }
    return true;
}
 
 
bool ConnectionHandler::sendFrameAscii(const std::string& frame, char delimiter) {
	bool result=sendBytes(frame.c_str(),frame.length());
	if(!result) return false;
	return sendBytes(&delimiter,1);
}
 
// Close down the connection properly.
void ConnectionHandler::close() {
    try{
        socket_.close();
    } catch (...) {
        std::cout << "closing failed: connection already closed" << std::endl;
    }
}
void ConnectionHandler::shortToBytes(short num, char* bytesArr) {
    bytesArr[0] = ((num >> 8) & 0xFF);
    bytesArr[1] = (num & 0xFF);
}

short ConnectionHandler::bytesToShort(char *bytesArr, int i) {
    short result = (short)((bytesArr[i] & 0xff) << 8);
    result += (short)(bytesArr[i+1] & 0xff);
    return result;
}

//for messages 4 | 11 ... only send the opcode
bool ConnectionHandler::sendFrame(short opCode) {
    bool flag;
    char *bitArr = new char[2];
    shortToBytes(opCode,bitArr);
    flag = sendBytes(bitArr, 2);
    delete[] bitArr;
    return flag;
}
 //for messages 1|2|3 . opcode username \0 pass \0
 //or message 8 . opcode username \0
bool ConnectionHandler::sendFrame(std::string restOfMessage, short opCode) {
    bool flag;
    int length = restOfMessage.length()+3;
    char *bitArr = new char[length];
    shortToBytes(opCode,bitArr);
    strcpy(bitArr+2,restOfMessage.c_str());
    for(int i =0; i<length-1;i++)
        if(bitArr[i]==' ') bitArr[i]='\0';
    bitArr[length-1] ='\0';

    flag = sendBytes(bitArr, length);
    delete[] bitArr;
    return flag;

}
//for messages 5 ,6 ,7, 9 ,10
bool ConnectionHandler::sendFrameShort(short courseNum, short opCode) {
    bool flag;
    char *bitArr = new char[4];
    shortToBytes(opCode,bitArr);
    shortToBytes(courseNum,bitArr+2);
    flag = sendBytes(bitArr, 4);
    delete[] bitArr;
    return flag;
}

bool ConnectionHandler::getFrame(std::string Recieved) {
    return false;
}

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>

#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <port number>" << std::endl;
        return 1;
    }

    addrinfo hints{.ai_protocol = IPPROTO_TCP,
                   .ai_socktype = SOCK_STREAM,
                   .ai_family = PF_INET};

    addrinfo* result;

    int getaddrresult = getaddrinfo("localhost", argv[1], &hints, &result);
    if (getaddrresult != 0) {
        std::cerr << "getaddrinfo failed: " << gai_strerror(getaddrresult) << std::endl;
        return 1;
    }

    if (result == nullptr) {
        std::cerr << "getaddrinfo returned nullptr" << std::endl;
        return 1;
    }

    int sockFd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

    if (sockFd == -1) {
        std::cerr << "socket failed" << std::endl;
        return 1;
    }

    int connectResult = connect(sockFd, result->ai_addr, result->ai_addrlen);

    if (connectResult == -1) {
        std::cerr << "connect failed" << std::endl;
        return 1;
    }

    std::cout << "Connected to server" << std::endl;

    while (true) {
        std::string message;
        std::cout << "Please enter message: ";
        std::getline(std::cin, message);

        if (message == "exit") {
            break;
        }

        int sendResult = send(sockFd, message.c_str(), message.size(), 0);

        if (sendResult == -1) {
            perror("send failed");
            return 1;
        }

        std::cout << "Sent " << sendResult << " bytes" << std::endl;
    }

    std::cout << "Closing connection" << std::endl;
    close(sockFd);
    freeaddrinfo(result);


    return 0;
}
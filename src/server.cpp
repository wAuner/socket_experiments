#include <fcntl.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <format>
#include <iostream>
#include <print>

using namespace std;

pair<string, string> getHostAndPort(const sockaddr *addr) {
    char host[NI_MAXHOST];
    char port[NI_MAXSERV];
    // maybe sockaddr_storage is better? that's what copilot suggested
    int getnameinforesult = getnameinfo(addr, sizeof(addr->sa_len), host,
                                        NI_MAXHOST, port, NI_MAXSERV, 0);
    if (getnameinforesult != 0) {
        return {"<unknown>", "<unknown>"};
    }
    return {host, port};
}

int main() {
    addrinfo hints{.ai_protocol = IPPROTO_TCP,
                   .ai_socktype = SOCK_STREAM,
                   .ai_family = PF_INET};
    //    .ai_flags = AI_PASSIVE}; // with AI_PASSIVE, we can use nullptr for
    //    the host and it binds to all interfaces
    // The three members ai_family, ai_socktype, and
    // ai_protocol in each returned addrinfo structure are suitable for a call
    // to socket(2)
    addrinfo *result;
    const char *port = "50069";
    int getaddrresult = getaddrinfo("localhost", port, &hints, &result);
    if (getaddrresult != 0) {
        println("getaddrinfo failed: <{}>", gai_strerror(getaddrresult));
        return -1;
    }

    if (result == nullptr) {
        println("getaddrinfo returned nullptr");
        return -1;
    }

    const auto [host, portString] = getHostAndPort(result->ai_addr);
    int sockFd =
        socket(result->ai_family, result->ai_socktype, result->ai_protocol);

    if (sockFd == -1) {
        println("socket failed: <{}>", strerror(errno));
        return -1;
    }

    if (bind(sockFd, result->ai_addr, result->ai_addrlen) == -1) {
        println("bind failed: <{}>", strerror(errno));
        return -1;
    }
    println("bound to port {}", portString);

    if (listen(sockFd, 10) == -1) {
        println("listen failed: <{}>", strerror(errno));
        return -1;
    }

    println("listening for incoming connections on {}:{}", host, portString);

    sockaddr_storage clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    while (true) {
        int clientFd = accept(sockFd, (sockaddr *)&clientAddr, &clientAddrLen);
        if (clientFd == -1) {
            println("accept failed: <{}>", strerror(errno));
            return -1;
        }
        const auto [clientHost, clientPort] =
            getHostAndPort((sockaddr *)&clientAddr);
        println("accepted connection from {}:{}", clientHost, clientPort);

        std::vector<char> buffer(1024);
        int bytesReceived;
        while (true) {
            bytesReceived = recv(clientFd, buffer.data(), buffer.size(), 0);
            if (bytesReceived == -1) {
                println("recv failed: <{}>", strerror(errno));
                return -1;
            } else if (bytesReceived > 0) {
                std::string receivedData(buffer.data(), bytesReceived);
                println("received {} bytes: <{}>", bytesReceived, receivedData);
            } else if (bytesReceived == 0) {
                println("client closed connection");
                break;
            }
            buffer.clear();
        } 

        close(clientFd);
        println("closed connection to {}:{}", clientHost, clientPort);
    } 
    close(sockFd);

    freeaddrinfo(result);
    return 0;
}

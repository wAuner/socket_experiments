#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <stdio.h>
#include <vector>

using namespace std;

pair<string, string> getHostAndPort(const sockaddr *addr) {
    char host[NI_MAXHOST];
    char port[NI_MAXSERV];
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
    addrinfo *result;
    const char *port = "50069";
    int getaddrresult = getaddrinfo("localhost", port, &hints, &result);
    if (getaddrresult != 0) {
        cout << "getaddrinfo failed: " << gai_strerror(getaddrresult) << endl;
        return -1;
    }

    if (result == nullptr) {
        cout << "getaddrinfo returned nullptr" << endl;
        return -1;
    }

    const auto [host, portString] = getHostAndPort(result->ai_addr);
    int sockFd =
        socket(result->ai_family, result->ai_socktype, result->ai_protocol);

    if (sockFd == -1) {
        perror("socket failed");
        return -1;
    }

    if (bind(sockFd, result->ai_addr, result->ai_addrlen) == -1) {
        perror("bind failed");
        return -1;
    }
    cout << "bound to port " << portString << endl;

    if (listen(sockFd, 10) == -1) {
        perror("listen failed");
        return -1;
    }

    cout << "listening for incoming connections on " << host << ":"
         << portString << endl;

    sockaddr_storage clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    while (true) {
        int clientFd = accept(sockFd, (sockaddr *)&clientAddr, &clientAddrLen);
        if (clientFd == -1) {
            perror("accept failed");
            return -1;
        }
        const auto [clientHost, clientPort] =
            getHostAndPort((sockaddr *)&clientAddr);
        cout << "accepted connection from " << clientHost << ":" << clientPort
             << endl;

        int bytesReceived;
        while (true) {
            std::vector<char> buffer(1024);
            bytesReceived = recv(clientFd, buffer.data(), buffer.size(), 0);
            if (bytesReceived == -1) {
                perror("recv failed");
                return -1;
            } else if (bytesReceived > 0) {
                std::string receivedData(buffer.data(), bytesReceived);
                cout << "received " << bytesReceived << " bytes: <"
                     << receivedData << ">" << endl;
            } else if (bytesReceived == 0) {
                cout << "client closed connection" << endl;
                break;
            }
            buffer.clear();
        }

        close(clientFd);
        cout << "closed connection to " << clientHost << ":" << clientPort
             << endl;
    }
    close(sockFd);

    freeaddrinfo(result);
    return 0;
}

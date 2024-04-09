#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

#include <format>
#include <print>
#include <iostream>

using namespace std;

const char* protocolToString(int protocol) {
    switch (protocol) {
        case IPPROTO_TCP:
            return "TCP";
        case IPPROTO_UDP:
            return "UDP";
        default:
            return "Unknown";
    }
}

int main(int argc, const char * argv[]) {
   addrinfo hints {.ai_protocol = IPPROTO_TCP};
   addrinfo* result;
   
   int getaddrresult = getaddrinfo("vastlimits.com", "https", &hints, &result);
   if (getaddrresult != 0) {
      println("getaddrinfo failed: <{}>", gai_strerror(getaddrresult));
      return -1;
   }
   
   int counter = 1;
   for (addrinfo* entry = result; entry != nullptr; entry = entry->ai_next) {
      char address[NI_MAXHOST];
      char service[NI_MAXSERV];
      if (getnameinfo(entry->ai_addr, entry->ai_addrlen, address, NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICHOST) != 0) {
         println("getnameinfo failed: <{}>", gai_strerror(getaddrresult));
         return -1;
      
      }
      println("Element no. {}: address is: <{}>, service is: <{}>, protocol: <{}>", counter, address, service, protocolToString(entry->ai_protocol));
      counter++;
   }
   
   println("hello {}", "world");
   return 0;
}

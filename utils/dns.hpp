#ifndef DNS_HPP
#define DNS_HPP

#include <string>
#include <stdexcept>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cstring>

namespace Utils {

    std::string resolve_hostname(const std::string& hostname) {
        struct addrinfo hints, *res;
        char ip_str[INET_ADDRSTRLEN];
        
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET; // Только IPv4
        hints.ai_socktype = SOCK_STREAM;
        
        int status = getaddrinfo(hostname.c_str(), NULL, &hints, &res);
        
        if (status != 0) {
            throw std::runtime_error("DNS resolution failed for " + hostname + 
                                     ": " + std::string(gai_strerror(status)));
        }
        
        struct sockaddr_in *ipv4 = (struct sockaddr_in *)res->ai_addr;
        inet_ntop(AF_INET, &(ipv4->sin_addr), ip_str, INET_ADDRSTRLEN);
        
        freeaddrinfo(res);
        
        return std::string(ip_str);
    }

} // namespace Utils

#endif // DNS_HPP

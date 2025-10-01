#include "dns.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <cstring>
#include <iostream>

std::string Utils::resolve_hostname(const std::string& hostname) {
    struct addrinfo hints, *res, *p;
    int status;
    char ip_str[INET_ADDRSTRLEN];
    std::string ip_address;

    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;  
    hints.ai_socktype = SOCK_STREAM;

    status = getaddrinfo(hostname.c_str(), NULL, &hints, &res);
    
    if (status != 0) {
        std::string error_msg = "Hostname resolution failed for '";
        error_msg += hostname + "': " + gai_strerror(status);
        throw std::runtime_error(error_msg);
    }

    for (p = res; p != NULL; p = p->ai_next) {
        struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
        
        if (inet_ntop(AF_INET, &(ipv4->sin_addr), ip_str, INET_ADDRSTRLEN) != NULL) {
            ip_address = ip_str;
            break;
        }
    }

    freeaddrinfo(res);

    if (ip_address.empty()) {
        throw std::runtime_error("Could not find a valid IPv4 address for the specified hostname.");
    }

    return ip_address;
}


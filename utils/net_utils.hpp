#ifndef NET_UTILS_HPP
#define NET_UTILS_HPP

#include <iostream>
#include <string>
#include <vector>
#include <ifaddrs.h>
#include <cstring>
#include <algorithm>

namespace Utils {

    std::vector<std::string> get_all_interfaces() {
        std::vector<std::string> interfaces;
        struct ifaddrs *ifaddr, *ifa;

        if (getifaddrs(&ifaddr) == -1) {
            return interfaces;
        }

        for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
            if (ifa->ifa_addr == NULL) continue;
            
            std::string name = ifa->ifa_name;
            
            bool exists = false;
            if (std::find(interfaces.begin(), interfaces.end(), name) != interfaces.end()) {
                exists = true;
            }
            
            if (!exists) {
                interfaces.push_back(name);
            }
        }

        freeifaddrs(ifaddr);

        return interfaces;
    }

}

#endif

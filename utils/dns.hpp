#ifndef DNS_HPP
#define DNS_HPP

#include <string>
#include <stdexcept>

namespace Utils {
    std::string resolve_hostname(const std::string& hostname);
}

#endif


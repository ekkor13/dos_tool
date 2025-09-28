#ifndef CORE_ATTACKER_HPP
#define CORE_ATTACKER_HPP

#include "../cli/cli.hpp" // Для TargetInfo
#include <string>


class CoreAttacker {
public:
    // Конструктор принимает цель
    CoreAttacker(const TargetInfo& target);

    // Главный метод: запускает бесконечный цикл отправки SYN-пакетов
    void start_flood();

private:
    const TargetInfo target_;
    
    // Поле для хранения ID сокета
    int raw_socket_fd_ = -1; 
    
    // Приватные методы для инициализации
    void create_raw_socket();
    void set_socket_options();
    void send_packet();
};

#endif 

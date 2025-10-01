#include "packet.hpp"
#include "../utils/checksum.hpp"
#include <cstdint>
#include <cstdlib>
#include <cstring>          
#include <iostream>
#include <netinet/in.h>
#include <random>
#include <cstdlib>

// Константы для сырого пакета
const int IP_HEADER_LEN = 20; // 20 байт - стандартная длина IP-заголовка
const int TCP_HEADER_LEN = 20; // 20 байт - стандартная длина TCP-заголовка
const int TOTAL_PACKET_LEN = IP_HEADER_LEN + TCP_HEADER_LEN;

// Конструктор
PacketGenerator::PacketGenerator(const TargetInfo& target)
    : target_(target), buffer_(TOTAL_PACKET_LEN) // Инициализируем буфер
{
    std::memset(buffer_.data(), 0, TOTAL_PACKET_LEN);
}

// Главный метод сборки
std::vector<char> PacketGenerator::get_raw_packet() const {
    struct iphdr* ip_h = (struct iphdr*)buffer_.data();
    struct tcphdr* tcp_h = (struct tcphdr*)(buffer_.data() + IP_HEADER_LEN);

    // Заполнение заголовков
    fill_ip_header(ip_h);
    fill_tcp_header(tcp_h);
    
    // IP Checksum
    ip_h->check = 0; // Обнуляем поле перед расчетом
    ip_h->check = checksum((uint16_t*)ip_h, IP_HEADER_LEN);

    // TCP Checksum
    struct PseudoHeader psh;
    psh.source_ip = ip_h->saddr;
    psh.dest_ip = ip_h->daddr;
    psh.tcp_length = htons(TCP_HEADER_LEN); // Длина в сетевом порядке байтов
    
    // Буфер для склейки PseudoHeader + TCP Header
    int psh_size = sizeof(PseudoHeader) + TCP_HEADER_LEN;
    std::vector<char> psh_buff(psh_size);

    std::memcpy(psh_buff.data(), &psh, sizeof(PseudoHeader));
    std::memcpy(psh_buff.data() + sizeof(PseudoHeader), tcp_h, TCP_HEADER_LEN);

    tcp_h->check = 0; // Обнуляем поле перед расчетом
    tcp_h->check = checksum((uint16_t*)psh_buff.data(), psh_size);
    
    return buffer_;
}

// Заполнение IP-заголовка
void PacketGenerator::fill_ip_header(struct iphdr* ip_h) const {
    ip_h->version = 4;
    ip_h->ihl = 5;
    ip_h->tos = 0; // Тип сервиса
    ip_h->tot_len = htons(TOTAL_PACKET_LEN); // Общая длина пакета в сетевом порядке
    ip_h->id = htons(12345); // ID пакета (можно рандомизировать)
    ip_h->frag_off = 0; // Флаг фрагментации
    ip_h->ttl = 64; // Time to Live
    ip_h->protocol = IPPROTO_TCP; // TCP = 6
    ip_h->check = 0; // Будет рассчитана позже
   
    ip_h->saddr=get_random_ip();
    ip_h->daddr = inet_addr(target_.ip.c_str()); 
}

// Вспомогательная функция: Заполнение TCP-заголовка
void PacketGenerator::fill_tcp_header(struct tcphdr* tcp_h) const {
    // Порты
    tcp_h->source=htons(get_random_ip());
    tcp_h->dest = htons(target_.port); // Целевой порт в сетевом порядке
    
    tcp_h->seq=htons(get_random_seq());
    tcp_h->ack_seq = 0; // ACK = 0 для SYN-пакета
    
    tcp_h->doff = 5;
    
    // Флаги TCP
    tcp_h->syn = 1;
    tcp_h->ack = 0;
    tcp_h->psh = 0;
    tcp_h->rst = 0;
    tcp_h->fin = 0;
    
    tcp_h->window = htons(5840); // Размер окна
    tcp_h->check = 0; // Будет рассчитана позже
    tcp_h->urg_ptr = 0; // Указатель срочности

}

uint32_t PacketGenerator::get_random_ip() const{

  uint32_t random_ip = (static_cast<uint32_t>(std::rand()) << 16) | static_cast<uint32_t>(std::rand());
  return random_ip;
}

uint16_t PacketGenerator::get_random_port() const{
  const int MIN_PORT = 1025;
  const int MAX_PORT = 65535;

  return static_cast<uint16_t>((std::rand() % (MAX_PORT - MIN_PORT + 1)) + MIN_PORT);
}

uint32_t PacketGenerator::get_random_seq() const {
    return (static_cast<uint32_t>(std::rand()) << 16) | static_cast<uint32_t>(std::rand());
}

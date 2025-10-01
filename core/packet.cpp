#include "packet.hpp"
#include "../utils/checksum.hpp"
#include <cstdint>
#include <cstdlib>
#include <cstring>          
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdexcept>

const int IP_HEADER_LEN = 20; 
const int TCP_HEADER_LEN = 20; 
const int TOTAL_PACKET_LEN = IP_HEADER_LEN + TCP_HEADER_LEN;

PacketGenerator::PacketGenerator(const TargetInfo& target)
    : target_(target), buffer_(TOTAL_PACKET_LEN) 
{
    std::memset(buffer_.data(), 0, TOTAL_PACKET_LEN);
}

std::vector<char> PacketGenerator::get_raw_packet() const {
    struct iphdr* ip_h = (struct iphdr*)buffer_.data();
    struct tcphdr* tcp_h = (struct tcphdr*)(buffer_.data() + IP_HEADER_LEN);

    fill_ip_header(ip_h);
    fill_tcp_header(tcp_h);
    
    ip_h->check = 0; 
    ip_h->check = checksum((uint16_t*)ip_h, IP_HEADER_LEN);

    PseudoHeader psh;
    psh.source_ip = ip_h->saddr;
    psh.dest_ip = ip_h->daddr;
    psh.placeholder = 0;
    psh.protocol = IPPROTO_TCP;
    psh.tcp_length = htons(TCP_HEADER_LEN);
    
    int psh_size = sizeof(PseudoHeader) + TCP_HEADER_LEN;
    std::vector<char> psh_packet(psh_size);
    std::memcpy(psh_packet.data(), &psh, sizeof(PseudoHeader));
    std::memcpy(psh_packet.data() + sizeof(PseudoHeader), tcp_h, TCP_HEADER_LEN);
    
    tcp_h->check = 0;
    tcp_h->check = checksum((uint16_t*)psh_packet.data(), psh_size);

    return buffer_;
}

void PacketGenerator::fill_ip_header(struct iphdr* ip_h) const {
    ip_h->ihl = 5;
    ip_h->version = 4;
    ip_h->tos = 0;
    ip_h->tot_len = htons(TOTAL_PACKET_LEN);
    ip_h->id = htons(12345);
    ip_h->frag_off = 0;
    ip_h->ttl = 64;
    ip_h->protocol = IPPROTO_TCP;
    ip_h->check = 0;
   
    ip_h->saddr=get_random_ip();
    ip_h->daddr = inet_addr(target_.ip.c_str()); 
}

void PacketGenerator::fill_tcp_header(struct tcphdr* tcp_h) const {
    tcp_h->source = htons(get_random_port());
    tcp_h->dest = htons(target_.port);
    
    tcp_h->seq = htonl(get_random_seq());
    tcp_h->ack_seq = 0;
    
    tcp_h->doff = 5;
    
    tcp_h->syn = 1;
    tcp_h->ack = 0;
    tcp_h->psh = 0;
    tcp_h->rst = 0;
    tcp_h->fin = 0;
    
    tcp_h->window = htons(5840);
    tcp_h->check = 0;
    tcp_h->urg_ptr = 0;
}

uint32_t PacketGenerator::get_random_ip() const {
    uint32_t random_ip = (static_cast<uint32_t>(std::rand()) << 16) | static_cast<uint32_t>(std::rand());
    return random_ip;
}

uint16_t PacketGenerator::get_random_port() const {
    const int MIN_PORT = 1025;
    const int MAX_PORT = 65535;
    
    return static_cast<uint16_t>((std::rand() % (MAX_PORT - MIN_PORT + 1)) + MIN_PORT);
}

uint32_t PacketGenerator::get_random_seq() const {
    return (static_cast<uint32_t>(std::rand()) << 16) | static_cast<uint32_t>(std::rand());
}

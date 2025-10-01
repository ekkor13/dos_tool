#ifndef PACKET_HPP
#define PACKET_HPP

#include <cstdint>
#include <string>
#include <vector>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include "../cli/cli.hpp"

struct TargetInfo;

struct PseudoHeader{
  uint32_t source_ip;
  uint32_t dest_ip;
  uint8_t placeholder = 0;
  uint8_t protocol = IPPROTO_TCP;
  uint16_t tcp_length;
};

class PacketGenerator{
  public:

    PacketGenerator(const TargetInfo& target);

    std::vector<char> get_raw_packet() const;

  private:

    const TargetInfo& target_;

    std::vector<char> buffer_;

    void fill_ip_header(struct iphdr* ip_h) const;
    void fill_tcp_header(struct tcphdr* tcp_h) const;
    void calculate_checksums();

    uint32_t get_random_ip() const;
    uint16_t get_random_port() const;
    uint32_t get_random_seq() const;
};

#endif

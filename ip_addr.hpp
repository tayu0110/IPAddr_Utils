#ifndef __IP_ADDR_H_INCLUDED__
#define __IP_ADDR_H_INCLUDED__

#include <string>
#include <vector>
#include <stdint.h>

namespace ip_addr {
  class IPv4Addr {
    uint8_t cidr;
    uint8_t octet[4];
   public:
    IPv4Addr() : cidr{0}, octet{0, 0, 0, 0} {}
    IPv4Addr(uint32_t addr, uint8_t cidr = 32) : cidr{cidr}, octet{addr >> 24, (addr >> 16) & 0xFF, (addr >> 8) & 0xFF, addr & 0xFF} {}
    IPv4Addr(uint8_t oct1, uint8_t oct2, uint8_t oct3, uint8_t oct4, uint8_t cidr = 32) : cidr{cidr}, octet{oct1, oct2, oct3, oct4} {}
    IPv4Addr(std::string addr_str);
    IPv4Addr(std::vector<uint8_t> addr, uint8_t cidr = 32);
    std::string to_string() const;
    std::string to_bin_string() const;
    uint32_t to_32bit_integer() const;
    bool contain(const IPv4Addr &addr) const;
    bool operator==(const IPv4Addr &rhs) {
      return cidr == rhs.cidr && octet[0] == rhs.octet[0] && octet[1] == rhs.octet[1] && octet[2] == rhs.octet[2] && octet[3] == rhs.octet[3];
    }
  };

  class IPv6Addr {
    uint8_t cidr;
    uint16_t hectet[8];
   public:
    IPv6Addr() : cidr{0}, hectet{0, 0, 0, 0, 0, 0, 0, 0} {}
    IPv6Addr(uint64_t addr_hi, uint64_t addr_lo, uint8_t cidr = 128)
      : cidr{cidr},
        hectet{addr_hi >> 48, (addr_hi >> 32) & 0xFFFF, (addr_hi >> 16) & 0xFFFF, addr_hi & 0xFFFF,
               addr_lo >> 48, (addr_lo >> 32) & 0xFFFF, (addr_lo >> 16) & 0xFFFF, addr_lo & 0xFFFF} {}
    IPv6Addr(uint16_t hex1, uint16_t hex2, uint16_t hex3, uint16_t hex4, uint16_t hex5, uint16_t hex6, uint16_t hex7, uint16_t hex8, uint8_t cidr = 128)
      : cidr{cidr}, hectet{hex1, hex2, hex3, hex4, hex5, hex6, hex7, hex8} {}
    IPv6Addr(std::string addr_str);
    IPv6Addr(std::vector<uint16_t> addr, uint8_t cidr = 128);
    std::string to_string() const;
    std::string to_bin_string() const;
    std::pair<uint64_t, uint64_t> to_64bit_integer_pair() const;
    __uint128_t to_128bit_integer() const;
    bool contain(const IPv6Addr &addr) const;
    bool operator==(const IPv6Addr &rhs) {
      if(cidr != rhs.cidr) return false;
      for(int i = 0; i < 8; i++) if(hectet[i] == rhs.hectet[i]) return false;
      return true;
    }
  };
}

#endif
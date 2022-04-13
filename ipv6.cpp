#include "ipaddr_utils/ip_addr.hpp"

#include <string>
#include <regex>


static std::pair<std::string, std::string> split_cidr(std::string addr) {
  int pos = addr.find('/');
  if(pos == addr.npos)
    return std::make_pair(addr, "128");

  std::string cidr = addr.substr(pos+1);
  addr = addr.substr(0, pos);

  return std::make_pair(addr, cidr);
}



namespace ip_addr {
  const std::regex ipv6_ptrn("^((([0-9a-f]{1,4}:){7}([0-9a-f]{1,4}|:))|(([0-9a-f]{1,4}:){6}(:[0-9a-f]{1,4}|((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3})|:))|(([0-9a-f]{1,4}:){5}(((:[0-9a-f]{1,4}){1,2})|:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3})|:))|(([0-9a-f]{1,4}:){4}(((:[0-9a-f]{1,4}){1,3})|((:[0-9a-f]{1,4})?:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(([0-9a-f]{1,4}:){3}(((:[0-9a-f]{1,4}){1,4})|((:[0-9a-f]{1,4}){0,2}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(([0-9a-f]{1,4}:){2}(((:[0-9a-f]{1,4}){1,5})|((:[0-9a-f]{1,4}){0,3}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(([0-9a-f]{1,4}:){1}(((:[0-9a-f]{1,4}){1,6})|((:[0-9a-f]{1,4}){0,4}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(:(((:[0-9a-f]{1,4}){1,7})|((:[0-9a-f]{1,4}){0,5}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:)))\(/(1[0-2][0-9]|[0-9]?[0-9]))?$");

  IPv6Addr::IPv6Addr(std::string addr_str) {
    if(!std::regex_match(addr_str, ipv6_ptrn))
      throw std::invalid_argument("Given IPv4 Address is invalid");

    auto [address, cidr_str] = split_cidr(addr_str);
    cidr = std::stoi(cidr_str);

    int idx = 0, i = 0;
    memset(hectet, 0, sizeof(hectet));

    for(i = 0; i < address.length(); i++) {
      char c = address[i];
      if(c == ':') {
        if(address[i+1] == ':')
          break;
        idx++;
        continue;
      }
      int k = ('0' <= c && c <= '9') ? c - '0' : c - 'a' + 10;
      hectet[idx] = (hectet[idx] << 4) | k;
    }

    if(i != address.length()) {
      std::string buf;
      idx = 7;
      for(i = address.length()-1; i >= 0; i--) {
        char c = address[i];
        if(c == ':') {
          for(auto nc : buf) {
            int k = ('0' <= nc && nc <= '9') ? nc - '0' : nc - 'a' + 10;
            hectet[idx] = (hectet[idx] << 4) | k;
          }
          if(address[i-1] == ':') break;
          idx--;
          buf = "";
          continue;
        }
        buf = buf + c;
      }
    }
  }


  IPv6Addr::IPv6Addr(std::vector<uint16_t> addr, uint8_t cidr = 128) : cidr{cidr} {
    if(!addr.size() != 8)
      throw std::invalid_argument("The number of IPv6 Address elements must be 8.");

    for(int i = 0; i < 8; i++)
      hectet[i] = addr[i];
  }


  std::string IPv6Addr::to_string() const {
    std::string res;

    for(int i = 0; i < 8; i++) {
      if(i) res += ":";
      res += std::to_string(hectet[i]);
    }
    res += "/";
    res += std::to_string(cidr);

    return res;
  }


  std::string IPv6Addr::to_bin_string() const {
    std::string res;

    for(int i = 7; i >= 0; i--) {
      uint8_t hc = hectet[i];
      for(int j = 0; j < 16; j++) {
        res += '0' + !!(hc & 1);
        hc >>= 1;
      }
    }
    std::reverse(res.begin(), res.end());

    return res;
  }


  std::pair<uint64_t, uint64_t> IPv6Addr::to_64bit_integer_pair() const {
    uint64_t addr_hi = ((uint64_t)hectet[0] << 48) | ((uint64_t)hectet[1] << 32) | ((uint64_t)hectet[2] << 16) | (uint64_t)hectet[3];
    uint64_t addr_lo = ((uint64_t)hectet[4] << 48) | ((uint64_t)hectet[5] << 32) | ((uint64_t)hectet[6] << 16) | (uint64_t)hectet[7];

    return std::make_pair(addr_hi, addr_lo);
  }


  __uint128_t IPv6Addr::to_128bit_integer() const {
    auto [hi, lo] = to_64bit_integer_pair();

    return ((__uint128_t)hi << 64) | (__uint128_t)lo;
  }


  bool IPv6Addr::contain(const IPv6Addr &addr) const {
    __uint128_t this_int = to_128bit_integer();
    __uint128_t addr_int = addr.to_128bit_integer();

    return (this_int & addr_int) == this_int;
  }
}
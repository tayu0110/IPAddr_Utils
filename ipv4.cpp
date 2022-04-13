#include "ipaddr_utils/ip_addr.hpp"

#include <string>
#include <regex>


static std::pair<std::string, std::string> split_cidr(std::string addr) {
  int pos = addr.find('/');
  if(pos == addr.npos)
    return std::make_pair(addr, "32");

  std::string cidr = addr.substr(pos+1);
  addr = addr.substr(0, pos);

  return std::make_pair(addr, cidr);
}



namespace ip_addr {
  const std::regex ipv4_ptrn("^((25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9]?[0-9])\.){3}(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9]?[0-9])(\/(3[0-2]|[1-2]?[0-9]))$");
  
  IPv4Addr::IPv4Addr(std::string addr_str) {
    if(!std::regex_match(addr_str, ipv4_ptrn))
      throw std::invalid_argument("Given IPv4 Address is invalid");

    auto [address, cidr_str] = split_cidr(addr_str);
    cidr = std::stoi(cidr_str);

    int idx = 0;
    std::string buf;
    
    for(auto c : address) {
      if(c == '.') {
        octet[idx++] = std::stoi(buf);
        buf = "";
      } else {
        buf.push_back(c);
      }
    }
  }


  IPv4Addr::IPv4Addr(std::vector<uint8_t> addr, uint8_t cidr = 32) : cidr{cidr} {
    if(!addr.size() != 4)
      throw std::invalid_argument("The number of IPv4 Address elements must be 4.");

    for(int i = 0; i < 4; i++)
      octet[i] = addr[i];
  }


  std::string IPv4Addr::to_string() const {
    std::string res;

    for(int i = 0; i < 4; i++) {
      if(i) res += ".";
      res += std::to_string(octet[i]);
    }
    res += "/";
    res += std::to_string(cidr);

    return res;
  }


  std::string IPv4Addr::to_bin_string() const {
    std::string res;

    for(int i = 3; i >= 0; i--) {
      uint8_t oc = octet[i];
      for(int j = 0; j < 8; j++) {
        res += '0' + !!(oc & 1);
        oc >>= 1;
      }
    }
    std::reverse(res.begin(), res.end());

    return res;
  }


  uint32_t IPv4Addr::to_32bit_integer() const {
    return ((uint32_t)octet[0] << 24) | ((uint32_t)octet[1] << 16) | ((uint32_t)octet[2] << 8) | (uint32_t)octet[3];
  }


  bool IPv4Addr::contain(const IPv4Addr &addr) const {
    uint32_t this_int = to_32bit_integer();
    uint32_t addr_int = addr.to_32bit_integer();

    return (this_int & addr_int) == this_int;
  }
}
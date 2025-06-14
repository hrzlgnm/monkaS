#include <ip/Address.hpp>

#include <algorithm>
#include <arpa/inet.h>
#include <ostream>

namespace monkas::ip
{
auto asLinuxAf(AddressFamily f) -> int
{
    switch (f)
    {
    case AddressFamily::IPv4:
        return AF_INET;
    case AddressFamily::IPv6:
        return AF_INET6;
    case AddressFamily::Unspecified:
    default:
        return AF_UNSPEC;
    }
}

auto operator<<(std::ostream &o, AddressFamily a) -> std::ostream &
{
    switch (a)
    {
    case AddressFamily::IPv4:
        o << "inet";
        break;
    case AddressFamily::IPv6:
        o << "inet6";
        break;
    case AddressFamily::Unspecified:
        o << "unspec";
        break;
    }
    return o;
}

Address::operator bool() const
{
    return m_addressFamily != AddressFamily::Unspecified;
}

auto Address::addressFamily() const -> AddressFamily
{
    return m_addressFamily;
}

auto Address::addressLength() const -> Address::size_type
{
    if (m_addressFamily == AddressFamily::IPv4)
    {
        return ipV4AddrLen;
    }
    if (m_addressFamily == AddressFamily::IPv6)
    {
        return ipV6AddrLen;
    }
    return 0;
}

auto Address::fromString(const std::string &address) -> Address
{
    std::array<uint8_t, ipV6AddrLen> addr{};
    if (inet_pton(AF_INET, address.data(), addr.data()) == 1)
    {
        return Address::fromBytes(addr.data(), ipV4AddrLen);
    }
    if (inet_pton(AF_INET6, address.data(), addr.data()) == 1)
    {
        return Address::fromBytes(addr.data(), ipV6AddrLen);
    }
    return {};
}

auto Address::fromBytes(const uint8_t *bytes, size_type len) -> Address
{
    if (len == ipV6AddrLen || len == ipV4AddrLen)
    {
        Address a;
        std::copy_n(bytes, len, a.begin());
        a.m_addressFamily = (len == ipV6AddrLen ? AddressFamily::IPv6 : AddressFamily::IPv4);
        return a;
    }
    return {};
}

auto Address::fromBytes(const std::array<uint8_t, ipV4AddrLen> &bytes) -> Address
{
    return fromBytes(bytes.data(), bytes.size());
}

auto Address::fromBytes(const std::array<uint8_t, ipV6AddrLen> &bytes) -> Address
{
    return fromBytes(bytes.data(), bytes.size());
}

auto Address::toString() const -> std::string
{
    std::array<char, INET6_ADDRSTRLEN> out{};
    if (inet_ntop(asLinuxAf(m_addressFamily), data(), out.data(), out.size()) != nullptr)
    {
        return {out.data()};
    }
    return {"Unspecified"};
}

auto operator<<(std::ostream &o, const Address &a) -> std::ostream &
{
    return o << a.toString();
}

auto operator<(const Address &lhs, const Address &rhs) -> bool
{
    if (lhs.addressFamily() == rhs.addressFamily())
    {
        const auto addressLength = lhs.addressLength();
        return std::lexicographical_compare(lhs.begin(), lhs.begin() + addressLength, rhs.begin(),
                                            rhs.begin() + addressLength);
    }
    return lhs.addressLength() < rhs.addressLength();
}

auto operator==(const Address &lhs, const Address &rhs) -> bool
{
    if (lhs.addressFamily() == rhs.addressFamily())
    {
        const auto addressLength = lhs.addressLength();
        return std::equal(lhs.begin(), lhs.begin() + addressLength, rhs.begin(), rhs.begin() + addressLength);
    }
    return false;
}

auto operator!=(const Address &lhs, const Address &rhs) -> bool
{
    return !(lhs == rhs);
}

} // namespace monkas::ip

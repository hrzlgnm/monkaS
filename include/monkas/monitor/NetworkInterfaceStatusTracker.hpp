#pragma once

#include <bitset>
#include <chrono>
#include <set>
#include <string>
#include <type_traits>

#include <ethernet/Address.hpp>
#include <fmt/ostream.h>
#include <network/Address.hpp>

namespace monkas::monitor
{

using Duration = std::chrono::duration<int64_t, std::milli>;
using Addresses = std::set<network::Address>;

// TODO: stats for nerds per interface descriptor
class NetworkInterfaceStatusTracker
{
  public:
    enum class OperationalState : uint8_t
    {
        Unknown,
        NotPresent,
        Down,
        LowerLayerDown,
        Testing,
        Dormant,
        Up,
    };
    enum class GatewayClearReason : uint8_t
    {
        LinkDown,
        RouteDeleted,
        AllIPv4AddressesRemoved,
    };

    enum class DirtyFlag : uint8_t
    {
        NameChanged,
        OperationalStateChanged,
        MacAddressChanged,
        BroadcastAddressChanged,
        GatewayAddressChanged,
        NetworkAddressesChanged,
        // NOTE: keep last
        FlagsCount,
    };

    using DirtyFlags = std::bitset<static_cast<std::underlying_type_t<DirtyFlag>>(DirtyFlag::FlagsCount)>;

    NetworkInterfaceStatusTracker();

    [[nodiscard]] auto name() const -> const std::string&;
    void setName(const std::string& name);

    [[nodiscard]] auto operationalState() const -> OperationalState;
    void setOperationalState(OperationalState operstate);

    [[nodiscard]] auto macAddress() const -> const ethernet::Address&;
    void setMacAddress(const ethernet::Address& address);

    [[nodiscard]] auto broadcastAddress() const -> const ethernet::Address&;
    void setBroadcastAddress(const ethernet::Address& address);

    [[nodiscard]] auto gatewayAddress() const -> const ip::Address&;
    void setGatewayAddress(const ip::Address& gateway);
    void clearGatewayAddress(GatewayClearReason r);

    [[nodiscard]] auto networkAddresses() const -> const Addresses&;

    void addNetworkAddress(const network::Address& address);
    void removeNetworkAddress(const network::Address& address);

    [[nodiscard]] auto age() const -> Duration;

    [[nodiscard]] auto hasName() const -> bool;

    [[nodiscard]] auto isDirty() const -> bool;
    [[nodiscard]] auto isDirty(DirtyFlag flag) const -> bool;
    [[nodiscard]] auto dirtyFlags() const -> DirtyFlags;
    void clearFlag(DirtyFlag flag);

  private:
    void touch(DirtyFlag flag);

    // TODO: only use public api
    friend auto operator<<(std::ostream& o, const NetworkInterfaceStatusTracker& s) -> std::ostream&;
    std::string m_name;
    ethernet::Address m_macAddress;
    ethernet::Address m_broadcastAddress;
    OperationalState m_operState {OperationalState::Unknown};
    Addresses m_networkAddresses;
    ip::Address m_gateway;
    std::chrono::time_point<std::chrono::steady_clock> m_lastChanged;
    DirtyFlags m_dirtyFlags;
};

using OperationalState = NetworkInterfaceStatusTracker::OperationalState;
auto operator<<(std::ostream& o, OperationalState op) -> std::ostream&;
using GatewayClearReason = NetworkInterfaceStatusTracker::GatewayClearReason;
auto operator<<(std::ostream& o, GatewayClearReason r) -> std::ostream&;
using DirtyFlag = NetworkInterfaceStatusTracker::DirtyFlag;
using DirtyFlags = NetworkInterfaceStatusTracker::DirtyFlags;
auto operator<<(std::ostream& o, DirtyFlag d) -> std::ostream&;
auto operator<<(std::ostream& o, const DirtyFlags& d) -> std::ostream&;

}  // namespace monkas::monitor

template<>
struct fmt::formatter<monkas::monitor::NetworkInterfaceStatusTracker> : fmt::ostream_formatter
{
};

template<>
struct fmt::formatter<monkas::monitor::OperationalState> : fmt::ostream_formatter
{
};

template<>
struct fmt::formatter<monkas::monitor::GatewayClearReason> : fmt::ostream_formatter
{
};

template<>
struct fmt::formatter<monkas::monitor::DirtyFlag> : fmt::ostream_formatter
{
};

template<>
struct fmt::formatter<monkas::monitor::DirtyFlags> : fmt::ostream_formatter
{
};

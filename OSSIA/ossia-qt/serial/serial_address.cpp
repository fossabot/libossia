// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia-qt/serial/serial_address.hpp>
#include <ossia-qt/serial/serial_node.hpp>

namespace ossia
{
namespace net
{

serial_address::serial_address(
    const serial_address_data& p, serial_node& parent)
    : generic_address{p, parent}, mData{p}
{
}

void serial_address::valueCallback(const ossia::value& val)
{
  this->set_value(val);
  send(m_value);
}
}
}

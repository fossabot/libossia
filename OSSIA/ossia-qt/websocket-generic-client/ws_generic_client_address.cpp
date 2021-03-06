// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia-qt/websocket-generic-client/ws_generic_client_address.hpp>

namespace ossia
{
namespace net
{

ws_generic_client_address::ws_generic_client_address(
    const ws_generic_client_address_data& data,
    ossia::net::node_base& node_base)
    : generic_address{data, node_base}, mData{data}
{
}

ws_generic_client_address::~ws_generic_client_address()
{
  callback_container<value_callback>::callbacks_clear();
}
}
}

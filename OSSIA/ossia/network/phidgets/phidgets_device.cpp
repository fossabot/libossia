// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "phidgets_device.hpp"
#include "phidgets_address.hpp"
#include "phidgets_node.hpp"
#include "phidgets_protocol.hpp"
namespace ossia
{

phidget_device::phidget_device(std::string n)
    : ossia::net::device_base{std::make_unique<ossia::phidget_protocol>()}
    , phidget_node{*this, *this}
    , m_name{std::move(n)}
{
  m_protocol->set_device(*this);
  this->m_capabilities.change_tree = true;
}

phidget_device::~phidget_device()
{
}

net::node_base* phidget_device::get_parent() const
{
  return nullptr;
}

const net::node_base& phidget_device::get_root_node() const
{
  return *this;
}

net::node_base& phidget_device::get_root_node()
{
  return *this;
}

phidget_protocol& phidget_device::get_protocol() const
{
  return static_cast<phidget_protocol&>(device_base::get_protocol());
}
}

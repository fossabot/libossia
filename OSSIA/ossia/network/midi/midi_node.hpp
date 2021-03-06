#pragma once

#include <ossia/network/base/node.hpp>

namespace ossia
{
namespace net
{
namespace midi
{
class midi_protocol;
class midi_device;
class OSSIA_EXPORT midi_node : public ossia::net::node_base
{
protected:
  midi_device& m_device;
  node_base& m_parent;
  std::unique_ptr<address_base> m_address;

public:
  using iterator = ossia::ptr_container<ossia::net::node_base>::iterator;
  using const_iterator
      = ossia::ptr_container<ossia::net::node_base>::const_iterator;
  ~midi_node();
  midi_node(midi_device& aDevice, ossia::net::node_base& aParent);

  device_base& get_device() const final override;
  node_base* get_parent() const final override;

  node_base& set_name(std::string) override;

  address_base* get_address() const final override;
  address_base* create_address(val_type) final override;
  bool remove_address() final override;

  std::unique_ptr<node_base>
  make_child(const std::string& name) final override;
  void removing_child(node_base& node_base) final override;
};
}
}
}

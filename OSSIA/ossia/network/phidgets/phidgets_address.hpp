#pragma once
#include <ossia/network/base/address.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia/network/domain/domain.hpp>
#include <ossia/network/phidgets/detail/phidgetspp.hpp>
namespace ossia
{
class phidget_protocol;
class phidget_address final : public ossia::net::address_base
{
  ossia::net::node_base& m_parent;
  phidget_protocol& m_protocol;
  ppp::phidget_ptr m_phidget;
  ppp::interface_kit* m_ik{};
  ossia::domain m_domain;

public:
  phidget_address(
      ppp::phidget_ptr p, phidget_protocol& proto, ossia::net::node_base& par);
  ~phidget_address();

  net::node_base& get_node() const override;
  ppp::phidget_ptr phidget() const
  {
    return m_phidget;
  }

  void pull_value() override;
  ossia::value value() const override;
  net::address_base& push_value(const ossia::value&) override;
  net::address_base& push_value(ossia::value&&) override;
  net::address_base& push_value() override;
  net::address_base& set_value(const ossia::value&) override;
  net::address_base& set_value(ossia::value&&) override;
  val_type get_value_type() const override;
  net::address_base& set_value_type(val_type) override;
  access_mode get_access() const override;
  net::address_base& set_access(access_mode) override;
  const domain& get_domain() const override;
  net::address_base& set_domain(const domain&) override;
  bounding_mode get_bounding() const override;
  net::address_base& set_bounding(bounding_mode) override;
  repetition_filter get_repetition_filter() const override;
  net::address_base& set_repetition_filter(repetition_filter) override;
};
}

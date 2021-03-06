#pragma once
#include <ossia/detail/any_map.hpp>
#include <ossia/editor/dataspace/dataspace.hpp>
#include <ossia/editor/value/value.hpp>
#include <ossia/network/common/complex_type.hpp>
#include <ossia/network/common/extended_types.hpp>
#include <ossia/network/domain/domain.hpp>

namespace ossia
{
namespace net
{
/**
 * @brief The data that can be found inside an address
 *
 * Used to create a new address / node.
 * Unlike address_base and its child classes, can be copied, moved, etc.
 *
 * Name is the name of the node only.
 */
struct address_data
{
  address_data() = default;
  address_data(const address_data&) = default;
  address_data(address_data&&) = default;
  address_data& operator=(const address_data&) = default;
  address_data& operator=(address_data&&) = default;

  address_data(std::string n) : name{std::move(n)}
  {
  }

  std::string name;
  ossia::value value;
  ossia::complex_type type;
  ossia::optional<ossia::domain> domain;
  ossia::optional<ossia::access_mode> access;
  ossia::optional<ossia::bounding_mode> bounding;
  ossia::optional<ossia::repetition_filter> rep_filter;
  ossia::optional<bool> muted{};
  ossia::optional<bool> critical{};
  unit_t unit;

  extended_attributes extended;

  operator const ossia::extended_attributes&() const
  {
    return extended;
  }
  operator ossia::extended_attributes&()
  {
    return extended;
  }
};

/**
 * @brief Full information about an address.
 *
 * address is an osc-like address, e.g.
 *
 * `/foo/bar/baz.3/fib`
 *
 * in address_data, name would be `fib`
 */
struct full_address_data
{
  public:
    full_address_data() = default;
    full_address_data(const full_address_data&) = default;
    full_address_data(full_address_data&&) = default;
    full_address_data& operator=(const full_address_data&) = default;
    full_address_data& operator=(full_address_data&&) = default;

    full_address_data(std::string n) : address{std::move(n)}
    {
    }

    std::string address;

  private:
    ossia::value val;
    ossia::value previous_val;

  public:
    ossia::val_type type{};
    ossia::domain domain;
    ossia::access_mode access{};
    ossia::bounding_mode bounding{};
    ossia::repetition_filter rep_filter{};
    ossia::unit_t unit;
    bool muted{};
    bool critical{};

    extended_attributes extended;

    operator const ossia::extended_attributes&() const
    {
      return extended;
    }
    operator ossia::extended_attributes&()
    {
      return extended;
    }

    void set_value(const ossia::value& v) {
      previous_val = std::move(val);
      val = v;
    }
    void set_value(ossia::value&& v) {
      previous_val = std::move(val);
      val = std::move(v);
    }

    bool filter_repetition(const ossia::value& v) const
    {
      return get_repetition_filter() == ossia::repetition_filter::ON
             && v == previous_val;
    }


    const ossia::value& value() const { return this->val; }
    ossia::val_type get_value_type() const { return type; }
    access_mode get_access() const { return access; }
    const ossia::domain& get_domain() const { return domain; }
    bounding_mode get_bounding() const { return bounding; }
    repetition_filter get_repetition_filter() const { return rep_filter; }
    const ossia::unit_t& get_unit() const { return unit; }
    bool get_muted() const { return muted; }
    bool get_critical() const { return critical; }

};

OSSIA_EXPORT
inline const std::string& osc_address_string(const ossia::net::full_address_data& d)
{
  return d.address;
}
}
}

#pragma once
#include <ossia/editor/dataspace/dataspace.hpp>
#include <ossia/editor/dataspace/dataspace_base.hpp>
#include <ossia/editor/value/value.hpp>
#include <brigand/algorithms/fold.hpp>
namespace ossia
{

#include <ossia/editor/dataspace/dataspace_strong_variants.hpp>
struct OSSIA_EXPORT value_with_unit final
{
  strong_value_variant v;
  value_with_unit() = default;
  value_with_unit(const value_with_unit& other) = default;
  value_with_unit(value_with_unit&& other) = default;
  value_with_unit& operator=(const value_with_unit& other) = default;
  value_with_unit& operator=(value_with_unit&& other) = default;
  template <typename T>
  value_with_unit(const T& arg) noexcept : v(arg)
  {
  }

  auto which() const
  {
    return v.which();
  }
  operator bool() const
  {
    return bool(v);
  }

  template <typename T>
  const T* target() const noexcept
  {
    return v.target<T>();
  }

  template <typename T>
  T* target() noexcept
  {
    return v.target<T>();
  }
};
}

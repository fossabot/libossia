#pragma once

#include <memory>

#include <ossia/editor/expression/expression_fwd.hpp>
#include <ossia/editor/value/value.hpp>
#include <ossia/network/base/address.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia_export.h>

namespace ossia
{
class Destination;

namespace expressions
{
/**
 * @brief expression_pulse : becomes true when a message happens
 * @see expression.hpp
 */
class OSSIA_EXPORT expression_pulse final
    : public expression_callback_container
{
public:
  expression_pulse(const Destination& destination);
  expression_pulse(expression_pulse&& other) = delete;
  expression_pulse& operator=(expression_pulse&& other) = delete;
  expression_pulse(const expression_pulse& other) = delete;
  expression_pulse& operator=(const expression_pulse& other) = delete;

  virtual ~expression_pulse();

  /*! evaluate if a destination value change have been observed since
   expression
   creation or since the last update
   \return true after the destination being updated */
  bool evaluate() const;

  /*! reset the evaluation to false until the next destination update */
  void update() const;

  const Destination& get_destination() const;

private:
  void on_first_callback_added() override;
  void on_removing_last_callback() override;

  void destination_callback(const value& value);

  Destination m_destination;

  net::address_base::callback_index m_callback;
  mutable bool m_result{};
};
}
}

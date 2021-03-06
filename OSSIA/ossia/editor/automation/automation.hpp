#pragma once
#include <ossia/detail/optional.hpp>
#include <ossia/detail/ptr_container.hpp>
#include <ossia/editor/curve/behavior.hpp>
#include <ossia/editor/scenario/time_process.hpp>
#include <ossia/editor/state/state_element.hpp>
#include <ossia/editor/value/destination.hpp>
#include <ossia_export.h>

namespace ossia
{
class value;
struct time_value;
namespace net
{
class address_base;
}

/**
 * \brief The automation class
 *
 * An automation is a time process that represents a curve of one
 * or multiple parameters.
 *
 * It drives (i.e. sends messages) to an address, according to a driving value.
 * Its driven \ref net::address_base can be either of numeric type (\ref
 * val_type::INT, \ref val_type::FLOAT...)
 * or of \ref val_type::TUPLE, if the tuple only has numeric elements (e.g.
 * Tuple{Float, Float}).
 *
 * The driving \ref value can either be a single \ref Behavior or a \ref Tuple
 * of \ref Behavior,
 * in accordance to the type of the driven \ref net::address_base.
 *
 * The automation has a "source" domain, i.e. the data space in which the
 * transformation
 * happens. The target domain is taken from the driven address_base.
 * The unit is stored in m_lastMessage.unit.
 *
 *
 * \see \ref Behavior \ref curve \ref curve_segment
 */
class OSSIA_EXPORT automation final : public ossia::time_process
{
public:
  automation();
  automation(Destination, const ossia::behavior&);
  automation(Destination, ossia::behavior&&);

  ~automation();

  void set_destination(Destination d);
  void set_behavior(ossia::behavior b);

  void clean();

private:
  ossia::state_element offset(ossia::time_value, double pos) override;
  ossia::state_element state(ossia::time_value date, double pos) override;

  void start(ossia::state& st) override;
  void stop() override;
  void pause() override;
  void resume() override;

  static ossia::value
  compute_value(double, ossia::val_type drivenType, const ossia::behavior&);

  void update_message(double t);

  ossia::behavior m_drive;
  optional<ossia::message> m_lastMessage;
  ossia::val_type m_drivenType{};
};
}

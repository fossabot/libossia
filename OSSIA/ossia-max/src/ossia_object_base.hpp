#pragma once
#include "ossia-max.hpp"
#include <ossia/detail/safe_vec.hpp>
#include <ossia/ossia.hpp>

namespace ossia
{
namespace max
{

#pragma mark -
#pragma mark t_object_base structure declaration

enum class Type
{
  root = 0,
  param,
  remote,
  model,
  view,
  device,
  client
};

enum class AddrType
{
  relative = 0,
  absolute,
  global
};

struct t_object_base
{
  t_object m_object; // the Max object instance.
  // !!! this member is handled by Max API : that's why there is no place in
  // our code where it is initialized.

  void** m_inlets{};
  void* m_data_out{};
  void* m_set_out{};
  void* m_dump_out{};

  ossia::net::node_base* m_node{};

  Type m_otype{};
  t_symbol* m_name{};
  AddrType m_address_type{};
  bool m_dead = false; // wether this object is being deleted or not;
                       //            t_canvas*   m_last_opened_canvas{};

  void* m_clock{};
  void* m_regclock{};   // registration clock
  void* m_unregclock{}; // unregistration clock

  void apply_value_visitor(const ossia::value& val);

  static void push(t_object_base* x, t_symbol*, int argc, t_atom* argv);
  static void bang(t_object_base* x);
  static void defer_set_output(t_object_base*x, t_symbol*s ,int argc, t_atom* argv);
  //            static void tick(t_object_base* x);
  static void relative_namespace(t_object_base* x);
};

#pragma mark -
#pragma mark Utilities

// Converts a max string to a type used in the api
static ossia::val_type name_to_type(ossia::string_view name)
{
  if (name == "integer")
    return ossia::val_type::INT;
  if (name == "float")
    return ossia::val_type::FLOAT;
  if (name == "numeric")
    return ossia::val_type::FLOAT;
  if (name == "array")
    return ossia::val_type::TUPLE;
  if (name == "impulse")
    return ossia::val_type::IMPULSE;
  if (name == "bool")
    return ossia::val_type::BOOL;
  if (name == "boolean")
    return ossia::val_type::BOOL;
  if (name == "string")
    return ossia::val_type::STRING;
  if (name == "symbol")
    return ossia::val_type::STRING;
  if (name == "vec2f")
    return ossia::val_type::VEC2F;
  if (name == "vec3f")
    return ossia::val_type::VEC3F;
  if (name == "vec4f")
    return ossia::val_type::VEC4F;
  if (name == "char")
    return ossia::val_type::CHAR;

  return ossia::val_type::FLOAT;
}

// Typed function switcher to convert ossia::value to t_atom
struct value2atom
{
  std::vector<t_atom> operator()(impulse) const
  {
    t_atom a;

    atom_setsym(&a, gensym("bang"));

    std::vector<t_atom> va;

    va.push_back(a);

    return va;
  }

  std::vector<t_atom> operator()(int32_t i) const
  {
    t_atom a;

    atom_setfloat(&a, i);

    std::vector<t_atom> va;

    va.push_back(a);

    return va;
  }

  std::vector<t_atom> operator()(float f) const
  {
    t_atom a;
    atom_setfloat(&a, f);

    std::vector<t_atom> va;

    va.push_back(a);

    return va;
  }

  std::vector<t_atom> operator()(bool b) const
  {
    t_atom a;
    float f = b ? 1. : 0.;

    atom_setfloat(&a, f);

    std::vector<t_atom> va;
    va.push_back(a);

    return va;
  }

  std::vector<t_atom> operator()(const std::string& str) const
  {
    t_symbol* s = gensym(str.c_str());
    t_atom a;

    atom_setsym(&a, s);
    std::vector<t_atom> va;
    va.push_back(a);

    return va;
  }

  std::vector<t_atom> operator()(char c) const
  {
    std::vector<t_atom> va;
    t_atom a;

    atom_setfloat(&a, (float)c);
    va.push_back(a);

    return va;
  }

  std::vector<t_atom> operator()(vec2f vec) const
  {
    t_atom a[2];

    atom_setfloat(a, vec[0]);
    atom_setfloat(a + 1, vec[1]);

    std::vector<t_atom> va;

    va.push_back(a[0]);
    va.push_back(a[1]);

    return va;
  }

  std::vector<t_atom> operator()(vec3f vec) const
  {
    t_atom a[3];

    atom_setfloat(a, vec[0]);
    atom_setfloat(a + 1, vec[1]);
    atom_setfloat(a + 2, vec[2]);

    std::vector<t_atom> va;

    va.push_back(a[0]);
    va.push_back(a[1]);
    va.push_back(a[2]);

    return va;
  }

  std::vector<t_atom> operator()(vec4f vec) const
  {
    t_atom a[4];

    atom_setfloat(a, vec[0]);
    atom_setfloat(a + 1, vec[1]);
    atom_setfloat(a + 2, vec[2]);
    atom_setfloat(a + 3, vec[3]);

    std::vector<t_atom> va;

    va.push_back(a[0]);
    va.push_back(a[1]);
    va.push_back(a[2]);

    return va;
  }

  std::vector<t_atom> operator()(const std::vector<ossia::value>& t) const
  {
    std::vector<t_atom> va;

    for (auto v : t)
    {
      std::vector<t_atom> b;
      value2atom vm;

      b = v.apply(vm);
      std::move(b.begin(), b.end(), std::back_inserter(va));
    }

    return va;
  }

  std::vector<t_atom> operator()() const
  {
    std::vector<t_atom> va;
    return va;
  }
};

// Template typed function switcher to convert t_atom or standard type into
// ossia::value
template <typename T>
struct value_visitor
{
  T* x;

  void operator()(impulse) const
  {
    outlet_bang(x->m_data_out);

    if (x->m_set_out)
      outlet_bang(x->m_set_out);
  }

  void operator()(int32_t i) const
  {
    t_atom a;
    atom_setlong(&a, i);
    outlet_int(x->m_data_out, i);

    if (x->m_set_out)
      defer_low((t_object*)x,(method)t_object_base::defer_set_output,
                gensym("set"),1,&a);
  }

  void operator()(float f) const
  {
    t_atom a;

    atom_setfloat(&a, f);
    outlet_float(x->m_data_out, f);

    if (x->m_set_out)
      defer_low((t_object*)x,(method)t_object_base::defer_set_output,
                gensym("set"),1,&a);
  }

  void operator()(bool b) const
  {
    t_atom a;
    int i = b ? 1 : 0;

    atom_setlong(&a, i);
    outlet_int(x->m_data_out, i);

    if (x->m_set_out)
      defer_low((t_object*)x,(method)t_object_base::defer_set_output,
                gensym("set"),1,&a);
  }

  void operator()(const std::string& str) const
  {
    t_symbol* s = gensym(str.c_str());
    t_atom a;

    atom_setsym(&a, s);
    outlet_anything(x->m_data_out, s, 0, nullptr);

    if (x->m_set_out)
      defer_low((t_object*)x,(method)t_object_base::defer_set_output,
                gensym("set"),1,&a);
  }

  void operator()(char c) const
  {
    t_atom a;

    atom_setlong(&a, (long)c);
    outlet_int(x->m_data_out, (long)c);

    if (x->m_set_out)
      defer_low((t_object*)x,(method)t_object_base::defer_set_output,
                gensym("set"),1,&a);
  }

  void operator()(vec2f vec) const
  {
    t_atom a[2];

    atom_setfloat(a, vec[0]);
    atom_setfloat(a + 1, vec[1]);
    outlet_list(x->m_data_out, gensym("list"), 2, a);

    if (x->m_set_out)
      defer_low((t_object*)x,(method)t_object_base::defer_set_output,
                gensym("set"),2,a);
  }

  void operator()(vec3f vec) const
  {
    t_atom a[3];

    atom_setfloat(a, vec[0]);
    atom_setfloat(a + 1, vec[1]);
    atom_setfloat(a + 2, vec[2]);
    outlet_list(x->m_data_out, gensym("list"), 3, a);

    if (x->m_set_out)
      defer_low((t_object*)x,(method)t_object_base::defer_set_output,
                gensym("set"),3,a);
  }

  void operator()(vec4f vec) const
  {
    t_atom a[4];

    atom_setfloat(a, vec[0]);
    atom_setfloat(a + 1, vec[1]);
    atom_setfloat(a + 2, vec[2]);
    atom_setfloat(a + 3, vec[3]);
    outlet_list(x->m_data_out, gensym("list"), 4, a);

    if (x->m_set_out)
      defer_low((t_object*)x,(method)t_object_base::defer_set_output,
                gensym("set"),4,a);
  }

  void operator()(const std::vector<ossia::value>& t) const
  {
    std::vector<t_atom> va;

    for (auto v : t)
    {
      std::vector<t_atom> b;
      value2atom vm;
      b = v.apply(vm);
      std::move(b.begin(), b.end(), std::back_inserter(va));
    }
    t_atom* list_ptr = !va.empty() ? va.data() : nullptr;
    outlet_list(x->m_data_out, gensym("list"), va.size(), list_ptr);

    if (x->m_set_out)
      defer_low((t_object*)x,(method)t_object_base::defer_set_output,
                gensym("set"),va.size(), list_ptr);
  }

  void operator()() const
  {
    object_error(
        (t_object*)x, "%s receive an invalid data", x->m_name->s_name);
  }
};

} // max namespace
} // ossia namespace

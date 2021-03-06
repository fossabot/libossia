// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "client.hpp"
#include "device.hpp"
#include "model.hpp"
#include "parameter.hpp"
#include "remote.hpp"
#include "view.hpp"
#include "utils.hpp"

#include "ossia/network/osc/osc.hpp"
#include <ossia/network/oscquery/oscquery_mirror.hpp>

#include <functional>
#include <iostream>
#include <memory>

using namespace ossia::max;

#pragma mark -
#pragma mark ossia_client class methods

extern "C" void ossia_client_setup()
{
  auto& ossia_library = ossia_max::instance();

  // instantiate the ossia.client class
  ossia_library.ossia_client_class = class_new(
      "ossia.client", (method)ossia_client_new, (method)ossia_client_free,
      (short)sizeof(t_client), 0L, A_GIMME, 0);

  class_addmethod(
      ossia_library.ossia_client_class, (method)t_client::register_children,
      "register", A_NOTHING, 0);
  class_addmethod(
      ossia_library.ossia_client_class, (method)t_client::loadbang, "loadbang",
      A_NOTHING, 0);
  class_addmethod(
      ossia_library.ossia_client_class, (method)t_object_base::relative_namespace,
              "namespace", A_NOTHING, 0);
  class_addmethod(
      ossia_library.ossia_client_class, (method)ossia_client_connect,
      "connect", A_GIMME, 0);
  class_addmethod(
      ossia_library.ossia_client_class,
      (method)protocol_settings::print_protocol_help, "help", A_NOTHING, 0);

  class_register(CLASS_BOX, ossia_library.ossia_client_class);

}

extern "C" void* ossia_client_new(t_symbol* name, long argc, t_atom* argv)
{
  auto& ossia_library = ossia_max::instance();
  t_client* x = (t_client*)object_alloc(ossia_library.ossia_client_class);

  if (x)
  {
    // make outlets
    x->m_dump_out
        = outlet_new(x, NULL); // anything outlet to dump client state

    x->m_device = 0;
    x->m_node = 0;

    x->m_otype = Type::client;

    if (ossia::max::find_peer(x))
    {
      error("You can have only one [ossia.device] or [ossia.client] per patcher.");
      ossia_client_free(x);
      return nullptr;
    }

    // parse arguments
    long attrstart = attr_args_offset(argc, argv);

    // check name argument
    x->m_name = gensym("Max");
    if (attrstart && argv)
    {
      if (atom_gettype(argv) == A_SYM)
      {
        x->m_name = atom_getsym(argv);
      }
    }

    // process attr args, if any
    attr_args_process(x, argc - attrstart, argv + attrstart);

    ossia_library.clients.push_back(x);
  }

  return (x);
}

extern "C" void ossia_client_free(t_client* x)
{
  x->m_dead = true;
  x->unregister_children();

  if (x->m_device)
    delete (x->m_device);
  x->m_device = nullptr;
  outlet_delete(x->m_dump_out);
  ossia_max::instance().clients.remove_all(x);
  register_quarantinized();
}

static void dump_child(t_client* x, const ossia::net::node_base& node)
{
  for (const auto& child : node.children())
  {
    std::stringstream ss;
    auto parent = child->get_parent();
    while (parent != nullptr)
    {
      ss << "\t";
      parent = parent->get_parent();
    }
    ss << child->get_name();

    t_atom a;
    atom_setsym(&a, gensym(ss.str().c_str()));
    outlet_anything(x->m_dump_out, gensym("child"), 1, &a);
    dump_child(x, *child);
  }
}

extern "C" void ossia_client_dump(t_client* x)
{
  dump_child(x, x->m_device->get_root_node());
}

extern "C" void
ossia_client_connect(t_client* x, t_symbol*, int argc, t_atom* argv)
{
  ossia_client_free(x); // unregister and delete m_device

  if (argc && argv->a_type == A_SYM)
  {
    std::string protocol = atom_getsym(argv)->s_name;

    if (protocol == "Minuit")
    {
      std::string device_name = "Max";
      protocol_settings::minuit settings{};

      argc--;
      argv++;

      if (argc == 4 && argv[0].a_type == A_SYM && argv[1].a_type == A_SYM
          && argv[2].a_type == A_LONG && argv[3].a_type == A_LONG)
      {
        device_name = atom_getsym(argv++)->s_name;
        settings.remoteip = atom_getsym(argv++)->s_name;
        settings.remoteport = atom_getfloat(argv++);
        settings.localport = atom_getfloat(argv++);
      }

      try
      {
        x->m_device = new ossia::net::generic_device{
            std::make_unique<ossia::net::minuit_protocol>(
                device_name, settings.remoteip, settings.remoteport,
                settings.localport),
            x->m_name->s_name};
      }
      catch (const std::exception& e)
      {
        object_error((t_object*)x, "%s", e.what());
        return;
      }

      object_post(
          (t_object*)x,
          "Connected with Minuit protocol to %s on port %u and listening on "
          "port %u",
          settings.remoteip.c_str(), settings.remoteport, settings.localport);
    }
    else if (protocol == "oscquery")
    {
      std::string wsurl;

      argc--;
      argv++;

      if (argc == 1 && argv[0].a_type == A_SYM)
      {
        wsurl = atom_getsym(argv)->s_name;
      }

      try
      {
        auto protocol = new ossia::oscquery::oscquery_mirror_protocol{wsurl};
        x->m_device = new ossia::net::generic_device{
            std::unique_ptr<ossia::net::protocol_base>(protocol), "Max"};
      }
      catch (const std::exception& e)
      {
        object_error((t_object*)x, "%s", e.what());
        return;
      }

      object_post(
          (t_object*)x, "Connected with oscquery protocol to %s",
          wsurl.c_str());
    }
    else
    {
      object_error((t_object*)x, "Unknown protocol: %s", protocol.c_str());
      return;
    }
  }
  else
  {
    protocol_settings::print_protocol_help();
    return;
  }

  if (x->m_device)
  {
    x->m_device->get_protocol().update(*x->m_device);
    x->m_node = &x->m_device->get_root_node();

    t_client::register_children(x);
    t_client::explore(x->m_device->get_root_node());

    for (auto& child : x->m_device->children())
    {
      std::cout << child->get_name() << std::endl;
    }
  }
}

namespace ossia
{
namespace max
{

#pragma mark -
#pragma mark t_client structure functions

void t_client::register_children(t_client* x)
{
  std::vector<t_object_base*> children_view = find_children_to_register(
      &x->m_object, get_patcher(&x->m_object), gensym("ossia.view"));

  for (auto child : children_view)
  {
    if (child->m_otype == Type::view)
    {
      t_view* view = (t_view*)child;
      view->register_node(x->m_node);
    }
    else if (child->m_otype == Type::remote)
    {
      t_remote* remote = (t_remote*)child;
      remote->register_node(x->m_node);
    }
  }
}

void t_client::unregister_children()
{
  std::vector<t_object_base*> children_model = find_children_to_register(
      &m_object, get_patcher(&m_object), gensym("ossia.model"));

  for (auto child : children_model)
  {
    if (child->m_otype == Type::model)
    {
      t_model* model = (t_model*)child;
      model->unregister();
    }
    else if (child->m_otype == Type::param)
    {
      t_parameter* parameter = (t_parameter*)child;
      parameter->unregister();
    }
  }

  std::vector<t_object_base*> children_view = find_children_to_register(
      &m_object, get_patcher(&m_object), gensym("ossia.view"));

  for (auto child : children_view)
  {
    if (child->m_otype == Type::view)
    {
      t_view* view = (t_view*)child;
      view->unregister();
    }
    else if (child->m_otype == Type::remote)
    {
      t_remote* remote = (t_remote*)child;
      remote->unregister();
    }
  }
}

void t_client::loadbang(t_client* x)
{
  // if (LB_LOAD == (int)type)
  register_children(x);
}

void t_client::explore(const ossia::net::node_base& node)
{
  for (const auto& child : node.children_copy())
  {
    if (auto addr = child->get_address())
    {
      // attach to callback to display value update
      addr->add_callback([=](const value& v) {
        std::cerr << "[message] " << osc_address_string(*addr) << " <- "
                  << value_to_pretty_string(v) << std::endl;
      });

      // update the value
      addr->pull_value();
    }

    /*
     fmt::MemoryWriter w;
     w << *child;
     std::cerr << w.str() << "\n";
     */
    std::cout << child->get_name() << std::endl;

    explore(*child);
  }
}

} // max namespace
} // ossia namespace

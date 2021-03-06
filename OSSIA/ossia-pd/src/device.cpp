// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "device.hpp"
#include "model.hpp"
#include "parameter.hpp"
#include "remote.hpp"
#include "view.hpp"
#include "utils.hpp"

#include <ossia/network/osc/osc.hpp>
#include <ossia/network/oscquery/oscquery_server.hpp>

namespace ossia
{
namespace pd
{

static void device_free(t_device* x)
{
  x->x_dead = true;
  x->unregister_children();
  if (x->x_device)
    delete (x->x_device);
  ossia_pd::instance().devices.remove_all(x);
  outlet_free(x->x_dumpout);
  register_quarantinized();
}

static void* device_new(t_symbol* name, int argc, t_atom* argv)
{
  auto& ossia_pd = ossia_pd::instance();
  t_device* x = (t_device*)eobj_new(ossia_pd.device);
  // TODO SANITIZE : Direct leak
  t_binbuf* d = binbuf_via_atoms(argc, argv);

  if (x && d)
  {
    ossia_pd.devices.push_back(x);
    x->x_otype = Type::device;

    x->x_name = gensym("Pd");
    x->x_dumpout = outlet_new((t_object*)x, gensym("dumpout"));

    if (argc != 0 && argv[0].a_type == A_SYMBOL)
    {
      x->x_name = atom_getsymbol(argv);
    }

    auto local_proto_ptr = std::make_unique<ossia::net::local_protocol>();
    x->x_device = new ossia::net::generic_device{std::move(local_proto_ptr),
                                                 x->x_name->s_name};
    x->x_node = &x->x_device->get_root_node();

    ebox_attrprocess_viabinbuf(x, d);

    if (find_peer(x))
    {
      error(
            "Only one [ø.device]/[ø.client] instance per patcher is allowed.");
      device_free(x);
      x = nullptr;
    }
  }

  return (x);
}

void t_device::loadbang(t_device* x, t_float type)
{
  if (LB_LOAD == (int)type)
  {
    register_children(x);
  }
}

void t_device::register_children(t_device* x)
{

  std::vector<t_obj_base*> modelnodes
      = find_child_to_register(x, x->x_obj.o_canvas->gl_list, "ossia.model");
  for (auto v : modelnodes)
  {
    if (v->x_otype == Type::model)
    {
      t_model* model = (t_model*)v;
      model->register_node(x->x_node);
    }
    else if (v->x_otype == Type::param)
    {
      t_param* param = (t_param*)v;
      param->register_node(x->x_node);
    }
  }

  std::vector<t_obj_base*> viewnodes
      = find_child_to_register(x, x->x_obj.o_canvas->gl_list, "ossia.view");
  for (auto v : viewnodes)
  {
    if (v->x_otype == Type::view)
    {
      t_view* view = (t_view*)v;
      view->register_node(x->x_node);
    }
    else if (v->x_otype == Type::remote)
    {
      t_remote* remote = (t_remote*)v;
      remote->register_node(x->x_node);
    }
  }
}

void t_device::unregister_children()
{
  std::vector<t_obj_base*> node
      = find_child_to_register(this, x_obj.o_canvas->gl_list, "ossia.model");
  for (auto v : node)
  {
    if (v->x_otype == Type::model)
    {
      t_model* model = (t_model*)v;
      model->unregister();
    }
    else if (v->x_otype == Type::param)
    {
      t_param* param = (t_param*)v;
      param->unregister();
    }
  }

  std::vector<t_obj_base*> viewnode
      = find_child_to_register(this, x_obj.o_canvas->gl_list, "ossia.view");
  for (auto v : viewnode)
  {
    if (v->x_otype == Type::view)
    {
      t_view* view = (t_view*)v;
      view->unregister();
    }
    else if (v->x_otype == Type::remote)
    {
      t_remote* remote = (t_remote*)v;
      remote->unregister();
    }
  }
}

void device_expose(t_device* x, t_symbol*, int argc, t_atom* argv)
{

  if (argc && argv->a_type == A_SYMBOL)
  {
    auto& proto = static_cast<ossia::net::local_protocol&>(
        x->x_device->get_protocol());
    std::string protocol = argv->a_w.w_symbol->s_name;
    if (protocol == "Minuit")
    {
      Protocol_Settings::minuit settings{};
      argc--;
      argv++;
      if (argc == 3 && argv[0].a_type == A_SYMBOL && argv[1].a_type == A_FLOAT
          && argv[2].a_type == A_FLOAT)
      {
        settings.remoteip = atom_getsymbol(argv++)->s_name;
        settings.remoteport = atom_getfloat(argv++);
        settings.localport = atom_getfloat(argv++);
      }

      try
      {
        proto.expose_to(std::make_unique<ossia::net::minuit_protocol>(
            x->x_name->s_name, settings.remoteip, settings.remoteport,
            settings.localport));
      }
      catch (const std::exception& e)
      {
        pd_error(x, "%s", e.what());
        return;
      }
      logpost(
          x, 3,
          "New 'Minuit' protocol connected to %s on port %u and listening on "
          "port %u",
          settings.remoteip.c_str(), settings.remoteport, settings.localport);
    }
    else if (protocol == "oscquery")
    {
      Protocol_Settings::oscquery settings{};
      argc--;
      argv++;
      if (argc == 2 && argv[0].a_type == A_FLOAT && argv[1].a_type == A_FLOAT)
      {
        settings.oscport = atom_getfloat(argv++);
        settings.wsport = atom_getfloat(argv++);
      }

      try
      {
        proto.expose_to(
            std::make_unique<ossia::oscquery::oscquery_server_protocol>(
                settings.oscport, settings.wsport));
      }
      catch (const std::exception& e)
      {
        pd_error(x, "%s", e.what());
        return;
      }
      logpost(
          x, 3,
          "New 'oscquery' protocol with OSC port %u and WS port %u, listening "
          "on port %u",
          settings.oscport, settings.wsport, settings.oscport);
    }
    else if (protocol == "osc")
    {
      Protocol_Settings::osc settings{};
      argc--;
      argv++;
      if (argc == 3 && argv[0].a_type == A_SYMBOL && argv[1].a_type == A_FLOAT
          && argv[2].a_type == A_FLOAT)
      {
        settings.remoteip = atom_getsymbol(argv)->s_name;
        settings.remoteport = atom_getfloat(argv++);
        settings.localport = atom_getfloat(argv++);
      }

      try
      {
        proto.expose_to(std::make_unique<ossia::net::osc_protocol>(
            settings.remoteip, settings.remoteport, settings.localport));
      }
      catch (const std::exception& e)
      {
        pd_error(x, "%s", e.what());
        return;
      }
      logpost(
          x, 3,
          "New 'OSC' protocol connect to %s on port %u and listening on port "
          "%u",
          settings.remoteip.c_str(), settings.remoteport, settings.localport);
    }
    else
    {
      pd_error((t_object*)x, "Unknown protocol: %s", protocol.c_str());
    }
  }
  else
    Protocol_Settings::print_protocol_help();
}

void device_name(t_device *x, t_symbol* s, int argc, t_atom* argv){
  if( argc == 0 )
  {
    t_atom a;
    SETSYMBOL(&a,gensym(x->x_device->get_name().c_str()));
    outlet_anything(x->x_dumpout,gensym("name"),1,&a);
  } else if ( argv[0].a_type == A_SYMBOL ) {
    t_symbol* name = argv[0].a_w.w_symbol;
    x->x_device->set_name(name->s_name);
  } else {
    pd_error(x,"bad argument to message 'name'");
  }
}

extern "C" void setup_ossia0x2edevice(void)
{
  t_eclass* c = eclass_new(
      "ossia.device", (method)device_new, (method)device_free,
      (short)sizeof(t_device), CLASS_DEFAULT, A_GIMME, 0);

  if (c)
  {
    class_addcreator((t_newmethod)device_new,gensym("ø.device"), A_GIMME, 0);

      // TODO delete register method (only for debugging purpose)
    eclass_addmethod(
          c, (method)t_device::register_children,"register", A_NULL, 0);
    eclass_addmethod(c, (method)t_device::loadbang, "loadbang", A_NULL, 0);
    eclass_addmethod(c, (method)obj_namespace, "namespace", A_NULL, 0);
    eclass_addmethod(c, (method)device_expose, "expose", A_GIMME, 0);
    eclass_addmethod(
          c, (method)Protocol_Settings::print_protocol_help, "help", A_NULL, 0);
    eclass_addmethod(c, (method)device_name, "name", A_GIMME, 0);
  }

  auto& ossia_pd = ossia_pd::instance();
  ossia_pd.device = c;
}
} // pd namespace
} // ossia namespace

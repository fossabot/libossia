// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <QtTest>
#include <ossia/ossia.hpp>
#include <ossia/editor/scenario/time_value.hpp>
#include <iostream>
#include <thread>
#include <atomic>
#include "../Editor/TestUtils.hpp"

using namespace ossia;
class ConstraintBenchmark : public QObject
{
  Q_OBJECT

    void print_states(const ossia::scenario& s)
    {
      int i = 0;
      for(const auto& node : s.get_time_nodes())
      {
        std::cout << "Node " << i << "(" << node->is_evaluating() << ")";
        int j = 0;
        for(auto& ev : node->get_time_events())
        {
          std::cout << ":  Event: " << j << " => " << ev->get_status() << "\n";
          j++;
        }
        i++;
      }
    }

  void add_constraint_parallel(
      ossia::scenario& s,
      ossia::time_value def = 100._tv,
      ossia::time_value min = 100._tv,
      ossia::time_value max = 100._tv)
  {
    using namespace ossia;
    auto sn = s.get_start_time_node();
    auto se = *sn->get_time_events().begin();
    auto en = std::make_shared<ossia::time_node>();
    en->set_expression(ossia::expressions::make_expression_false());
    auto ee = std::make_shared<ossia::time_event>(ossia::time_event::exec_callback{}, *en, ossia::expressions::make_expression_true());
    en->insert(en->get_time_events().end(), ee);
    auto c = ossia::time_constraint::create([] (auto&&...) {}, *se, *ee, def, min, max);
    s.add_time_constraint(c);
  }

  ossia::time_event* add_constraint_serial(
      ossia::scenario& s,
      ossia::time_event& se,
      ossia::time_value def = 100._tv,
      ossia::time_value min = 100._tv,
      ossia::time_value max = 100._tv)
  {
    using namespace ossia;
    auto en = std::make_shared<ossia::time_node>();
    en->set_expression(ossia::expressions::make_expression_false());
    auto ee = std::make_shared<ossia::time_event>(ossia::time_event::exec_callback{}, *en, ossia::expressions::make_expression_true());
    en->insert(en->get_time_events().end(), ee);
    auto c = ossia::time_constraint::create([] (auto&&...) {}, se, *ee, def, min, max);
    s.add_time_constraint(c);
    return ee.get();
  }

private Q_SLOTS:
  void test_states_rigid()
  {
      root_scenario root;
      add_constraint_parallel(*root.scenario, 2_tv, 2_tv, 2_tv);

      std::cout << "\nBefore start:\n";
      print_states(*root.scenario);

      root.constraint->start();

      std::cout << "\nAfter start:\n";
      print_states(*root.scenario);

      root.constraint->tick(1000_tv);
      std::cout << "\nAfter tick 1:\n";
      print_states(*root.scenario);

      root.constraint->tick(1000_tv);
      std::cout << "\nAfter tick 2:\n";
      print_states(*root.scenario);

      root.constraint->tick(1000_tv);
      std::cout << "\nAfter tick 3:\n";
      print_states(*root.scenario);

      root.constraint->tick(1000_tv);
      std::cout << "\nAfter tick 4:\n";
      print_states(*root.scenario);
  }


  void test_states_flexible()
  {
      root_scenario root;
      add_constraint_parallel(*root.scenario, 3_tv, 2_tv, 4_tv);

      std::cout << "\nBefore start:\n";
      print_states(*root.scenario);

      root.constraint->start();

      std::cout << "\nAfter start:\n";
      print_states(*root.scenario);

      root.constraint->tick(1000_tv);
      std::cout << "\nAfter tick 1:\n";
      print_states(*root.scenario);

      root.constraint->tick(1000_tv);
      std::cout << "\nAfter tick 2:\n";
      print_states(*root.scenario);

      root.constraint->tick(1000_tv);
      std::cout << "\nAfter tick 3:\n";
      print_states(*root.scenario);

      root.constraint->tick(1000_tv);
      std::cout << "\nAfter tick 4:\n";
      print_states(*root.scenario);

      root.constraint->tick(1000_tv);
      std::cout << "\nAfter tick 5:\n";
      print_states(*root.scenario);
  }

  void test_states_flexible_no_min()
  {
      root_scenario root;
      add_constraint_parallel(*root.scenario, 3_tv, 0_tv, 4_tv);

      std::cout << "\nBefore start:\n";
      print_states(*root.scenario);

      root.constraint->start();

      std::cout << "\nAfter start:\n";
      print_states(*root.scenario);

      root.constraint->tick(1000_tv);
      std::cout << "\nAfter tick 1:\n";
      print_states(*root.scenario);

      root.constraint->tick(1000_tv);
      std::cout << "\nAfter tick 2:\n";
      print_states(*root.scenario);

      root.constraint->tick(1000_tv);
      std::cout << "\nAfter tick 3:\n";
      print_states(*root.scenario);

      root.constraint->tick(1000_tv);
      std::cout << "\nAfter tick 4:\n";
      print_states(*root.scenario);

      root.constraint->tick(1000_tv);
      std::cout << "\nAfter tick 5:\n";
      print_states(*root.scenario);
  }

  void test_states_flexible_no_max()
  {
      root_scenario root;
      add_constraint_parallel(*root.scenario, 3_tv, 2_tv, ossia::Infinite);

      std::cout << "\nBefore start:\n";
      print_states(*root.scenario);

      root.constraint->start();

      std::cout << "\nAfter start:\n";
      print_states(*root.scenario);

      root.constraint->tick(1000_tv);
      std::cout << "\nAfter tick 1:\n";
      print_states(*root.scenario);

      root.constraint->tick(1000_tv);
      std::cout << "\nAfter tick 2:\n";
      print_states(*root.scenario);

      root.constraint->tick(1000_tv);
      std::cout << "\nAfter tick 3:\n";
      print_states(*root.scenario);

      root.constraint->tick(1000_tv);
      std::cout << "\nAfter tick 4:\n";
      print_states(*root.scenario);

      root.constraint->tick(1000_tv);
      std::cout << "\nAfter tick 5:\n";
      print_states(*root.scenario);
  }


  void test_basic()
  {
    return;
    root_scenario root;

    for(int i = 0; i < 10000; i++)
      add_constraint_parallel(*root.scenario);

    const int N = 1000;
    root.constraint->start();
    auto t0 = std::chrono::high_resolution_clock::now();
    for(int i = 0; i < N; i++)
    {
      root.constraint->tick(1000_tv);
    }
    auto t1 = std::chrono::high_resolution_clock::now();

    auto tick_us = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() / double(N);
    qDebug() << tick_us;
  }

  void test_graph_parallel()
  {
    return;
    std::map<int, double> dur;
    for(auto k : {0, 1, 2, 5, 10, 50, 100, 200, 300, 400, 500, 600,
        700, 800, 900, 1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000,
        20000, 50000, 100000})
    {
      root_scenario root;

      for(int i = 0; i < k; i++)
        add_constraint_parallel(*root.scenario);

      const int N = 1000;
      root.constraint->start();
      auto t0 = std::chrono::high_resolution_clock::now();
      for(int i = 0; i < N; i++)
      {
        root.constraint->tick(1000_tv);
      }
      auto t1 = std::chrono::high_resolution_clock::now();

      auto tick_us = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() / double(N);
      dur.insert({k, tick_us});
      qDebug() << tick_us;
    }

    for(auto e : dur)
    {
      std::cerr << "XYPoint { x: " << e.first << "; y: " << e.second << " }\n";
    }
  }

  void test_graph_serial()
  {
    return;
    std::map<int, double> dur;
    for(auto k : {0, 1, 2, 5, 10, 50, 100, 200, 300, 400, 500, 600,
        700, 800, 900, 1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000,
        20000, 50000, 100000})
    {
      root_scenario root;

      ossia::time_event* start{root.start_event.get()};
      for(int i = 0; i < k; i++)
        start = add_constraint_serial(*root.scenario, *start);

      const int N = 1000;
      root.constraint->start();
      auto t0 = std::chrono::high_resolution_clock::now();
      for(int i = 0; i < N; i++)
      {
        root.constraint->tick(1000_tv);
      }
      auto t1 = std::chrono::high_resolution_clock::now();

      auto tick_us = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() / double(N);
      dur.insert({k, tick_us});
      qDebug() << tick_us;
    }

    for(auto e : dur)
    {
      // XYPoint { x: 0; y: 0 }
      std::cerr << "XYPoint { x: " << e.first << "; y: " << e.second << " }\n";
    }
  }


  void test_graph_random()
  {
    std::map<int, double> dur;
    for(auto k : {0, 1, 2, 5, 10, 50, 100, 200, 300, 400, 500, 600,
        700, 800, 900, 1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000,
        20000, 50000, 100000})
    {
      root_scenario root;

      ossia::time_event* start{root.start_event.get()};
      for(int i = 0; i < k; i++)
      {
        auto t = rand() % 2;
        if(t == 0)
            add_constraint_parallel(*root.scenario);
        else
        {
            auto N = root.scenario->get_time_nodes().size();
            auto e = root.scenario->get_time_nodes()[rand() % N]->get_time_events()[0];

            add_constraint_serial(*root.scenario, *e);
        }
      }
      const int N = 1000;
      root.constraint->start();
      auto t0 = std::chrono::high_resolution_clock::now();
      for(int i = 0; i < N; i++)
      {
        root.constraint->tick(1000_tv);
      }
      auto t1 = std::chrono::high_resolution_clock::now();

      auto tick_us = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() / double(N);
      dur.insert({k, tick_us});
      qDebug() << tick_us;
    }

    for(auto e : dur)
    {
      // XYPoint { x: 0; y: 0 }
      std::cerr << "XYPoint { x: " << e.first << "; y: " << e.second << " }\n";
    }
  }

};


QTEST_APPLESS_MAIN(ConstraintBenchmark)

#include "ConstraintBenchmark.moc"


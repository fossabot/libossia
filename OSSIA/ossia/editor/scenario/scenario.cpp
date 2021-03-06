// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/editor/scenario/scenario.hpp>
#include <ossia/editor/scenario/time_constraint.hpp>
#include <ossia/editor/scenario/time_event.hpp>
#include <ossia/editor/scenario/time_node.hpp>

#include <ossia/detail/algorithms.hpp>
#include <ossia/detail/logger.hpp>
#include <ossia/editor/exceptions.hpp>
#include <cassert>
#include <hopscotch_map.h>
#include <iostream>
#include <map>
#include <set>

namespace ossia
{
scenario::scenario()
{
  // create the start TimeNode
  m_nodes.push_back(std::make_shared<time_node>());
}

scenario::~scenario()
{
  for (auto& timenode : m_nodes)
  {
    timenode->cleanup();
  }
}

void scenario::start(ossia::state& st)
{
  m_waitingNodes.push_back(m_nodes[0].get());
  // start each TimeConstraint if possible
  for (const auto& timeConstraint : m_constraints)
  {
    time_constraint& cst = *timeConstraint;
    time_event::status startStatus = cst.get_start_event().get_status();
    time_event::status endStatus = cst.get_end_event().get_status();

    // the constraint is in the past
    if (startStatus == time_event::status::HAPPENED
        && endStatus == time_event::status::HAPPENED)
    {
    }
    // the start of the constraint is pending
    else if (
        startStatus == time_event::status::PENDING
        && endStatus == time_event::status::NONE)
    {
    }
    // the constraint is supposed to be running
    else if (
        startStatus == time_event::status::HAPPENED
        && endStatus == time_event::status::NONE)
    {
      m_runningConstraints.insert(&cst);
      cst.start(st);
    }
    // the constraint starts in the void and ends on a timenode that did
    // execute
    else if (
        startStatus == time_event::status::NONE
        && endStatus == time_event::status::HAPPENED)
    {
    }
    // the end of the constraint is pending
    else if (
        startStatus == time_event::status::HAPPENED
        && endStatus == time_event::status::PENDING)
    {
      m_runningConstraints.insert(&cst);
      cst.start(st);
    }
    // the constraint is in the future
    else if (
        startStatus == time_event::status::NONE
        && endStatus == time_event::status::NONE)
    {
    }
    // error
    else
    {
      throw execution_error(
          "scenario_impl::start: "
          "TimeEvent's status configuration of the "
          "TimeConstraint is not handled");
    }
  }
}

void scenario::stop()
{
  // stop each running TimeConstraints
  for (const auto& timeConstraint : m_constraints)
  {
    time_constraint& cst = *timeConstraint;
    cst.stop();
  }

  for (const auto& node : m_nodes)
  {
    node->reset();
  }

  m_runningConstraints.clear();
  constraints_started.clear();
  constraints_stopped.clear();
  m_waitingNodes.clear();
  m_overticks.clear();
  m_lastDate = time_value{};
}

void scenario::pause()
{
  // pause all running TimeConstraints
  for (const auto& timeConstraint : m_constraints)
  {
    auto& cst = *timeConstraint;
    cst.pause();
  }
}

void scenario::resume()
{
  // resume all running TimeConstraints
  for (const auto& timeConstraint : m_constraints)
  {
    auto& cst = *timeConstraint;
    cst.resume();
  }
}

void scenario::add_time_constraint(
    std::shared_ptr<time_constraint> timeConstraint)
{
  time_constraint& cst = *timeConstraint;

  // store the TimeConstraint if it is not already stored
  if (!contains(m_constraints, timeConstraint))
  {
    m_constraints.push_back(std::move(timeConstraint));
  }

  // store TimeConstraint's start node if it is not already stored
  add_time_node(cst.get_start_event().get_time_node().shared_from_this());

  // store TimeConstraint's end node if it is not already stored
  add_time_node(cst.get_end_event().get_time_node().shared_from_this());
}

void scenario::remove_time_constraint(
    const std::shared_ptr<time_constraint>& timeConstraint)
{
  remove_one(m_constraints, timeConstraint);
}

void scenario::add_time_node(std::shared_ptr<time_node> timeNode)
{
  // store a TimeNode if it is not already stored
  if (!contains(m_nodes, timeNode))
  {
    m_nodes.push_back(std::move(timeNode));
  }
}

void scenario::remove_time_node(const std::shared_ptr<time_node>& timeNode)
{
  remove_one(m_nodes, timeNode);
}

const std::shared_ptr<time_node>& scenario::get_start_time_node() const
{
  return m_nodes[0];
}

const ptr_container<time_node>& scenario::get_time_nodes() const
{
  return m_nodes;
}

const ptr_container<time_constraint>& scenario::get_time_constraints() const
{
  return m_constraints;
}
}

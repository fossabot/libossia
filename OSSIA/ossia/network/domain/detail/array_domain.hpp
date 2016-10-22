#pragma once
#include <ossia/network/domain/domain_base.hpp>
namespace ossia
{

/**
 * Applying a domain value by value to arrays
 */
struct tuple_clamp
{
  const net::domain_base<Tuple>& domain;

  ossia::value operator()(bounding_mode b, const Tuple& val) const
  {
    if (b == bounding_mode::FREE)
      return val;

    // We handle values by checking component by component
    const auto& values = domain.values;
    if(values.empty())
    {
      const auto N = val.value.size();
      const bool has_min = bool(domain.min);
      const bool has_max = bool(domain.max);
      if (has_min && has_max)
      {
        const Tuple& min = domain.min.get();
        const Tuple& max = domain.max.get();

        if(N != min.value.size() || N != max.value.size())
          return {};

        ossia::Tuple res;
        res.value.resize(N);
        switch (b)
        {
          case bounding_mode::CLIP:
            for(std::size_t i = 0; i < N; i++) res.value[i] = ossia::clamp(val.value[i], min.value[i], max.value[i]);
            break;
          case bounding_mode::WRAP:
            for(std::size_t i = 0; i < N; i++) res.value[i] = ossia::wrap(val.value[i], min.value[i], max.value[i]);
            break;
          case bounding_mode::FOLD:
            for(std::size_t i = 0; i < N; i++) res.value[i] = ossia::fold(val.value[i], min.value[i], max.value[i]);
            break;
          case bounding_mode::LOW:
            for(std::size_t i = 0; i < N; i++) res.value[i] = ossia::clamp_min(val.value[i], min.value[i]);
            break;
          case bounding_mode::HIGH:
            for(std::size_t i = 0; i < N; i++) res.value[i] = ossia::clamp_max(val.value[i], max.value[i]);
            break;
          default:
            break;
        }
        return res;
      }
      else if (has_min)
      {
        const Tuple& min = domain.min.get();
        if(N != min.value.size())
          return {};
        ossia::Tuple res;
        res.value.resize(N);
        switch(b)
        {
          case bounding_mode::CLIP:
          case bounding_mode::LOW:
            for(std::size_t i = 0; i < N; i++) res.value[i] = ossia::clamp_min(val.value[i], min.value[i]);
          default:
            break;
        }
        return res;
      }
      else if (has_max)
      {
        const Tuple& max = domain.max.get();
        if(N != max.value.size())
          return {};
        ossia::Tuple res;
        res.value.resize(N);
        switch(b)
        {
          case bounding_mode::CLIP:
          case bounding_mode::HIGH:
            for(std::size_t i = 0; i < N; i++) res.value[i] = ossia::clamp_max(val.value[i], max.value[i]);
          default:
            break;
        }
        return res;
      }

      return val;
    }
    else
    {
      auto it = values.find(val);
      return (it != values.end())
          ? val
          : ossia::value{};
    }
  }


  ossia::value operator()(bounding_mode b, Tuple&& val) const
  {
    if (b == bounding_mode::FREE)
      return val;

    // We handle values by checking component by component
    const auto& values = domain.values;
    if(values.empty())
    {
      const auto N = val.value.size();
      const bool has_min = bool(domain.min);
      const bool has_max = bool(domain.max);
      if (has_min && has_max)
      {
        const Tuple& min = domain.min.get();
        const Tuple& max = domain.max.get();

        if(N != min.value.size() || N != max.value.size())
          return {};

        switch (b)
        {
          case bounding_mode::CLIP:
            for(std::size_t i = 0; i < N; i++) val.value[i] = ossia::clamp(val.value[i], min.value[i], max.value[i]);
            break;
          case bounding_mode::WRAP:
            for(std::size_t i = 0; i < N; i++) val.value[i] = ossia::wrap(val.value[i], min.value[i], max.value[i]);
            break;
          case bounding_mode::FOLD:
            for(std::size_t i = 0; i < N; i++) val.value[i] = ossia::fold(val.value[i], min.value[i], max.value[i]);
            break;
          case bounding_mode::LOW:
            for(std::size_t i = 0; i < N; i++) val.value[i] = ossia::clamp_min(val.value[i], min.value[i]);
            break;
          case bounding_mode::HIGH:
            for(std::size_t i = 0; i < N; i++) val.value[i] = ossia::clamp_max(val.value[i], max.value[i]);
            break;
          default:
            break;
        }
        return std::move(val);
      }
      else if (has_min)
      {
        const Tuple& min = domain.min.get();
        if(N != min.value.size())
          return {};

        switch(b)
        {
          case bounding_mode::CLIP:
          case bounding_mode::LOW:
            for(std::size_t i = 0; i < N; i++) val.value[i] = ossia::clamp_min(val.value[i], min.value[i]);
          default:
            break;
        }
        return std::move(val);
      }
      else if (has_max)
      {
        const Tuple& max = domain.max.get();
        if(N != max.value.size())
          return {};

        switch(b)
        {
          case bounding_mode::CLIP:
          case bounding_mode::HIGH:
            for(std::size_t i = 0; i < N; i++) val.value[i] = ossia::clamp_max(val.value[i], max.value[i]);
          default:
            break;
        }
        return std::move(val);
      }
      return std::move(val);
    }
    else
    {
      auto it = values.find(val);
      return (it != values.end())
          ? std::move(val)
          : ossia::value{};
    }
  }
  // TODO numeric_tuple_clamp that will be used instead
  // of the loops in domain_clamp_visitor

};

template<std::size_t N>
struct vec_clamp
{
  const net::domain_base<Vec<float, N>>& domain;

  ossia::value operator()(bounding_mode b, Vec<float, N> val) const
  {
    if (b == bounding_mode::FREE)
      return val;

    // We handle values by checking component by component
    const auto& values = domain.values;
    if(values.empty())
    {
      const bool has_min = bool(domain.min);
      const bool has_max = bool(domain.max);
      if (has_min && has_max)
      {
        const Vec<float, N>& min = domain.min.get();
        const Vec<float, N>& max = domain.max.get();
        switch (b)
        {
          case bounding_mode::CLIP:
            for(std::size_t i = 0; i < N; i++) val.value[i] = ossia::clamp(val.value[i], min.value[i], max.value[i]);
            break;
          case bounding_mode::WRAP:
            for(std::size_t i = 0; i < N; i++) val.value[i] = ossia::wrap(val.value[i], min.value[i], max.value[i]);
            break;
          case bounding_mode::FOLD:
            for(std::size_t i = 0; i < N; i++) val.value[i] = ossia::fold(val.value[i], min.value[i], max.value[i]);
            break;
          case bounding_mode::LOW:
            for(std::size_t i = 0; i < N; i++) val.value[i] = ossia::clamp_min(val.value[i], min.value[i]);
            break;
          case bounding_mode::HIGH:
            for(std::size_t i = 0; i < N; i++) val.value[i] = ossia::clamp_max(val.value[i], max.value[i]);
            break;
          default:
            break;
        }
      }
      else if (has_min)
      {
        const Vec<float, N>& min = domain.min.get();
        switch(b)
        {
          case bounding_mode::CLIP:
          case bounding_mode::LOW:
            for(std::size_t i = 0; i < N; i++) val.value[i] = ossia::clamp_min(val.value[i], min.value[i]);
          default:
            break;
        }
      }
      else if (has_max)
      {
        const Vec<float, N>& max = domain.max.get();
        switch(b)
        {
          case bounding_mode::CLIP:
          case bounding_mode::HIGH:
            for(std::size_t i = 0; i < N; i++) val.value[i] = ossia::clamp_max(val.value[i], max.value[i]);
          default:
            break;
        }
      }

      return val;
    }
    else
    {
      auto it = values.find(val);
      return (it != values.end())
          ? val
          : ossia::value{};
    }
  }
};
}
#pragma once

#include <ranges>
#include <string>
#include <fmt/format.h>

namespace mgmt::home_assistant
{
  template<class Entity>
  class EntityStore
  {
  public:
    auto add(Entity entity)
    {
      if (std::ranges::find_if(entities_, [&entity](auto&& e) {
        return e.unique_id() == e.unique_id();
      }) != entities_.end()) {
        throw std::runtime_error{fmt::format("Entity with id: '{}' already exists", entity.unique_id())};
      }

      entities_.push_back(std::move(entity));
    }

    auto remove(const std::string& unique_id)
    {
      std::ranges::remove_if(entities_,  [&unique_id](auto&& entity) {
        return unique_id == entity.unique_id();
      });
    }

    auto get(const std::string& unique_id) -> Entity&
    {
      auto entity = std::ranges::find_if(entities_, [&unique_id](auto&& e) {
        return unique_id == e.unique_id();
      });

      if (entity == entities_.end()) {
        throw std::runtime_error(fmt::format("Cannot find entity for id: '{}'", unique_id));
      }

      return *entity;
    }

    auto exists(const std::string& unique_id) const
    {
      return std::ranges::find_if(entities_, [&unique_id](auto&& e) {
            return unique_id == e.unique_id();
          }) != entities_.end();
    }

  private:
    std::vector<Entity> entities_;
  };

  template<class E>
  inline auto entity_store = EntityStore<E>{};
}
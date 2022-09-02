#pragma once

#include <vector>
#include <optional>
#include <functional>

#include "device_id.hpp"

namespace mgmt::device {
using OnNodeRemoved_t = std::function<void()>;

class DeviceTree
{
  struct Node
  {
    DeviceId_t parent_{};
    DeviceId_t child_{};
    OnNodeRemoved_t func_{};

    Node(DeviceId_t p, DeviceId_t c, OnNodeRemoved_t f)
      : parent_{ std::move(p) }
      , child_{ std::move(c) }
      , func_{ std::move(f) }
    {}
    Node(Node&&) noexcept = default;
    Node& operator=(Node&&) noexcept = default;

    Node(const Node&) = default;
    Node& operator=(const Node&) = default;

    ~Node()
    {
      if (func_) {
        func_();
      }
    }
  };

public:
  /**
   * @brief Adds child to parent
   *
   * @param parent
   * @param child
   * @param on_parent_removed
   * @return true
   * @return false
   */
  bool add_child(const DeviceId_t& parent, const DeviceId_t& child, OnNodeRemoved_t on_parent_removed = OnNodeRemoved_t{});

  /**
   * @brief Removes child from parent
   *
   * @param parent
   * @param child
   * @return true
   * @return false
   */
  bool remove_child(const DeviceId_t& parent, const DeviceId_t& child);

  /**
   * @brief Removes parent if provided id matches to parent parent
   *
   * @return true
   * @return false
   */
  bool remove(const DeviceId_t&);

  /**
   * @brief Returns parent parent id for a given parent
   *
   * @return std::optional<DeviceId_t>
   */
  std::optional<DeviceId_t> parent(const DeviceId_t&) const;

  /**
   * @brief Returns all subnones for a given parent
   *
   * @return std::vector<DeviceId_t>
   */
  std::vector<DeviceId_t> all_children(const DeviceId_t&) const;

private:
  std::vector<Node> nodes_;
};
}// namespace mgmt::device
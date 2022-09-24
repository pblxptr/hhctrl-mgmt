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
    DeviceId_t parent_device_id{};
    DeviceId_t device_id{};
    OnNodeRemoved_t func{};

    Node(DeviceId_t parent_device_id, DeviceId_t device_id, OnNodeRemoved_t callback)
      : parent_device_id{ std::move(parent_device_id) }
      , device_id{ std::move(device_id) }
      , func{ std::move(callback) }
    {}
    // movable
    Node(Node&&) noexcept = default;
    Node& operator=(Node&&) noexcept = default;
    // non-copyable
    Node(const Node&) = default;
    Node& operator=(const Node&) = default;

    ~Node()
    {
      if (func) {
        func();
      }
    }
  };

public:
  /**
   * @brief Adds device_id to parent_device_id
   *
   * @param parent_device_id
   * @param device_id
   * @param on_parent_device_id_removed
   * @return true
   * @return false
   */
  bool add_child(const DeviceId_t& parent_device_id, const DeviceId_t& device_id, OnNodeRemoved_t on_parent_removed = OnNodeRemoved_t{});

  /**
   * @brief Removes device_id from parent_device_id
   *
   * @param parent_device_id
   * @param device_id
   * @return true
   * @return false
   */
  bool remove_child(const DeviceId_t& parent_device_id, const DeviceId_t& device_id);

  /**
   * @brief Removes parent_device_id if provided device_id matches to parent_device_id
   *
   * @return true
   * @return false
   */
  bool remove(const DeviceId_t& parent_device_id);

  /**
   * @brief Returns parent_device_id device_id for a given device_id
   *
   * @return std::optional<DeviceId_t>
   */
  std::optional<DeviceId_t> parent(const DeviceId_t& device_id) const;

  /**
   * @brief Returns all subnones for a given device_id
   *
   * @return std::vector<DeviceId_t>
   */
  std::vector<DeviceId_t> all_children(const DeviceId_t& parent_device_id) const;

private:
  std::vector<Node> nodes_;
};
}// namespace mgmt::device

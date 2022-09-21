#include <device/devicetree.hpp>

#include <algorithm>
#include <range/v3/range.hpp>
#include <range/v3/algorithm/for_each.hpp>
#include <range/v3/view/filter.hpp>

#include <device/logger.hpp>

namespace mgmt::device {
bool DeviceTree::add_child(const DeviceId_t& parent_device_id, const DeviceId_t& device_id, OnNodeRemoved_t on_parent_removed)
{
  common::logger::get(mgmt::device::Logger)->info("DeviceTree add device_id {} for parent_device_id {}", device_id, parent_device_id);

  if (auto node = std::ranges::find_if(nodes_, [device_id](auto&& xnode) {
        return xnode.device_id_ == device_id;
      });
      node != nodes_.end()) {

    return node->parent_device_id_ == parent_device_id;
  }

  nodes_.emplace_back(parent_device_id, device_id, std::move(on_parent_removed));

  return true;
}

bool DeviceTree::remove_child(const DeviceId_t& parent_device_id, const DeviceId_t& device_id)
{
  common::logger::get(mgmt::device::Logger)->info("DeviceTree remove device_id {} from parent_device_id {}", device_id, parent_device_id);

  auto node = std::ranges::find_if(nodes_, [parent_device_id, device_id](auto&& xnode) {
    return xnode.parent_device_id_ == parent_device_id && xnode.device_id_ == device_id;
  });

  if (node == nodes_.end()) {
    return false;
  }
  node->func_ = nullptr;

  nodes_.erase(node);

  return true;
}

bool DeviceTree::remove(const DeviceId_t& parent_device_id)
{
  common::logger::get(mgmt::device::Logger)->info("DeviceTree all nodes with parent_device_id: ", parent_device_id);

  return std::erase_if(nodes_, [parent_device_id](auto&& xnode) {
    return xnode.parent_device_id_ == parent_device_id;
  }) != 0;
}

std::optional<DeviceId_t> DeviceTree::parent(const DeviceId_t& device_id) const
{
  common::logger::get(mgmt::device::Logger)->info("DeviceTree get parent_device_id for {}", device_id);

  if (auto node = std::ranges::find_if(nodes_, [device_id](auto&& xnode) {
        return xnode.device_id_ == device_id;
      });
      node != nodes_.end()) {
    return node->parent_device_id_;
  }

  return std::nullopt;
}

std::vector<DeviceId_t> DeviceTree::all_children(const DeviceId_t& parent_device_id) const
{
  common::logger::get(mgmt::device::Logger)->info("DeviceTree get all children for parent_device_id: {}", parent_device_id);

  auto children = std::vector<DeviceId_t>{};
  auto push = [&children](auto&& device_id) {
    children.push_back(std::forward<decltype(device_id)>(device_id));
  };
  auto filter = [parent_device_id](auto&& node) {
    return node.parent_device_id_ == parent_device_id;
  };
  ranges::for_each(nodes_ | ranges::views::filter(filter),
    push,
    &Node::device_id_);
  return children;
}
}// namespace mgmt::device

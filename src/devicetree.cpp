#include <device/devicetree.hpp>

#include <ranges>
#include <algorithm>

#include <device/logger.hpp>

namespace mgmt::device {
  bool DeviceTree::add_child(const DeviceId_t& parent, const DeviceId_t& child, OnNodeRemoved_t on_parent_removed)
  {
    common::logger::get(mgmt::device::Logger)->info("DeviceTree add child {} for parent {}", child, parent);

    if (auto node = std::ranges::find_if(nodes_, [child](auto&& n) {
      return n.child_ == child;
    });
      node != nodes_.end())
    {
      if (node->parent_ == parent) {
        return true;
      }
      else {
        return false;
      }
    }

    nodes_.emplace_back(parent, child, std::move(on_parent_removed));

    return true;
  }

  bool DeviceTree::remove_child(const DeviceId_t& parent, const DeviceId_t& child)
  {
    common::logger::get(mgmt::device::Logger)->info("DeviceTree remove child {} from parent {}", child, parent);

    auto node = std::ranges::find_if(nodes_, [parent, child](auto&& n) {
      return n.parent_ == parent && n.child_ == child;
    });

    if (node == nodes_.end()) {
      return false;
    }
    node->func_ = nullptr;

    nodes_.erase(node);

    return true;
  }

  bool DeviceTree::remove(const DeviceId_t& parent)
  {
    common::logger::get(mgmt::device::Logger)->info("DeviceTree all nodes with parent: ", parent);

    return std::erase_if(nodes_, [parent](auto&& n) {
      return n.parent_ == parent;
    });
  }

  std::optional<DeviceId_t> DeviceTree::parent(const DeviceId_t& child) const
  {
    common::logger::get(mgmt::device::Logger)->info("DeviceTree get parent for {}", child);

    if (auto node = std::ranges::find_if(nodes_, [child](auto&& n) {
      return n.child_ == child;
    }); node != nodes_.end()) {
      return node->parent_;
    }

    return std::nullopt;
  }

  std::vector<DeviceId_t> DeviceTree::all_children(const DeviceId_t& parent) const
  {
    common::logger::get(mgmt::device::Logger)->info("DeviceTree get all children for parent: {}", parent);

    auto children = std::vector<DeviceId_t> {};
    auto push = [&children](auto&& e) {
      children.push_back(std::forward<decltype(e)>(e));
    };
    auto filter = [parent](auto&& n) {
      return n.parent_ == parent;
    };
    std::ranges::for_each(nodes_ | std::ranges::views::filter(filter),
                          push, &Node::child_);
    return children;
  }
}
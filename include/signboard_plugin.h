#pragma once

#include "graphics_util.h"

#include <chrono>
#include <memory>

namespace signboard {
// A plugin for the signboard
class SignboardPlugin {
public:
  SignboardPlugin(const std::string &pluginName)
      : _name(pluginName), _displayTime() {}

  // Can't copy plugins
  SignboardPlugin(const SignboardPlugin &other) = delete;
  SignboardPlugin &operator=(const SignboardPlugin &other) = delete;

  SignboardPlugin(SignboardPlugin &&other) = default;
  virtual ~SignboardPlugin() = default;

  // The _yielding member should be set true once the content of a plugin has
  // finished displaying. This authorises anything that owns the plugin to
  // know that the information is over and that it's free to advance.
  bool yielding() const { return _yielding; }

  void yield() { _yielding = true; }

  // Resets the _yielding member. Should be used by whatever owns the plugin
  // once it's advanced.
  void resetYielding() { _yielding = false; }

  std::chrono::time_point<std::chrono::system_clock> displayTime() {
    return _displayTime;
  }

  void setDisplayTime() { _displayTime = std::chrono::system_clock::now(); }

  // Function that should contain any information-refreshing logic and set the
  // _yielding member appropriately.
  virtual void update() {}

  // Update the canvas passed in by pointer with the plugin's content. This will
  // be called at the matrix's refresh rate, so logic in this function should be
  // optimised and not I/O blocking
  virtual void getCanvas(rgb_matrix::Canvas *canvas) const {}

  std::string name() const { return _name; }

private:
  std::string _name;
  std::chrono::time_point<std::chrono::system_clock> _displayTime;
  bool _yielding = false;
};
} // namespace signboard

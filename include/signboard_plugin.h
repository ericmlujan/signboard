#pragma once

#include "graphics_util.h"

#include <memory>

namespace signboard {
// A plugin for the signboard
// Plugins can contain arbitrary logic, but MUST implement getCanvas()
class SignboardPlugin {
public:
  SignboardPlugin(const std::string &pluginName) : name(pluginName) {}
  ~SignboardPlugin() = default;
  virtual void getCanvas(rgb_matrix::Canvas* canvas) const { }

private:
  std::string name;
};
} // namespace signboard

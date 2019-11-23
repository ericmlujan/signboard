#pragma once

#include <cassert>
#include <unordered_map>
#include <vector>

#include "signboard_plugin.h"

namespace signboard {
class MuxPlugin : public SignboardPlugin {
public:
  MuxPlugin() : SignboardPlugin("mux"){};

  virtual void update() override;
  virtual void getCanvas(rgb_matrix::Canvas *canvas) const override;

  void addPlugin(std::shared_ptr<SignboardPlugin> plugin);
  std::shared_ptr<SignboardPlugin> getPlugin(const std::string& name) const;

private:
  size_t _idx = 0;
  std::vector<std::shared_ptr<SignboardPlugin>> _plugins;
  std::unordered_map<std::string, size_t> _pluginMap;
};
} // namespace signboard

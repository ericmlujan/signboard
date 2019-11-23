#include "mux_plugin.h"
#include <iostream>

namespace signboard {
void MuxPlugin::update() {
  if (!_plugins.size()) {
    return;
  }

  assert(_idx < _plugins.size());

  _plugins[_idx]->update();

  if (_plugins[_idx]->yielding()) {
    _plugins[_idx]->resetYielding();
    ++_idx %= _plugins.size();
    _plugins[_idx]->setDisplayTime();
  }
}

void MuxPlugin::getCanvas(rgb_matrix::Canvas *canvas) const {
  if (_plugins.size() && _idx < _plugins.size()) {
    _plugins[_idx]->getCanvas(canvas);
  }
}

void MuxPlugin::addPlugin(std::shared_ptr<SignboardPlugin> plugin) {
  if (!plugin) {
    return;
  }
  _pluginMap.insert({plugin->name(), _plugins.size()});
  _plugins.push_back(plugin);
}

std::shared_ptr<SignboardPlugin> MuxPlugin::getPlugin(const std::string& name) const {
  auto result = _pluginMap.find(name);
  if (result != _pluginMap.end()) {
    return _plugins[result->second];
  }
}
} // namespace signboard

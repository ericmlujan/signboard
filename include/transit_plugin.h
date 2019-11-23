#pragma once

#include <chrono>
#include <cmath>

#include "graphics_util.h"
#include "signboard_plugin.h"
#include "transit_client.h"

namespace {
using clock = std::chrono::system_clock;
using time_point = std::chrono::time_point<clock>;
} // namespace

namespace signboard {
class TransitPlugin : public SignboardPlugin {
public:
  // TODO: don't hardcode in the agency
  TransitPlugin(const rgb_matrix::Font &font,
                std::shared_ptr<Deadpool> deadpool, const std::string& name)
      : _client("http://webservices.nextbus.com/service/publicXMLFeed",
                "sf-muni", deadpool),
        _font(font), SignboardPlugin(name) {}

  void addStopIds(const std::vector<std::string> &stopIds) {
    _client.addStopIds(stopIds);
  }

  virtual void update() override;
  virtual void getCanvas(rgb_matrix::Canvas *canvas) const override;

private:
  const rgb_matrix::Font &_font;
  Predictions _predictions;
  TransitClient _client;
};
} // namespace signboard

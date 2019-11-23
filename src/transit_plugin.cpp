#include "transit_plugin.h"

namespace {
const auto PLUGIN_DISPLAY_TIME = std::chrono::seconds(10);
} // namespace

namespace signboard {
void TransitPlugin::update() {
  if (clock::now() - displayTime() > PLUGIN_DISPLAY_TIME) {
    yield();
  }
  _predictions = _client.getPredictions();
}

void TransitPlugin::getCanvas(rgb_matrix::Canvas *canvas) const {
  if (!canvas) {
    return;
  }
  canvas->Clear();

  std::vector<TextLine> lines;
  if (_predictions.stops.size()) {
    lines.emplace_back(_predictions.stops.begin()->second.stopTitle);
    for (const auto &[_, stop] : _predictions.stops) {
      for (const auto &[_, route] : stop.routes) {
        if (route.predictions.size()) {
          lines.emplace_back();
          auto prediction = route.predictions[0];
          lines.back().cols[ColumnAlignment::LEFT] =
              Format("%s %s", route.routeTag.c_str(),
                     TruncateDirection(route.directionName).c_str());
          auto &rightCol = lines.back().cols[ColumnAlignment::RIGHT];
          rightCol = (prediction == 0 ? "now" : Format("%umin", prediction));
        }
      }
    }
  } else {
    lines.emplace_back("--");
  }

  FormatText(lines, _font, canvas);
}
} // namespace signboard

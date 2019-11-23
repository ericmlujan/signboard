#pragma once

#include <chrono>
#include <experimental/optional>
#include <iostream>

#include "curl/curl.h"
#include "deadpool.h"
#include "graphics_util.h"
#include "nlohmann/json.hpp"
#include "rapidxml.hpp"
#include "signboard_plugin.h"
#include "util.h"

namespace {
using json = nlohmann::json;
using clock = std::chrono::system_clock;
using time_point = std::chrono::time_point<clock>;
} // namespace

namespace signboard {
// Metadata for a NOAA weather station
struct StationData {
  std::string cwa;
  unsigned int gridX;
  unsigned int gridY;
};

struct WeatherData {
  std::string periodName;
  std::string forecast;
  float temperature;
  std::string temperatureUnit;
  std::string lastUpdated; // TODO: should this be a chrono timepoint?
};

class WeatherPlugin : public SignboardPlugin {
public:
  // Headless constructor for prototyping
  WeatherPlugin(double x, double y, const rgb_matrix::Font &font,
                std::shared_ptr<Deadpool> deadpool)
      : _font(font), _deadpool(deadpool), _lastRefreshTime(),
        SignboardPlugin("weather") {
    _deadpool->push([this, x, y] { _stationData = getStationData(x, y); });
  }

  void update();
  void getCanvas(rgb_matrix::Canvas *) const;

  // Given a UTM point, get the corresponding NOAA weather station
  // information
  std::optional<StationData> getStationData(double x, double y) const;

  // Get the latest weather data for the station stored in _stationData
  std::vector<WeatherData> getWeatherData() const;

private:
  const rgb_matrix::Font &_font;
  time_point _lastRefreshTime;
  std::shared_ptr<Deadpool> _deadpool;
  std::optional<StationData> _stationData = std::nullopt;
  std::vector<WeatherData> _weatherData;
};
} // namespace signboard

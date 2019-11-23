#include "weather_plugin.h"

namespace {
const char *URL_BASE = "https://api.weather.gov";
const auto REFRESH_DURATION = std::chrono::seconds(60);

const auto PLUGIN_DISPLAY_TIME = std::chrono::seconds(10);
} // namespace

namespace signboard {
void WeatherPlugin::update() {
  if (clock::now() - displayTime() > PLUGIN_DISPLAY_TIME) {
    yield();
  }

  if (clock::now() - _lastRefreshTime < REFRESH_DURATION) {
    return;
  }

  if (!_stationData) {
    return;
  }

  _deadpool->push([this] { _weatherData = getWeatherData(); });
  std::cout << "refreshing weather data" << std::endl;
  _lastRefreshTime = clock::now();
}

void WeatherPlugin::getCanvas(rgb_matrix::Canvas *canvas) const {
  if (!canvas) {
    return;
  }
  canvas->Clear();
  std::vector<TextLine> lines;
  std::string signboardText;
  if (!_stationData) {
    lines.emplace_back("No station data");
  } else if (!_weatherData.size()) {
    lines.emplace_back("No weather data");
  } else {
    size_t numLines = std::min<size_t>(4, _weatherData.size());
    for (size_t i = 0; i < numLines; ++i) {
      const auto &data = _weatherData[i];
      lines.emplace_back(
          data.periodName.c_str(), "",
          Format("%.0f%s", data.temperature, data.temperatureUnit.c_str()));
    }
  }
  FormatText(lines, _font, canvas);
}

std::optional<StationData> WeatherPlugin::getStationData(double x,
                                                         double y) const {
  std::string res;
  auto resCode = GetHTTP(Format("%s/points/%.4f,%.4f", URL_BASE, x, y), &res);

  if (resCode != 200) {
    fprintf(stderr, "http %d: %s\n", resCode, res.c_str());
    return std::nullopt;
  }

  json j;
  try {
    j = json::parse(res);
  } catch (const json::parse_error &err) {
    fprintf(stderr, "error in parsing json\n");
    return std::nullopt;
  }

  // TODO: handle malformed responses for which keys don't exist
  StationData stationData;
  stationData.cwa = j["properties"]["cwa"];
  stationData.gridX = j["properties"]["gridX"];
  stationData.gridY = j["properties"]["gridY"];
  std::cout << "got station " << stationData.cwa << " " << stationData.gridX
            << " " << stationData.gridY << std::endl;

  return stationData;
}

std::vector<WeatherData> WeatherPlugin::getWeatherData() const {
  std::vector<WeatherData> data;
  if (!_stationData) {
    return data;
  }

  std::string res;
  auto resCode = GetHTTP(Format("%s/gridpoints/%s/%d,%d/forecast", URL_BASE,
                                _stationData->cwa.c_str(), _stationData->gridX,
                                _stationData->gridY),
                         &res);

  if (resCode != 200) {
    fprintf(stderr, "http %d: %s\n", resCode, res.c_str());
    return data;
  }

  json j;
  try {
    j = json::parse(res);
  } catch (const json::parse_error &err) {
    fprintf(stderr, "error in parsing json\n");
    return data;
  }

  WeatherData w;
  for (const auto &period : j["properties"]["periods"]) {
    w.periodName = period["name"];
    w.forecast = period["shortForecast"];
    w.temperature = period["temperature"];
    w.temperatureUnit = period["temperatureUnit"];
    w.lastUpdated = j["properties"]["updateTime"];
    data.emplace_back(std::move(w));
  }
  return data;
}

} // namespace signboard

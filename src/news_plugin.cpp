#include "news_plugin.h"
#include "util.h"

#include "nlohmann/json.hpp"

namespace {
using json = nlohmann::json;
}

namespace signboard {
void NewsPlugin::update() {
  const auto REFRESH_INTERVAL = std::chrono::minutes(30);
  const auto NOT_FOUND_REFRESH_INTERVAL = std::chrono::seconds(10);
  auto timeSinceLastUpdate =
      std::chrono::system_clock::now() - _lastRefreshTime;
  if ((!_stories.size() && timeSinceLastUpdate > NOT_FOUND_REFRESH_INTERVAL) ||
      timeSinceLastUpdate > REFRESH_INTERVAL) {
    if (_deadpool) {
      _deadpool->push([this] { _stories = fetchStories(); });
      _lastRefreshTime = std::chrono::system_clock::now();
    }
  }

  // Tick the scroller
  // TODO check scroller to see if it's finished its scrolling so we know
  // whether to advance
  _scroller.update();
  if (_stories.size()) {
    if (_scroller.numLoops() > 0) {
      ++_currentStoryIdx;
      _currentStoryIdx %= _stories.size();
      auto &story = _stories[_currentStoryIdx];
      _scroller.setText(WrapText(
          Format("%s\n\n%s", story.headline.c_str(), story.summary.c_str()),
          _font, 128));
      yield();
    }
  } else {
    _scroller.setText(WrapText("Fetching latest news...", _font, 128));
    yield();
  }
}

void NewsPlugin::getCanvas(rgb_matrix::Canvas *canvas) const {
  // Delegate canvas fetching to the scroller
  _scroller.getCanvas(canvas);
}

std::vector<NewsStory> NewsPlugin::fetchStories() const {
  std::vector<NewsStory> stories;
  std::string jsonPayload;
  auto res = GetHTTP(_feedUrl, &jsonPayload);
  if (res == 200 || res == 304) {
    // Parse the response and populate our stories vector
    json j;
    try {
      j = json::parse(jsonPayload);
    } catch (const json::parse_error &err) {
      fprintf(stderr, "error in parsing json\n");
      return stories;
    }
    // TODO: validate response and handle errors
    // We'll need to implement a schema probably
    if (j.find("items") != j.end()) {
      NewsStory story;
      for (const auto &item : j["items"]) {
        story.headline = item["title"];
        story.summary = item["summary"];
        story.date = item["date_published"];
        stories.emplace_back(std::move(story));
      }
    }
  } else {
    std::fprintf(stderr, "Received HTTP %zu when fetching news from %s\n", res,
                 _feedUrl.c_str());
  }
  return stories;
}
}; // namespace signboard

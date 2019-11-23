#pragma once

#include <chrono>

#include "animation.h"
#include "deadpool.h"
#include "graphics_util.h"
#include "signboard_plugin.h"

namespace signboard {
struct NewsStory {
  std::string headline;
  std::string summary;
  std::string date;
};

class NewsPlugin : public SignboardPlugin {
public:
  NewsPlugin(const std::string &feedUrl, const rgb_matrix::Font &font,
             std::shared_ptr<Deadpool> deadpool)
      : _feedUrl(feedUrl), _font(font), _deadpool(deadpool),
        _scroller(font, 32), SignboardPlugin("news") {}

  void update();
  void getCanvas(rgb_matrix::Canvas *canvas) const;

  std::vector<NewsStory> fetchStories() const;

private:
  std::string _feedUrl;

  const rgb_matrix::Font &_font;
  std::shared_ptr<Deadpool> _deadpool;
  std::chrono::time_point<std::chrono::system_clock> _lastRefreshTime;

  Scroller _scroller;
  std::vector<NewsStory> _stories;
  size_t _currentStoryIdx = 0;
};
} // namespace signboard

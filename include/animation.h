#pragma once

#include <chrono>

#include "graphics_util.h"

namespace signboard {
class Scroller {
public:
  Scroller(const rgb_matrix::Font &font, size_t canvasHeight)
      : _font(font), _canvasHeight(canvasHeight) {}
  void setText(const std::string& text);
  void setText(const std::vector<TextLine> &text);
  void clearText();
  size_t numLoops() const;
  void update();
  void getCanvas(rgb_matrix::Canvas *canvas) const;

private:
  const rgb_matrix::Font &_font;
  std::vector<TextLine> _text;
  size_t _textHeight{0};
  size_t _canvasHeight{0};

  int _currentY = 0;
  bool _reachedBottom = false;
  size_t _numLoops = 0;

  std::chrono::time_point<std::chrono::system_clock> _scrollReference;
  const std::chrono::seconds PAGINATION_DURATION = std::chrono::seconds(3);
};
} // namespace signboard

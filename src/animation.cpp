#include <cmath>

#include "animation.h"

namespace signboard {
void Scroller::setText(const std::vector<TextLine> &text) {
  _text = text;
  _textHeight = text.size() * _font.height();
  _numLoops = 0;
  _reachedBottom = false;
  _scrollReference = std::chrono::time_point<std::chrono::system_clock>();
  _currentY = 0;
}

void Scroller::clearText() {
  _text.clear();
  _textHeight = 0;
  _numLoops = 0;
  _reachedBottom = false;
  _scrollReference = std::chrono::time_point<std::chrono::system_clock>();
  _currentY = 0;
}

size_t Scroller::numLoops() const {
  return _numLoops;
}

void Scroller::update() {
  if (_scrollReference == std::chrono::time_point<std::chrono::system_clock>()) {
    _scrollReference= std::chrono::system_clock::now();
  }

  bool shouldScroll =
      std::chrono::system_clock::now() - _scrollReference > PAGINATION_DURATION;
  if (shouldScroll) {
    if (_reachedBottom) {
      _currentY = 0;
      _reachedBottom = false;
      ++_numLoops;
      _scrollReference = std::chrono::system_clock::now();
      return;
    }

    // Reached the end of the text
    // TODO: update to account for the last pixel hitting the bottom
    if (_currentY < (int)_canvasHeight - (int)_textHeight) {
      _scrollReference = std::chrono::system_clock::now();
      _reachedBottom = true;
      return;
    }

    // TODO: handle floating point scroll speeds
    constexpr size_t SCROLL_SPEED = 1;
    _currentY -= SCROLL_SPEED;

    // We've scrolled one whole frame
    if (std::abs(_currentY) % _canvasHeight == 0) {
      _scrollReference = std::chrono::system_clock::now();
    }
  }
}

void Scroller::getCanvas(rgb_matrix::Canvas *canvas) const {
  canvas->Clear();
  int y = _currentY + _font.height();
  for (const auto &line : _text) {
    FormatLine(line, _font, y, canvas);
    y += _font.height();
  }
}
} // namespace signboard

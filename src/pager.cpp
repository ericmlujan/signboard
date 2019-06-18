#include "pager.h"

namespace signboard {
void Page::getFramebuffer(rgb_matrix::FrameCanvas *canvas) const {
  formatText(canvas);
}

void Page::formatText(rgb_matrix::FrameCanvas *canvas) const {
  if (!canvas) {
    return;
  }

  int yBase = std::max(_font.baseline(), 0);
  int y = yBase;

  // Read through the lines in the text field and split upon encountering \n
  std::string line;
  for (size_t i = 0; i <= _text.size(); ++i) {
    line += _text[i];
    if (_text[i] == '\n' || _text[i] == '\0') {
      // New line or end of string encountered; strip off newline and write to canvas
      line[line.size() - 1] = '\0';
      rgb_matrix::DrawText(canvas, _font, 0, 0 + y, _color,
                           nullptr, line.c_str(), 0);
      y += yBase;
      line.clear();

      // If we've overflowed the canvas, no need to continue
      if (y > canvas->height()) {
        break;
      }
    }
  }
}

void Page::setText(const std::string &text) { _text = text; }

bool Pager::pageStale() const {
  if (_pages.empty() || _currentIndex >= _pages.size()) {
    return false;
  }
  auto now = std::chrono::system_clock::now();
  auto timeSinceLastUpdate = (now - _pageTime);
  return timeSinceLastUpdate > _settings.pageDuration;
}

size_t Pager::registerPage(const std::shared_ptr<Page> &page) {
  _pages.push_back(page);
  return _pages.size() - 1;
}

void Pager::getCurrentFramebuffer(rgb_matrix::FrameCanvas *canvas) {
  canvas->Clear();
  if (_pages.empty() || _currentIndex + 1 > _pages.size()) {
    return;
  }
  // Check the last time we switched the page
  if (pageStale()) {
    _pageTime = std::chrono::system_clock::now();
    _currentIndex++;
    _currentIndex %= _pages.size();
  }

  _pages.at(_currentIndex)->getFramebuffer(canvas);
}
} // namespace signboard

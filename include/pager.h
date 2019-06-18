#pragma once

#include "graphics.h"
#include "led-matrix.h"

#include <chrono>
#include <cmath>
#include <memory>
#include <string>

namespace signboard {

using clock = std::chrono::system_clock;
// Represents a page of information to be displayed on the matrix
class Page {
public:
  Page(const rgb_matrix::Font &font, const rgb_matrix::Color &color)
      : _font(font), _color(color) {}
  Page(const Page &) = default;

  // Get the framebuffer for the current page
  void getFramebuffer(rgb_matrix::FrameCanvas *canvas) const;

  // Format a text string by parsing it and splitting into newlines.
  // If text is too long to be displayed on the matrix screen, it is truncated
  // (both vertically and horizontally)
  void formatText(rgb_matrix::FrameCanvas *canvas) const;

  // Set the stored text for this page
  void setText(const std::string &text);

private:
  // Static data, set at init
  const rgb_matrix::Font &_font;
  const rgb_matrix::Color &_color;

  // Dynamic data
  std::string _text = "";
};

// Class for updating the matrix framebuffer with the latest page of information
// and iterating through pages as appropriate
class Pager {
public:
  // Pager settings
  struct PagerSettings {
    // Default duration for a page to be displayed
    clock::duration pageDuration;
  };

  Pager(const PagerSettings &settings) : _settings(settings) {}
  Pager(const Pager &) = default;

  // Write the current framebuffer to the supplied canvas object, after clearing
  // it
  void getCurrentFramebuffer(rgb_matrix::FrameCanvas *canvas);
  size_t registerPage(const std::shared_ptr<Page> &page);

private:
  // Check if the current page has timed out and _currentIndex should be
  // incremented
  bool pageStale() const;

  PagerSettings _settings;

  // By default the pager will page with a constant time duration per page
  // TODO: this doesn't work for scrolling or animated text; we'll have to have
  // dynamic page duration at some point, but done is better than perfect Array
  // of pages registered with the pager
  std::vector<std::shared_ptr<Page>> _pages;
  std::size_t _currentIndex = 0;

  // "Time is an illusion, lunchtime doubly so."
  clock::time_point _pageTime;
};
} // namespace signboard

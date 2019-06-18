#pragma once

#include "graphics.h"
#include "led-matrix.h"

#include <algorithm>
#include <cmath>

namespace signboard {
// Defines how a column should be aligned within a line
enum ColumnAlignment { LEFT = 0, CENTER = 1, RIGHT = 2 };

struct Column {
  std::string text = "";
  ColumnAlignment alignment = ColumnAlignment::LEFT;
  void operator=(const std::string &text);
};

// A line of text consisting of three columns of text
// TODO: should the line also set its own font?
struct TextLine {
  TextLine() {
    cols.resize(3);
    cols[1].alignment = ColumnAlignment::CENTER;
    cols[2].alignment = ColumnAlignment::RIGHT;
  }

  std::vector<Column> cols;
  rgb_matrix::Color color{255, 255, 255};
};

void FormatText(const std::vector<TextLine> &lines,
                       const rgb_matrix::Font &font,
                       rgb_matrix::FrameCanvas *canvas);
void FormatLine(const TextLine &line, const rgb_matrix::Font &font,
                       const size_t baselineY, rgb_matrix::FrameCanvas *canvas);
} // namespace signboard

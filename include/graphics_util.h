#pragma once

#include "graphics.h"
#include "led-matrix.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <memory>
#include <string>

namespace signboard {
// Defines how a column should be aligned within a line
enum ColumnAlignment { LEFT = 0, CENTER = 1, RIGHT = 2 };

struct Column {
  std::string text = "";
  ColumnAlignment alignment = ColumnAlignment::LEFT;
  void operator=(const std::string &rhs) { text = rhs; }
};

// A line of text consisting of three columns of text
// TODO: should the line also set its own font?
struct TextLine {
  TextLine(const std::string &l, const std::string &c, const std::string &r) {
    cols.resize(3);
    cols[0].alignment = ColumnAlignment::LEFT;
    cols[0] = l;
    cols[1].alignment = ColumnAlignment::CENTER;
    cols[1] = c;
    cols[2].alignment = ColumnAlignment::RIGHT;
    cols[2] = r;
  }

  template <typename T> TextLine(const T &text) : TextLine(text, "", "") {}

  TextLine() : TextLine("", "", "") {}

  void operator=(const std::string &text) { cols[0] = text; }

  std::vector<Column> cols;
  rgb_matrix::Color color{255, 200, 0};
};

void FormatText(const std::vector<TextLine> &lines,
                const rgb_matrix::Font &font, rgb_matrix::Canvas *canvas);
void FormatLine(const TextLine &line, const rgb_matrix::Font &font,
                const size_t baselineY, rgb_matrix::Canvas *canvas);

std::vector<TextLine> WrapText(const std::string &text,
                               const rgb_matrix::Font &font, size_t lineWidth);
} // namespace signboard

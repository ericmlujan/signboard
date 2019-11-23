#include "graphics_util.h"
#include "lib/utf8-internal.h"

namespace signboard {
void FormatText(const std::vector<TextLine> &lines,
                const rgb_matrix::Font &font, rgb_matrix::Canvas *canvas) {
  size_t baselineY = std::max(0, font.baseline());
  for (const auto &line : lines) {
    FormatLine(line, font, baselineY, canvas);
    baselineY += font.baseline();
  }
}

// Format a line on the canvas, with the baseline at the y value of baselineY
void FormatLine(const TextLine &line, const rgb_matrix::Font &font,
                const size_t baselineY, rgb_matrix::Canvas *canvas) {
  // TODO: bounds checking for columns running into one another, right now the
  // text will just be on top of itself
  if (!canvas) {
    return;
  }
  auto canvasWidth = canvas->width();
  for (const auto &col : line.cols) {
    size_t colWidth = 0;
    for (size_t i = 0; i < col.text.size(); ++i) {
      colWidth += font.CharacterWidth(col.text[i]);
    }
    size_t x = 0;
    switch (col.alignment) {
    case ColumnAlignment::LEFT:
      // Just left-align the damn thing
      rgb_matrix::DrawText(canvas, font, x, baselineY, line.color, nullptr,
                           col.text.c_str(), 0);
      break;
    case ColumnAlignment::CENTER:
      // Find the centre of the canvas, then subtract off half the width of the
      // column
      x = std::max<size_t>(0,
                           std::floor<size_t>(canvasWidth / 2 - colWidth / 2));
      rgb_matrix::DrawText(canvas, font, x, baselineY, line.color, nullptr,
                           col.text.c_str(), 0);
      break;
    case ColumnAlignment::RIGHT:
      // Subtract off the width of the column from the width of the canvas
      x = std::max<size_t>(0, canvasWidth - colWidth);
      rgb_matrix::DrawText(canvas, font, x, baselineY, line.color, nullptr,
                           col.text.c_str(), 0);
      break;
    }
  }
}

std::vector<TextLine> WrapText(const std::string &text,
                               const rgb_matrix::Font &font, size_t lineWidth) {
  std::vector<TextLine> lines;
  // Iterate through each character of the string
  size_t currentWidth = 0;
  size_t lineStartIdx = 0;
  // Read one past the size of the string to capture the terminating null byte
  for (size_t i = 0; i < text.size() + 1; ++i) {
    // Encountered a newline; this automatically wraps!
    if (text[i] == '\n' || text[i] == '\0') {
      lines.emplace_back(text.substr(lineStartIdx, i - lineStartIdx));
      currentWidth = 0;
      lineStartIdx = i + 1;
      continue;
    }

    auto iter = text.begin() + i;
    uint32_t cp = utf8_next_codepoint(iter);
    currentWidth += font.CharacterWidth(cp);

    // Read past the end of the screen. Backtrack to the last known space
    if (currentWidth > lineWidth) {
      for (; i > 0; --i) {
        if (text[i] == ' ' || text[i] == '\n' || text[i] == '\t') {
          lines.emplace_back(text.substr(lineStartIdx, (int)i - lineStartIdx));
          lineStartIdx = i + 1;
          i++;
          currentWidth = 0;
          break;
        }
      }
    }
  }
  return lines;
};
} // namespace signboard

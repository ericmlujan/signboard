#include "graphics_util.h"

namespace signboard {
void Column::operator=(const std::string &rhs) { text = rhs; }

void FormatText(const std::vector<TextLine> &lines,
                const rgb_matrix::Font &font, rgb_matrix::FrameCanvas *canvas) {
  canvas->Clear();
  size_t baselineY = std::max(0, font.baseline());
  for (const auto &line : lines) {
    FormatLine(line, font, baselineY, canvas);
    baselineY += font.baseline();
  }
}

// Format a line on the canvas, with the baseline at the y value of baselineY
void FormatLine(const TextLine &line, const rgb_matrix::Font &font,
                const size_t baselineY, rgb_matrix::FrameCanvas *canvas) {
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
      x =
        std::max<size_t>(0, std::floor<size_t>(canvasWidth / 2 - colWidth / 2));
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
} // namespace signboard

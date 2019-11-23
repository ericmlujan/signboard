#include <chrono>
#include <iostream>
#include <signal.h>
#include <sstream>
#include <string>
#include <thread>
#include <unistd.h>

#include "animation.h"
#include "mux_plugin.h"
#include "news_plugin.h"
#include "transit_plugin.h"
#include "weather_plugin.h"

volatile bool interrupted = false;

static void handleSignal(int sig) {
  std::printf("Caught signal %d; exiting.", sig);
  interrupted = true;
}

int main(int argc, char **argv) {
  signal(SIGTERM, handleSignal);
  signal(SIGINT, handleSignal);

  // Init GPIO
  // Configure LED matrix
  // TODO: actually do this from a config file
  rgb_matrix::RuntimeOptions runtimeOptions;
  runtimeOptions.gpio_slowdown = 2;
  rgb_matrix::RGBMatrix::Options matrixOptions;
  matrixOptions.hardware_mapping = "adafruit-hat-pwm";
  matrixOptions.rows = 32;
  matrixOptions.cols = 64;
  matrixOptions.chain_length = 2;
  matrixOptions.pixel_mapper_config = "Rotate:180";
  constexpr float refreshRate = 30.0f; // Hz
  constexpr size_t usleepDuration = 1e6f / refreshRate;
  constexpr size_t brightness = 100;
  constexpr size_t workerThreads = 5;
  constexpr char *fontPath = "fonts/5x8.bdf";

  // Try loading the font
  rgb_matrix::Font font;
  if (!font.LoadFont(fontPath)) {
    std::fprintf(stderr, "Couldn't load font file from path %s\n", fontPath);
    return 1;
  }

  auto matrix = std::unique_ptr<rgb_matrix::RGBMatrix>(
      rgb_matrix::CreateMatrixFromOptions(matrixOptions, runtimeOptions));
  rgb_matrix::FrameCanvas *offscreenCanvas = matrix->CreateFrameCanvas();

  matrix->SetBrightness(brightness);

  // PLUGIN SETUP
  auto deadpool = std::make_shared<signboard::Deadpool>(workerThreads);

  signboard::MuxPlugin mux;
  auto weatherPlugin = std::make_shared<signboard::WeatherPlugin>(
      37.8029, -122.4107, font, deadpool);

  auto newsPlugin = std::make_shared<signboard::NewsPlugin>("https://www.npr.org/feeds/1001/feed.json", font, deadpool);

  mux.addPlugin(weatherPlugin);
  mux.addPlugin(newsPlugin);


  while (!interrupted) {
    mux.update();
    mux.getCanvas(offscreenCanvas);

    // Sleep for the refresh rate of the display
    std::this_thread::sleep_for(std::chrono::microseconds(usleepDuration));
    offscreenCanvas = matrix->SwapOnVSync(offscreenCanvas);
  }

  matrix->Clear();
  return 0;
}

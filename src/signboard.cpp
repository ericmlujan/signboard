#include "graphics_util.h"
#include "pager.h"
#include "transit_client.h"

#include <chrono>
#include <iostream>
#include <signal.h>
#include <sstream>
#include <string>
#include <thread>
#include <unistd.h>

volatile bool interrupted = false;

static void handleSignal(int sig) {
  std::printf("Caught signal %d; exiting.", sig);
  interrupted = true;
}

void getCurrentTime(std::string *out) {
  char buf[64];

  auto now = std::chrono::system_clock::now();
  std::time_t n = std::chrono::system_clock::to_time_t(now);
  std::strftime(buf, sizeof(buf), "%H:%M", std::localtime(&n));

  *out = buf;
}

int main(int argc, char **argv) {
  signal(SIGTERM, handleSignal);
  signal(SIGINT, handleSignal);

  // Init GPIO
  // Configure LED matrix
  // TODO: actually do this from a config file or some shit
  rgb_matrix::RuntimeOptions runtimeOptions;
  runtimeOptions.gpio_slowdown = 2;
  rgb_matrix::RGBMatrix::Options matrixOptions;
  matrixOptions.hardware_mapping = "adafruit-hat-pwm";
  matrixOptions.rows = 32;
  matrixOptions.cols = 64;
  matrixOptions.chain_length = 2;
  matrixOptions.pixel_mapper_config = "Rotate:180";
  float refreshRate = 7.0f; // Hz
  size_t usleepDuration = 1e6f / refreshRate;
  size_t brightness = 100;
  auto pageDuration = std::chrono::seconds(5);

  signboard::Pager::PagerSettings settings;
  settings.pageDuration = pageDuration;

  const rgb_matrix::Color red(255, 0, 0);
  const rgb_matrix::Color yellow(255, 174, 0);
  const rgb_matrix::Color white(255, 255, 255);
  const rgb_matrix::Color green(126, 255, 84);

  const char *fontPath = "fonts/5x8.bdf";

  // Try loading the font
  rgb_matrix::Font font;
  if (!font.LoadFont(fontPath)) {
    std::fprintf(stderr, "Couldn't load font file from path %s\n", fontPath);
    return 1;
  }

  auto matrix = std::unique_ptr<rgb_matrix::RGBMatrix>(
      rgb_matrix::CreateMatrixFromOptions(matrixOptions, runtimeOptions));
  rgb_matrix::FrameCanvas *offscreenMatrix = matrix->CreateFrameCanvas();

  matrix->SetBrightness(brightness);

  auto pager = std::make_unique<signboard::Pager>(settings);
  auto muniClient = std::make_unique<signboard::TransitClient>(
      "http://webservices.nextbus.com/service/publicXMLFeed", "sf-muni");

  auto page1 = std::make_shared<signboard::Page>(font, yellow);
  pager->registerPage(page1);

  // TODO read these from a config file instead
  // THIS IS JUST A KLUDGE FOR NOW
  static std::vector<std::string> stopIds = {"10|5857", "12|5857"};
  muniClient->addStopIds(stopIds);

  signboard::PredictionMap predictions;

  // const size_t characterWidth = std::floor<size_t>(
  //                                                  (matrixOptions.cols * matrixOptions.chain_length) / font.CharacterWidth(u'W'));

  // signboard::TextLine line1;
  // line1.cols[signboard::ColumnAlignment::LEFT] = "L";
  // line1.cols[signboard::ColumnAlignment::CENTER] = "C";
  // line1.cols[signboard::ColumnAlignment::RIGHT] = "R";
  // line1.color = yellow;

  while (!interrupted) {
    std::stringstream ss;
    predictions = muniClient->getPredictions();
    ss << predictions;
    page1->setText(ss.str());

    pager->getCurrentFramebuffer(offscreenMatrix);

    std::this_thread::sleep_for(std::chrono::microseconds(usleepDuration));
    offscreenMatrix = matrix->SwapOnVSync(offscreenMatrix);
  }

  matrix->Clear();
  return 0;
}

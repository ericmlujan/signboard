#include "transit_client.h"

namespace signboard {

using Clock = std::chrono::system_clock;

std::string TruncateDirection(const std::string &direction) {
  std::string newDirection = direction;
  // We'll look for the word " to "
  RemoveSubstring(" to ", &newDirection);
  // Now we'll slice out the intersection
  RemoveSubstring(" + ", &newDirection);
  return newDirection;
}

// Return the cached set of predictions and kick off a refresh if it's time to
// refresh
const Predictions& TransitClient::getPredictions() {
  auto now = Clock::now();

  // Trigger a refresh if it's that kind of time
  if (_deadpool && (now - _lastRefreshTime > _refreshDuration)) {
    _lastRefreshTime = Clock::now();
    _deadpool->push(&TransitClient::refreshPredictions, this);
  }

  return _predictions;
}

// Get a list of arrival times for a particular stop ID
void TransitClient::refreshPredictions() {
  if (_stopIds.empty()) {
    return;
  }
  std::string url = _baseURL + "?command=predictionsForMultiStops&a=" + _agency;
  for (const auto &stopId : _stopIds) {
    url += "&stops=" + stopId;
  }
  url += "&useShortTitles=true";

  std::string body;
  size_t res = GetHTTP(url, &body);

  // Error in processing, that's not good
  if (res != 200) {
    std::fprintf(stderr,
                 "could not fetch transit predictions; got http response %zu",
                 res);
  } else {
    {
      std::lock_guard<std::mutex> guard(_mutex);
      parseNextMuniResponse(&body[0], &_predictions);
      std::printf("Predictions refreshed.\n");
    }
  }
}

void TransitClient::parseNextMuniResponse(std::string xmlString,
                                          Predictions *results) const {
  if (xmlString.empty()) {
    return;
  }

  // Parse the resulting XML file
  rapidxml::xml_document<> xml;
  try {
    xml.parse<0>(&xmlString[0]);
  } catch (const rapidxml::parse_error &err) {
    std::fprintf(stderr,
                 "could not parse xml returned by the nextbus api: %s\n",
                 err.what());
    return;
  }

  results->clear();
  rapidxml::xml_node<> *body = xml.first_node("body");
  // Parse each set of predictions for the given stop ID
  for (auto predictions = body->first_node("predictions");
       predictions != nullptr;
       predictions = predictions->next_sibling("predictions")) {
    for (auto direction = predictions->first_node("direction");
         direction != nullptr;
         direction = direction->next_sibling("direction")) {
      std::string routeTitle =
          predictions->first_attribute("routeTitle")->value();
      std::string routeTag = predictions->first_attribute("routeTag")->value();
      std::string directionName = direction->first_attribute("title")->value();
      std::string stopTitle =
          predictions->first_attribute("stopTitle")->value();
      std::string stopId = predictions->first_attribute("stopTag")->value();
      for (auto prediction = direction->first_node("prediction");
           prediction != nullptr;
           prediction = prediction->next_sibling("prediction")) {
        results->addPrediction(
            routeTag, directionName, stopId, stopTitle,
            std::atoi(prediction->first_attribute("minutes")->value()));
      }
    }
  }
}
} // namespace signboard

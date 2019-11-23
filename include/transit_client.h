#pragma once

#include <chrono>
#include <ostream>
#include <thread>
#include <unordered_map>
#include <vector>

#include "deadpool.h"
#include "rapidxml.hpp"
#include "types.h"
#include "util.h"

namespace signboard {

// Formats a NextBus stop title to make it small enough to read on the signboard
std::string TruncateDirection(const std::string &direction);

// A client that fetches information from the [NextBus Public XML
// Feed](http://www.nextbus.com/xmlFeedDocs/NextBusXMLFeed.pdf)
// TODO: add support for multiple agencies and stopids
class TransitClient {
public:
  TransitClient(const std::string &baseURL, const std::string &agency,
                std::shared_ptr<Deadpool> deadpool)
      : _baseURL(baseURL), _agency(agency), _deadpool(deadpool),
        _refreshDuration(std::chrono::seconds(30)) {
    _lastRefreshTime = std::chrono::time_point<std::chrono::system_clock>();
    curl_global_init(CURL_GLOBAL_DEFAULT);
  }

  TransitClient(const TransitClient &) = default;
  TransitClient(TransitClient &&) = default;

  const Predictions &getPredictions();

  void refreshPredictions();
  void parseNextMuniResponse(std::string xmlString,
                             Predictions *predictions) const;

  void reset() { _stopIds.clear(); }

  void addStopIds(const std::vector<std::string> &stopIds) {
    _stopIds.insert(_stopIds.begin(), stopIds.begin(), stopIds.end());
  }

private:
  std::mutex _mutex;
  std::vector<std::string> _stopIds;
  std::string _baseURL;
  std::string _agency;
  Predictions _predictions;
  std::shared_ptr<Deadpool> _deadpool;

  // Refresh-related information
  std::chrono::time_point<std::chrono::system_clock> _lastRefreshTime;
  const std::chrono::seconds _refreshDuration;
};
} // namespace signboard

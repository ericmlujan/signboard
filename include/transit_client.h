#pragma once

#include <chrono>
#include <ostream>
#include <thread>
#include <unordered_map>
#include <vector>

#include "absl/container/flat_hash_map.h"
#include "absl/hash/hash.h"
#include "curl/curl.h"

#include "deadpool.h"
#include "rapidxml.hpp"
// #include "signboard_plugin.h"
#include "types.h"
#include "util.h"

#define _NUM_NETWORK_THREADS 5
#define _REFRESH_DURATION 30 // seconds

namespace signboard {

// Formats a NextBus stop title to make it small enough to read on the signboard
std::string TruncateDirection(const std::string &direction);

// A client that fetches information from the [NextBus Public XML
// Feed](http://www.nextbus.com/xmlFeedDocs/NextBusXMLFeed.pdf)
class TransitClient {
public:
  TransitClient(const std::string &baseURL, const std::string &agency)
      : _baseURL(baseURL), _agency(agency), _deadpool(_NUM_NETWORK_THREADS),
        _refreshDuration(std::chrono::seconds(_REFRESH_DURATION)) {
    _lastRefreshTime = std::chrono::time_point<std::chrono::system_clock>();
    curl_global_init(CURL_GLOBAL_DEFAULT);
  }

  TransitClient(const TransitClient &) = default;
  TransitClient(TransitClient &&) = default;

  Predictions getPredictions();

  void refreshPredictions();
  void parseNextMuniResponse(std::string xmlString,
                             Predictions *predictions) const;

  void reset() { _stopIds.clear(); }

  void addStopIds(const std::vector<std::string> &stopIds) {
    _stopIds.insert(_stopIds.begin(), stopIds.begin(), stopIds.end());
  }

  // TODO: these aren't really methods that should belong in the TransitClient
  // class, as they're just light wrappers for libcurl and should be factored
  // out into free functions in util
  size_t getHTTP(const std::string &url, std::string *out) const;
  static size_t handleHTTPResponse(char *bytes, size_t size, size_t nmenb,
                                   void *out);

  // virtual void getCanvas(rgb_matrix::Canvas* canvas) const;

private:
  std::mutex _mutex;
  std::vector<std::string> _stopIds;
  std::string _baseURL;
  std::string _agency;
  Predictions _predictions;
  Deadpool _deadpool;

  // Refresh-related information
  std::chrono::time_point<std::chrono::system_clock> _lastRefreshTime;
  const std::chrono::seconds _refreshDuration;
};
} // namespace signboard

#pragma once

#include <cstdlib>
#include <map>
#include <string>

#include "absl/hash/hash.h"

namespace signboard {
using Prediction = uint32_t;

struct Route {
  std::string routeTag;
  std::string directionName;
  std::vector<Prediction> predictions;

  Route(const std::string &tag, const std::string &direction)
      : routeTag(tag), directionName(direction) {}

  template <typename H> friend H AbslHashValue(H h, const Route &r) {
    return H::combine(std::move(h), r.routeTag, r.directionName);
  }
};

struct Stop {
  std::string stopId;
  std::string stopTitle;
  std::map<std::string, Route> routes; // routeTag => Route

  Stop(const std::string &id, const std::string &title)
      : stopId(id), stopTitle(title) {}

  template <typename H> friend H AbslHashValue(H h, const Stop &s) {
    return H::combine(std::move(h), s.stopId);
  }
};

struct Predictions {
  std::map<std::string, Stop> stops; // stopId => Stop

  void clear() {
    for (auto &[stopId, stop] : stops) {
      for (auto &[routeTag, route] : stop.routes) {
        route.predictions.clear();
      }
    }
    _size = 0;
  }

  size_t size() { return _size; }

  friend std::ostream &operator<<(std::ostream &stream,
                                  const Predictions &predictions) {
    for (const auto &[_, stop] : predictions.stops) {
      stream << stop.stopId << " - " << stop.stopTitle << std::endl;
      for (const auto &[_, route] : stop.routes) {
        stream << route.routeTag << " - " << route.directionName;
        for (auto prediction : route.predictions) {
          stream << " " << prediction;
        }
        stream << std::endl;
      }
    }
    return stream;
  }

  void addPrediction(const std::string &routeTag,
                     const std::string &directionName,
                     const std::string &stopId, const std::string &stopTitle,
                     Prediction prediction) {
    // TODO: need to add attributes
    auto stop = stops.find(stopId);
    if (stop == stops.end()) {
      stop =
          stops.insert(std::make_pair(stopId, Stop(stopId, stopTitle))).first;
    }
    auto route = stop->second.routes.find(routeTag);
    if (route == stop->second.routes.end()) {
      route =
          stop->second.routes
              .insert(std::make_pair(routeTag, Route(routeTag, directionName)))
              .first;
    }
    route->second.predictions.push_back(prediction);
    ++_size;
  }

private:
  size_t _size = 0;
};

} // namespace signboard

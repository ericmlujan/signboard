#pragma once

#include <cctype>
#include <cstring>
#include <memory>
#include <string>
#include <vector>

#include "curl/curl.h"

namespace signboard {
// A simple dynamically allocated buffer that contains bytes of data
// Note that this data structure will grow monotonically in size until
// destructed
struct MemoryBuffer {
  char *data;
  size_t size;

  MemoryBuffer() {
    size = 0;
    data = (char *)malloc(1);
    data[size] = 0;
  }

  ~MemoryBuffer() { free(data); }

  void append(const char *c, size_t bytes) {
    // resize dynamically to get the new information
    data = (char *)realloc(data, size + bytes + 1);
    // couldn't allocate memory
    if (data == nullptr) {
      return;
    }
    memcpy(&data[size], c, bytes);
    size += bytes;
    // null-terminate things of course
    data[size] = 0;
  }
  std::string toString() { return std::string(data); }

  void append(const std::string &str) { this->append(str.c_str(), str.size()); }
};

inline size_t HandleHTTPResponse(char *bytes, size_t size, size_t nmemb,
                                 void *out) {
  MemoryBuffer *buf = (MemoryBuffer *)out;
  size_t adjustedSize = size * nmemb;
  if (!buf) {
    return 0;
  }

  buf->append(bytes, adjustedSize);
  return adjustedSize;
}

inline size_t GetHTTP(const std::string &url, std::string *out) {
  auto handle = curl_easy_init();
  MemoryBuffer buf;
  size_t httpResponse = 0;

  struct curl_slist *list;
  list = curl_slist_append(NULL, "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10.14; rv:66.0) Gecko/20100101 Firefox/66.0");

  curl_easy_setopt(handle, CURLOPT_URL, url.c_str());
  curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, HandleHTTPResponse);
  curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void *)&buf);
  curl_easy_setopt(handle, CURLOPT_HTTPHEADER, list);

  // Perform the HTTP request
  CURLcode result = curl_easy_perform(handle);
  if (result != CURLE_OK) {
    fprintf(stderr, "error in performing HTTP request: %s\n",
            curl_easy_strerror(result));
  } else {
    *out = buf.toString();
    curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &httpResponse);
  }
  curl_easy_cleanup(handle);
  return httpResponse;
};

// Find the index of the first occurence of a substring within a string if one
// exists Returns size(string) if one does not exist. Uses linear find, runs in
// O(size(string))
inline size_t IndexOfSubstring(const std::string &substr,
                               const std::string &string) {
  size_t windowSize = substr.size();
  // If the substring is larger than the query string, no way it can be
  // contained
  if (windowSize > string.size()) {
    return string.size();
  }
  // If they're the same size, they have to be equal
  if (windowSize == string.size()) {
    return substr == string ? 0 : windowSize;
  }
  // Iterate through the larger string with a rolling window
  for (size_t i = 0; i + windowSize < string.size(); ++i) {
    std::string slice = string.substr(i, windowSize);
    if (slice == substr) {
      return i;
    }
  }
  return string.size();
}

// Remove the first occurence of a substring from a string, modifying the string
// passed in by pointer
inline void RemoveSubstring(const std::string &substr, std::string *string) {
  size_t pos = IndexOfSubstring(substr, *string);
  if (pos != string->size()) {
    *string = string->substr(pos + substr.size(), string->size());
  }
}

inline std::string ToUpper(const std::string &str) {
  std::string res;
  for (const auto &ch : str) {
    res += std::toupper(ch);
  }
  return res;
}

template<typename ...Args>
std::string Format(const std::string& formatString, Args... args) {
  size_t size = snprintf(nullptr, 0, formatString.c_str(), args...) + 1;
  std::unique_ptr<char[]> buf(new char[size]);
  snprintf(buf.get(), size, formatString.c_str(), args...);
  return std::string(buf.get(), buf.get() + size - 1);
}

} // namespace signboard

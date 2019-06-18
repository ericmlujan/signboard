#include "util.h"

#include <gtest/gtest.h>

#include <fstream>

using namespace signboard;

// Test fixture for an XML document from NextBus
class XMLTest : public ::testing::Test {
protected:
  std::string _text = "";

public:
  XMLTest() {
    std::ifstream xmlStream("test/data/nextmuni.xml");
    if (xmlStream.is_open()) {
      std::string buf;
      while (xmlStream) {
        getline(xmlStream, buf);
        _text += buf;
      }
      xmlStream.close();
    }
  }

  std::string text() { return _text; }
};

TEST(TestMemoryBuffer, init) {
  MemoryBuffer buf;
  EXPECT_EQ(buf.size, 0);
  EXPECT_EQ(buf.toString(), "");
}

TEST(TestMemoryBuffer, append) {
  MemoryBuffer buf;
  buf.append("abcd", 4);
  EXPECT_EQ(buf.size, 4);
  EXPECT_EQ(buf.toString(), "abcd");
}

TEST(TestMemoryBuffer, multiAppend) {
  MemoryBuffer buf;
  buf.append("foo", 3);
  buf.append("bar", 3);
  EXPECT_EQ(buf.size, 6);
  EXPECT_EQ(buf.toString(), "foobar");
}

TEST(TestMemoryBuffer, appendStdString) {
  MemoryBuffer buf;
  buf.append("hello");
  EXPECT_EQ(buf.size, 5);
  EXPECT_EQ(buf.toString(), "hello");
}

TEST(TestMemoryBuffer, appendEmpty) {
  MemoryBuffer buf;
  buf.append("");
  EXPECT_EQ(buf.size, 0);
  EXPECT_EQ(buf.toString(), "");
}

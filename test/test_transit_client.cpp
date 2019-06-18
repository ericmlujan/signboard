#include <gtest/gtest.h>
#include "transit_client.h"

#include <fstream>

// Set up a fixture for testing a thingo
namespace {
  class TestTransitClient : public ::testing::Test{
  public:
    TestTransitClient() : transitClient("http://webservices.nextbus.com/service/publicXMLFeed", "sf-muni") {
      std::ifstream xmlStream("test/data/nextmuni.xml");
      if (xmlStream.is_open()) {
        std::string buf;
        while (!xmlStream.eof()) {
          std::getline(xmlStream, buf);
          testData += buf;
        }
      }
    }

    signboard::TransitClient transitClient;
    std::string testData = "";
  };
}

TEST_F(TestTransitClient, testMuniParser) {
  ASSERT_GT(testData.size(), 0);

  signboard::Predictions predictions;
  transitClient.parseNextMuniResponse(&testData[0], &predictions);

  ASSERT_EQ(predictions.stops.size(), 1);
  ASSERT_EQ(predictions.stops.at("5857").routes.size(), 2);
  ASSERT_EQ(predictions.size(), 7);
}

TEST_F(TestTransitClient, muniParserWithEmptyString) {
  signboard::Predictions predictions;
  transitClient.parseNextMuniResponse("", &predictions);
  ASSERT_EQ(predictions.size(), 0);
}

TEST_F(TestTransitClient, muniParserWithMalformedXML) {
  signboard::Predictions predictions;
  std::string badData = "<dy ";

  transitClient.parseNextMuniResponse(badData, &predictions);
  ASSERT_EQ(predictions.size(), 0);
}


int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

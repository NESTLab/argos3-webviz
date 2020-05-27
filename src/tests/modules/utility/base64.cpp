#include "plugins/simulator/visualizations/webviz/utility/base64.h"

#include "gtest/gtest.h"

TEST(UtilityBase64, StringEncode) {
  std::string out;

  Base64::Encode("ABCD", &out);
  EXPECT_EQ(out, "QUJDRA==");
};

/****************************************/
/****************************************/

TEST(UtilityBase64, StringDecode) {
  std::string out;

  Base64::Decode("QUJDRA==", &out);
  EXPECT_EQ(out, "ABCD");
};

/****************************************/
/****************************************/

TEST(UtilityBase64, UTFEightEncode) {
  std::string out;

  Base64::Encode("✓ à la mode 😁 😠 © 4⃣", &out);
  EXPECT_EQ(out, "4pyTIMOgIGxhIG1vZGUg8J+YgSDwn5igIMKpIDTig6M=");
};

/****************************************/
/****************************************/

TEST(UtilityBase64, UTFEightDecode) {
  std::string out;

  Base64::Decode("4pyTIMOgIGxhIG1vZGUg8J+YgSDwn5igIMKpIDTig6M=", &out);
  EXPECT_EQ(out, "✓ à la mode 😁 😠 © 4⃣");
};

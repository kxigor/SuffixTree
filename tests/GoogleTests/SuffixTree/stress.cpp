#include <gtest/gtest.h>

#include <cstddef>

#include "../Helpers/TestGenerator.hpp"
#include "include/SuffixTree.hpp"
#include "include/SuffixTreeVeriefer.hpp"

static constexpr const std::size_t kLeftTestingLength = 1;
static constexpr const std::size_t kRightTestingLength = 200;
static constexpr const float kBasicRepeatCoef = 0.5;
static constexpr const float kStressRepeatCoef = 1;

class VariableLengthTest
    : public ::testing::TestWithParam<std::tuple<std::size_t, float>> {
 protected:
  void SetUp() override {
    length_ = std::get<0>(GetParam());
    coef_ = std::get<1>(GetParam());
  }
  std::size_t length_;
  float coef_;
};

TEST_P(VariableLengthTest, Basic) {
  StringRandomGenerator gen(coef_);
  const auto kFirst = gen.GenerateString(length_);
  const auto kSecond = gen.GenerateString(length_);
  suffix_tree::SuffixTree tree(kFirst + kSecond);
  suffix_tree::SuffixTreeVerifier verifier(tree);
  ASSERT_TRUE(verifier.Verify());
}

INSTANTIATE_TEST_SUITE_P(
    LengthAndCoefVariations, VariableLengthTest,
    ::testing::Combine(::testing::Range<std::size_t>(kLeftTestingLength,
                                                     kRightTestingLength),
                       ::testing::Values(kBasicRepeatCoef, kStressRepeatCoef)));

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
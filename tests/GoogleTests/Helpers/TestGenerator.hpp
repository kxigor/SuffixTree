#include <iostream>
#include <random>
#include <string>

class StringRandomGenerator {
 public:
  explicit StringRandomGenerator(double repeat_coef = 0.5)
      : repeat_coef_(repeat_coef) {
    rng_.seed(std::random_device{}());
  }

  std::string GenerateString(std::size_t length) {
    std::string result;
    char last_char = '\0';

    for (std::size_t i = 0; i < length; ++i) {
      if (last_char != '\0' && RandomDouble() < repeat_coef_) {
        result += last_char;
      } else {
        last_char = RandomChar();
        result += last_char;
      }
    }

    return result;
  }

 private:
  char RandomChar() {
    static const std::string kChars = "abcdefghijklmnopqrstuvwxyz";
    std::uniform_int_distribution<std::size_t> dist(0, kChars.size() - 1);
    return kChars[dist(rng_)];
  }

  double RandomDouble() {
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    return dist(rng_);
  }

  double repeat_coef_;
  std::mt19937 rng_;
};
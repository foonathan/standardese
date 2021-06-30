#include <vector>

#include "AB.hpp"

namespace standardese::examples::links {

/// This class inherits from [*A]().
class C : public A {
 public:
  /// This constructor takes a [std::vector]()
  C(std::vector<int> X) {}
};

}

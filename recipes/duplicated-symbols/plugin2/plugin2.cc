#include "plugin2.h"

#include <iostream>

#include "common.h"

namespace plugin2 {

void run() {
  std::cout << "# plugin2 run" << std::endl;
  foo();
}

}  // namespace plugin2

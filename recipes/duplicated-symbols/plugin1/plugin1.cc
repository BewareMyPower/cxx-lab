#include "plugin1.h"

#include <iostream>

#include "common.h"

namespace plugin1 {

void run() {
  std::cout << "# plugin1 run" << std::endl;
  foo();
}

}  // namespace plugin1

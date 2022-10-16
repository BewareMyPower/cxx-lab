#include "plugin1.h"
#include "plugin2.h"

int main(int argc, char* argv[]) {
  plugin1::run();
  plugin2::run();
  return 0;
}

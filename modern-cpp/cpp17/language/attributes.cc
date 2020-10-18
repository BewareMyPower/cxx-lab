#include <stdlib.h>
#include <time.h>

#include <iostream>
using namespace std;

int main(int argc, char* argv[]) {
  srand(time(nullptr));
  int n = rand();
  cout << "rand number: " << n << endl;
  switch (n % 3) {
    case 0:
      cout << "mod 3: 0" << endl;
      [[fallthrough]];
    case 1:
      cout << "mod 3: 1" << endl;
      break;
    case 2:
      cout << "mod 3: 2" << endl;
      break;
  }
  return 0;
}

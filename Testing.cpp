#include "Testing.h"

vector<Test *> tests;

//Based off Quilt Example
void runTests() {
  for_each(tests.begin(),tests.end(),Test::check);
}

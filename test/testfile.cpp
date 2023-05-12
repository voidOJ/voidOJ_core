#include "../src/code.hpp"
#include "../src/utils.hpp"

int main () {
    Code code(Code::language::CPP, 1,"testsource.cpp","answer.txt");
    code.compile();
    code.execute();
}
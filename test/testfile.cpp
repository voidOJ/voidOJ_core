#include "../src/code.hpp"
#include "../src/utils.hpp"

int main () {
    Code code(Code::language::CPP, 1,"testsource.cpp","answer.txt");
    code.compile();
    code.execute();
    code.judge();
    std::string result_judge;
     switch (code.oj_status)
    {
    case 0:
        result_judge = "AC";
        break;
    case 1:
        result_judge = "WA";
        break;
    case 2:
        result_judge = "TLE";
        break;
    case 3:
       result_judge = "OLE";
        break;
    case 4:
        result_judge = "MLE";
        break;
    case 5:
        result_judge = "RE";
        break;
    case 6:
        result_judge = "PE";
        break;
    case 7:
        result_judge = "CE";
        break;
    default:
        result_judge = "HACKER!";
        break;
    }
    std::cout<<result_judge<<std::endl;
    getchar();
}
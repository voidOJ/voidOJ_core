#include <json/allocator.h>
#include <json/json.h>
#include <json/value.h>

#include <fstream>

#include "../src/code.hpp"
#include "../src/utils.hpp"

Code::language get_type(std::string type) {
    if (type == "C")
        return Code::language::C;
    else if (type == "CPP")
        return Code::language::CPP;
    else if (type == "RUST")
        return Code::language::RUST;
    else
        return Code::language::UNKNOWN;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << std::endl
                  << "voidoj-cli <json_file>" << std::endl;
        return 127;
    }
    Code code;
    std::ifstream json_file(argv[1], std::ifstream::binary);
    Json::Value json_data;
    json_file >> json_data;

    code.type = get_type(json_data["type"].asString());
    code.id = json_data["id"].asInt();
    code.source_code = json_data["source_code"].asString();
    code.answer = json_data["answer"].asString();
    code.ignore_format = json_data["ignore_format"].asBool();
    code.time_limit = json_data["time_limit"].asInt();
    code.mem_limit = json_data["mem_limit"].asInt();

    code.compile();
    code.execute();
    code.judge();
    std::cout << "The judge result is: ";
    std::string result_judge;
    switch (code.oj_status) {
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
    std::cout << result_judge << std::endl;
    std::cout << "Time usage: " << std::endl
              << "CPU TIME: " << code.cpu_time << " ms  "
              << "TIME: " << code.time << " ms" << std::endl;
    std::cout << "Memory usage: " << code.memory << " KB" << std::endl;
}
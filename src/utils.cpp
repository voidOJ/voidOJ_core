#include "utils.hpp"

int int_execute(std::string command) {
    return system(command.c_str());
}

std::string gen_tempfile_name(std::string suffix) {
    std::string filename = "tempfile_" + gen_time_now() + suffix;
    return filename;
}

std::string gen_time_now() {
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    char buffer[20];
    std::strftime(buffer, sizeof(buffer), "%Y%m%d_%H%M%S", std::localtime(&now_time));
    return std::string(buffer);
}
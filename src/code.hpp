#pragma once

#include <fcntl.h>
#include <sys/ptrace.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <unistd.h>

#include <iostream>

#include "utils.hpp"

class Code {
   public:
    enum language { C,
                    CPP,
                    RUST,
                    PYTHON };

    // Basic variables
    language type;
    int id;
    std::string source_code;
    std::string answer;
    bool ignore_format;
    int time_limit;
    int mem_limit;

    // Variables after compile or interpret (and judge)
    std::string compile_bin;
    std::string compile_output;
    std::string exe_output;
    int compile_return;
    int exe_return;
    int time;
    OJ oj_status;

    // Basic Constructors
    Code(language lang, int id, std::string sc, std::string ans, bool ig_fmt, int t_limit, int m_limit);
    Code(language lang, int id, std::string sc, std::string ans);

    int compile();
    int execute();
    int interpret();
    int judge();
    void judge_diff();
    const char **gen_cp_command();
};
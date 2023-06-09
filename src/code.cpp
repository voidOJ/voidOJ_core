#include "code.hpp"

// Basic Constructors
Code::Code(language lang, int id, std::string sc, std::string ans) : type(lang), id(id), source_code(sc), answer(ans) {
    ignore_format = false;
    time_limit = 1000;
    mem_limit = 1024 * 128;
    mem_tolerance = 3500;
}
Code::Code(language lang, int id, std::string sc, std::string ans, bool ig_fmt, int t_limit, int m_limit) : type(lang), id(id), source_code(sc), answer(ans), ignore_format(ig_fmt), time_limit(t_limit), mem_limit(m_limit) {
    mem_tolerance = 3500;
}

Code::Code() {
    type = Code::language::C;
    id = 0;
    source_code = answer = "";

    ignore_format = true;
    time_limit = 1000;
    mem_limit = 1024 * 128;
    mem_tolerance = 3500;
}

Code::~Code() {
    remove(source_code.c_str());
    remove(answer.c_str());
    remove(compile_bin.c_str());
    remove(compile_output.c_str());
    remove(exe_output.c_str());
}

int Code::judge() {
    if (!compile_return && !exe_return)
        judge_diff();
    else {
        if (compile_return) oj_status = OJ::CE;
        if (!compile_return && exe_return && oj_status != OJ::TLE && oj_status != OJ::MLE && oj_status != OJ::OLE) oj_status = OJ::RE;
        return 0;
    }

    if (oj_status == OJ::AC) {
        if (time > time_limit)
            oj_status = OJ::TLE;
        else
            return 0;
    }
    return 0;
}

int Code::execute() {
    exe_output = gen_tempfile_name(".txt");
    std::string file_path = "./" + compile_bin;
    const char *command[] = {file_path.c_str(), NULL};

    int stdout_fd = dup(STDOUT_FILENO);
    int stderr_fd = dup(STDERR_FILENO);
    int file = open(exe_output.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    dup2(file, STDOUT_FILENO);
    dup2(file, STDERR_FILENO);

    pid_t child_pid = fork();

    if (child_pid == -1) {
        std::cerr << "Unable to fork child process." << std::endl;
        return 1;
    }

    if (child_pid == 0) {
        struct rlimit rl_time;
        rl_time.rlim_cur = 10;
        rl_time.rlim_max = 60;
        setrlimit(RLIMIT_CPU, &rl_time);

        struct rlimit rl_mem;
        rl_mem.rlim_cur = 256 * 1024 * 1024;
        rl_mem.rlim_max = 256 * 1024 * 1024;
        setrlimit(RLIMIT_AS, &rl_mem);

        struct rlimit rl_file;
        rl_file.rlim_cur = 12 * 1024 * 1024;
        rl_file.rlim_max = 100 * 1024 * 1024;
        setrlimit(RLIMIT_FSIZE, &rl_file);

        alarm(10);
        execvp(command[0], (char *const *)command);

        std::cerr << "Failed" << std::endl;
        return 1;
    }
    if (child_pid > 0) {
        int status;
        struct rusage ru;

        auto time_start = std::chrono::high_resolution_clock::now();

        wait4(child_pid, &status, 0, &ru);
        cpu_time = ru.ru_utime.tv_sec * 1000 + ru.ru_utime.tv_usec / 1000 + ru.ru_stime.tv_sec * 1000 + ru.ru_stime.tv_usec / 1000;
        memory = ru.ru_maxrss - mem_tolerance;

        auto time_end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start).count();
        time = duration;

        dup2(stdout_fd, STDOUT_FILENO);
        dup2(stderr_fd, STDERR_FILENO);
        close(file);
        close(stdout_fd);
        close(stderr_fd);

        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            exe_return = 0;
        } else {
            exe_return = 1;
            if (WTERMSIG(status) == SIGXCPU || WTERMSIG(status) == SIGALRM) oj_status = OJ::TLE;
            if (WTERMSIG(status) == SIGSEGV) oj_status = OJ::MLE;
            if (WTERMSIG(status) == SIGXFSZ) oj_status = OJ::OLE;
        }

        return 0;
    }
    return 0;
}

int Code::compile() {
    compile_output = gen_tempfile_name(".txt");
    compile_bin = gen_tempfile_name(".out");
    const char **command = nullptr;

    if (type == C) {
        const char *CP_C[] = {"gcc", source_code.c_str(), "-fno-asm", "-Wall", "-o", compile_bin.c_str(), NULL};
        command = new const char *[7];
        std::copy(CP_C, CP_C + 7, command);
    }

    else if (type == CPP) {
        const char *CP_CPP[] = {"g++", source_code.c_str(), "-fno-asm", "-Wall", "-o", compile_bin.c_str(), NULL};
        command = new const char *[7];
        std::copy(CP_CPP, CP_CPP + 7, command);
    }

    int stdout_fd = dup(STDOUT_FILENO);
    int stderr_fd = dup(STDERR_FILENO);
    int file = open(compile_output.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    dup2(file, STDOUT_FILENO);
    dup2(file, STDERR_FILENO);

    pid_t child_pid = fork();

    if (child_pid == -1) {
        std::cerr << "Unable to fork child process." << std::endl;
        return 1;
    }

    if (child_pid == 0) {
        struct rlimit rl_time;
        rl_time.rlim_cur = 30;
        rl_time.rlim_max = 60;
        setrlimit(RLIMIT_CPU, &rl_time);

        execvp(command[0], (char *const *)command);

        std::cerr << "Failed" << std::endl;
        return 1;
    }

    if (child_pid > 0) {
        int status;
        waitpid(child_pid, &status, 0);

        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            std::cout << "Compiled successfully." << std::endl;
            compile_return = 0;
        } else {
            std::cerr << "Failed to compile." << std::endl;
            compile_return = 1;
        }

        dup2(stdout_fd, STDOUT_FILENO);
        dup2(stderr_fd, STDERR_FILENO);
        close(file);
        close(stdout_fd);
        close(stderr_fd);

        return 0;
    }
    return 0;
}

void Code::judge_diff() {
    std::string diff = "diff -q -b -w -B --strip-trailing-cr " + exe_output + " " + answer;
    if (int_execute(diff) && oj_status != 0) oj_status = OJ::WA;
    diff = "diff -q -B --strip-trailing-cr " + exe_output + " " + answer;
    if (!ignore_format && int_execute(diff) && oj_status != 0)
        oj_status = OJ::PE;
    else
        oj_status = OJ::AC;
}
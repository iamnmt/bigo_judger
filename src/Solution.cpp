#include "Solution.h"
#include "helper.h"
#include "CompileResult.h"
#include "configer.h"

extern configer::configer bconf;

Solution::Solution() {}

Solution::Solution(const std::string& filename, double timeout, bool use_pypy) {
    ext = helper::extension(filename);
    raw = helper::raw_filename(filename);

    if (ext == "cpp" || ext == "cc") {
        compile_cmd = helper::format("g++ -std=c++17 -O2 \"%s\" -o " JUDGE_BIN "\"%s\"", filename.data(), raw.data());
        run_command = JUDGE_BIN "/\"" + raw + "\"";
    }
    else if (ext == "java") {
        compile_cmd = helper::format("javac \"%s\" -d " JUDGE_BIN, filename.data());
        run_command = "java -cp " JUDGE_BIN " \"" + raw + "\"";
        timeout *= 3;
    }
    else if (ext == "py") {
        if (use_pypy) {
            compile_cmd = helper::format("pypy3 -m py_compile \"%s\"", filename.data());
            run_command = "pypy3 \"" + filename + "\"";
        }
        else {
            compile_cmd = helper::format("python3 -m py_compile \"%s\"", filename.data());
            run_command = "python3 \"" + filename + "\"";
        }
        timeout *= 5;
    }
    else if (ext == "js") {
        
    }
    else {
        throw std::runtime_error("Not supported extension: " + ext + "\n");
    }
    run_command +=  " < %s";
    run_command = helper::format("timeout -s SIGABRT %lf ", timeout) + run_command + " 1> %s 2> %s";
}

CompileResult Solution::compile() const {
    std::string compile_cmd = this->compile_cmd + " 2>&1 | grep -i error > " + JUDGE_COMPILE_ERRORS;
    fclose(popen(compile_cmd.data(), "r"));
    return {helper::read_file(JUDGE_COMPILE_ERRORS)};
}

RunResult Solution::run(const std::string& in, const std::string& out, const std::string& err) const {
    std::string run_command = helper::format(this->run_command, in.data(), out.data(), err.data());
    char buff[BUFF_SIZE];
    auto start = std::chrono::system_clock::now();
    FILE* fp = popen(run_command.data(), "r");
    auto ret = fgets(buff, BUFF_SIZE - 1, fp);
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start).count();

    int exit_code = pclose(fp) >> 8;

    std::string error;
    fp = fopen(err.data(), "rt");
    if (fp) {
        char ch;
        while ((ch = getc(fp)) != EOF)
            error += ch;
        fclose(fp);
    }
    return {exit_code, time / 1000.0, error};
}

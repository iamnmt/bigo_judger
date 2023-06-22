#include "Checker.h"
#include "helper.h"
#include "configer.h"

extern configer::configer bconf;

Checker::Checker() {}

Checker::Checker(const std::string& filename, double timeout, const std::string& options): Solution(filename, timeout) {
    if (ext == "cpp" || ext == "cc") {
        run_command = JUDGE_BIN "\"" + raw + "\"";
    } else if (ext == "py") {
        run_command = "pypy3 --version";
        int exit_code = pclose(popen(run_command.data(), "r")) >> 8;
        if (exit_code == PCLOSE_ERROR) {
            run_command = "python3 \"" + filename + "\"";
        } else {
            run_command = "pypy3 \"" + filename + "\"";
        }
    } else {
        throw std::runtime_error("Not supported checker extension: " + ext + "\n");
    }
    run_command += " \"%s\" \"%s\" \"%s\" " + options + " < \"%s\"";
}

CompileResult Checker::compile() const {
    return Solution::compile();
}

int Checker::compare(const std::string& in, const std::string& out, const std::string& ans) const {
    helper::make_dir(JUDGE_FEEDBACK);
    std::string run_command = helper::format(this->run_command, in.data(), ans.data(), JUDGE_FEEDBACK.data(), out.data());

    return pclose(popen(run_command.data(), "r")) >> 8;
}

std::string Checker::check(const std::string& in, const std::string& out, const std::string& ans) const {
    if (compare(in, out, ans) == EXIT_AC) {
        return "";
    }
    return "Diffpos:\n" + helper::read_file(JUDGE_DIFFPOS) + "Message:\n" + helper::read_file(JUDGE_MESSAGE);
}

TestResult Checker::judge(const Solution& sol, const Test& test) const {
    RunResult run_result = sol.run(test.in(), JUDGE_OUTPUT, JUDGE_STDERR);

    if (run_result.exit_code == EXIT_TIMEOUT) {
        return {Verdict::TLE, run_result.time, ""};
    }
    else if (run_result.exit_code != 0) {
        return {Verdict::RE, run_result.time, run_result.error};
    }
    else {
        std::string diff_out_ans = this->check(test.in(), JUDGE_OUTPUT, test.ans());
        if (diff_out_ans == "") {
            return {Verdict::AC, run_result.time, ""};
        }
        else {
            return {Verdict::WA, run_result.time, diff_out_ans};
        }
    }
}

JudgeResult Checker::judge(const Solution& sol, const std::vector<Test>& tests) const {
    JudgeResult sol_result;
    for (auto& test: tests) {
        sol_result[test.name] = this->judge(sol, test);
    }

    return sol_result;
}

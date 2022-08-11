#ifndef DEFAULT_CHECKER
#define DEFAULT_CHECKER
#include "Checker.h"
#include "definitions.h"

struct DefaultChecker: Checker {
    DefaultChecker(const std::string& options = "");
    CompileResult compile() const;
    int compare(const std::string& in, const std::string& out, const std::string& ans) const;

    std::string parse_options(const std::string &options);
    int wrong_answer(int judgeans_line, int judgeans_pos, int team_line, int team_pos, const char *err, ...) const;
    bool case_sensitive, space_change_sensitive, use_floats;
    flt float_abs_tol, float_rel_tol;
};
#endif

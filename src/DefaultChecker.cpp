#include "DefaultChecker.h"
#include "helper.h"

DefaultChecker::DefaultChecker(const std::string& options) {
    float_abs_tol = float_rel_tol = -1;
    std::string error = parse_options(options);
    if (error != "") {
        std::cout << "Invalid option arguments: \"" + error + "\"\n";
        exit(0);
    }
}

CompileResult DefaultChecker::compile() const {
    return {""};
}

int DefaultChecker::compare(const std::string& in, const std::string &out, const std::string& ans) const {
    helper::make_dir(JUDGE_FEEDBACK);

    std::ifstream judgein(in), judgeans(ans), teamout(out);
    int judgeans_pos = 0, team_pos = 0;
    int judgeans_line = 1, team_line = 1;
    std::string judge, team;

    while (true) {
        // Space!  Can't live with it, can't live without it...
        while (isspace(judgeans.peek())) {
            char c = (char)judgeans.get();
            if (space_change_sensitive) {
                int d = teamout.get();
                if (c != d) {
                    return wrong_answer(
                        judgeans_line, judgeans_pos, team_line, team_pos,
                        "Space change error: got %d expected %d", d, c
                    );
                }
                if (d == '\n') ++team_line;
                ++team_pos;
            }
            if (c == '\n') ++judgeans_line;
            ++judgeans_pos;
        }
        while (isspace(teamout.peek())) {
            char d = (char)teamout.get();
            if (space_change_sensitive) {
                return wrong_answer(
                    judgeans_line, judgeans_pos, team_line, team_pos,
                    "Space change error: judge out of space, got %d from team", d
                );
            }
            if (d == '\n') ++team_pos;
            ++team_pos;
        }

        if (!(judgeans >> judge))
            break;

        if (!(teamout >> team)) {
            return wrong_answer(
                judgeans_line, judgeans_pos, team_line, team_pos,
                "User EOF while judge had more output\n(Next judge token: \"%s\")", judge.c_str()
            );
        }

        flt jval, tval;
        if (use_floats && helper::is_float(judge.c_str(), jval)) {
            if (!helper::is_float(team.c_str(), tval)) {
                return wrong_answer(
                    judgeans_line, judgeans_pos, team_line, team_pos,
                    "Expected float, got: %s", team.c_str()
                );
            }
            if (!helper::equal(tval, jval, float_abs_tol, float_rel_tol)) {
                return wrong_answer(
                    judgeans_line, judgeans_pos, team_line, team_pos,
                    "Too large difference.\n Judge: %s\n Team: %s\n Difference: %Lg\n (abs tol %Lg rel tol %Lg)",
                    judge.c_str(), team.c_str(), fabsl(jval - tval), float_abs_tol, float_rel_tol
                );
            }
        } else if (case_sensitive) {
            if (strcmp(judge.c_str(), team.c_str()) != 0) {
                return wrong_answer(
                    judgeans_line, judgeans_pos, team_line, team_pos,
                    "String tokens mismatch\nJudge: \"%s\"\nTeam: \"%s\"",
                    judge.c_str(), team.c_str()
                );
            }
        } else {
            if(strcasecmp(judge.c_str(), team.c_str()) != 0) {
                return wrong_answer(
                    judgeans_line, judgeans_pos, team_line, team_pos,
                    "String tokens mismatch\nJudge: \"%s\"\nTeam: \"%s\"", judge.c_str(), team.c_str());
            }
        }
        judgeans_pos += judge.length();
        team_pos += team.length();
    }

    if (teamout >> team) {
        return wrong_answer(
            judgeans_line, judgeans_pos, team_line, team_pos,
            "Trailing output:\n\"%s\"", team.c_str()
        );
    }
    return EXIT_AC;
}

std::string DefaultChecker::parse_options(const std::string& options) {
    std::istringstream iss(options);
    std::vector<std::string> argv;
    std::string arg;
    while (iss >> arg) {
        argv.emplace_back(arg);
    };
    int argc = argv.size();

    for (int i = 0; i < argc; i++) {
        if (argv[i] == "case_sensitive") {
            case_sensitive = true;
        }
        else if (argv[i] == "space_change_sensitive") {
            space_change_sensitive = true;
        }
        else if (argv[i] == "float_absolute_tolerance") {
            if (i + 1 == argc || !helper::is_float(argv[i + 1].data(), float_abs_tol))
                return argv[i];
            ++i;
        }
        else if (argv[i] == "float_relative_tolerance") {
            if (i + 1 == argc || !helper::is_float(argv[i + 1].data(), float_rel_tol))
                return argv[i];
            ++i;
        }
        else if (argv[i] == "float_tolerance") {
            if (i + 1 == argc || !helper::is_float(argv[i + 1].data(), float_rel_tol))
                return argv[i];
            float_abs_tol = float_rel_tol;
            ++i;
        }
        else {
            return argv[i];
        }
    }

    use_floats = float_abs_tol >= 0 || float_rel_tol >= 0;

    return "";
}

int DefaultChecker::wrong_answer(int judgeans_line, int judgeans_pos, int team_line, int team_pos, const char *err, ...) const {
    FILE* judgemessage = fopen((JUDGE_FEEDBACK + "/judgemessage.txt").data(), "wt");
    FILE* diffpos = fopen((JUDGE_FEEDBACK + "/diffposition.txt").data(), "wt");

    va_list pvar;
    va_start(pvar, err);
    fprintf(judgemessage, "Wrong answer on line %d of output (corresponding to line %d in answer file)\n",
            team_line, judgeans_line);
    vfprintf(judgemessage, err, pvar);
    fprintf(judgemessage, "\n");
    if (diffpos) {
        fprintf(diffpos, "%d %d", judgeans_pos, team_pos);
    }

    fclose(judgemessage);
    fclose(diffpos);

    return EXIT_WA;
}

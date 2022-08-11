#ifndef HELPER
#define HELPER
#include <bits/stdc++.h>
#include "Test.h"
#include "definitions.h"

namespace std {
    std::string to_string(const std::thread::id& id);
}

namespace helper {
    std::string extension(const std::string& name);

    std::string raw_filename(const std::string& name);

    std::string read_file(const std::string& file);

    std::string diff(const std::string& a, const std::string& b);

    std::vector<Test> get_tests(const std::string& test_path, const std::string& inp_ext, const std::string& ans_ext);

    template <class... T>
    std::string format(const char* fmt, const T&... args) {
        char buff[BUFF_SIZE];
        sprintf(buff, fmt, args...);
        return buff;
    }

    template <class... T>
    std::string format(const std::string& fmt, const T&... args) {
        return format(fmt.data(), args...);
    }
    
    void make_dir(const std::string& dir);
    void make_dirs();
    void clean();

    bool is_float(const char* s, flt &val);
    bool is_int(const char* s, int& val);
    int equal(flt f1, flt f2, flt float_abs_tol, flt float_rel_tol);
}
#endif

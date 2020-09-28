/* CMD sort
 */
#include <iostream>
#include <functional>
#include <fstream>
#include <cstring>
#include <clocale>


class cmd_sort {
    class lines_vec {
        struct object {
            std::string read;
            int int_;
            std::string upper{""};
            object(const std::string & rread) : read(rread), int_(std::atoi(rread.data())) {}
        };
        std::vector<object> lines;

        enum sort_type {upper, numeric, def};
        void sort( sort_type type ) {
            auto sort_up = [] (const object & a, const object & b) { return a.upper < b.upper; };
            auto sort_def = [] (const object & a, const object & b) { return a.read < b.read; };
            auto sort_num = [] (const object & a, const object & b) { return a.int_ < b.int_; };
            switch (type) {
                case upper:
                    std::sort(lines.begin(), lines.end(), sort_up);
                    break;
                case numeric:
                    std::sort(lines.begin(), lines.end(), sort_num);
                    break;
                default:
                    std::sort(lines.begin(), lines.end(), sort_def);
                    break;
            }
        }
    public:
        void sort_upper() {
            for (auto & str : lines) {
                str.upper.reserve(str.read.size());
                for (const char symbol : str.read)
                    str.upper += std::toupper(symbol);
            }
            sort(upper);
        }
        void sort_numeric() { sort(numeric); }
        void sort_default() { sort(def); }
        void print() { std::for_each(lines.begin(), lines.end(), [] (auto a) { std::cout << a.read << "\n"; }); }
        void emplace_back( const std::string & s ) {
            lines.emplace_back(s);
        }
        bool all_zero() {
            bool all_zero = true;
            std::for_each(lines.begin(), lines.end(), [&all_zero] (auto a) { if (a.int_ != 0) { all_zero = false; return; }});
            return all_zero;
        }
    };
public:
    static void sort_stream(std::istream & input, bool upper_case, bool numeric )
    {
        lines_vec lines;
        // read lines
        std::string line;
        while (std::getline(input, line)) {
            lines.emplace_back(line);
        }

        bool all_zero = false;
        if (numeric) {
            lines.sort_numeric();
            all_zero = lines.all_zero();
            if (!all_zero) {
                lines.print();
                return;
            }
        }
        if (all_zero && upper_case) {
            lines.sort_default();
            lines.print();
            return;
        } else if (upper_case) {
            lines.sort_upper();
        } else
            lines.sort_default();

        lines.print();
    }
};

int main(int argc, char ** argv)
{
    bool upper_case = false;
    bool numeric = false;
    const char * input_name = nullptr;
    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] == '-') {
            if (argv[i][1] != '-') {
                const size_t len = std::strlen(argv[i]);
                for (size_t j = 1; j < len; ++j) {
                    switch (argv[i][j]) {
                        case 'f':
                            upper_case = true;
                            break;
                        case 'n':
                            numeric = true;
                            break;
                    }
                }
            }
            else {
                if (std::strcmp(argv[i], "--ignore-case") == 0) {
                    upper_case = true;
                }
                else if (std::strcmp(argv[i], "--numeric-sort") == 0) {
                    numeric = true;
                }
            }
        }
        else {
            input_name = argv[i];
        }
    }
    if (input_name != nullptr) {
        std::ifstream f(input_name);
        cmd_sort::sort_stream(f, upper_case, numeric);
    }
    else {
        cmd_sort::sort_stream(std::cin, upper_case, numeric);
    }

    return 0;
}

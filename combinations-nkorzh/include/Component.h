#pragma once

#include <ctime>
#include <istream>
#include <string>
#include <variant>
#include <sstream>

enum class InstrumentType : char
{
    C = 'C',
    F = 'F',
    O = 'O',
    P = 'P',
    U = 'U',
    Unknown = '\0'
};

enum class timePeriod : int {
    quorter = 93,
    month = 31,
    year = 372,
    day = 1
};


class ttime : public std::tm {
    /* necessary for comparisons only */
    long long get_days() const;
    long long get_days( const ttime &) const;
public:
    bool operator==(const ttime &) const;
    bool operator!=(const ttime &) const;
    bool operator<(const ttime &) const;
    bool operator>(const ttime &) const;
};

struct xmlComponent {
    using str = const std::string;

    InstrumentType type{InstrumentType::Unknown};
    std::variant<double, char> ratio{0.0};
    char strike{0};
    char expiration{0};
    int strike_offset{0};
    std::variant<std::pair<int, int>, int> expiration_offset{std::make_pair(0, 0)}; // counted in days

    xmlComponent() = default;
    xmlComponent( str & _type, str & _ratio, str & _strike, str & _exp, str & _strike_offset, str & _exp_offset );

    void init( str & _type, str & _ratio, str & _strike, str & _exp,
            str & _strike_offset, str & _exp_offset );
    static int count_pluses(const str & s);
};

struct Component {
    static Component from_stream( std::istream & );
    static Component from_string( const std::string & );

    InstrumentType type{InstrumentType::Unknown};
    double ratio{0};
    double strike{0};
    ttime expiration;
    int strike_offset{0};
    std::variant<int, std::pair<int, int>> expiration_offset{0};
    int personal_id{-1};
    int second_id{-1};

    long long get_exp_in_days() const;
    static long long get_exp_in_days( const class ttime & );

    bool operator<( const Component &b ) const;
};

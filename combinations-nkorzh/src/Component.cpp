#include "Component.h"

#include <iomanip>
#include <cmath>

#define EPSILON 1E-5
#define EQUAL_DOUBLE(A, B) (fabs((A) - (B)) < EPSILON)

Component Component::from_stream(std::istream & strm)
{
    Component component;

    char type = '\0';
    strm >> type;
    if (strm.fail()) {
        return {};
    }
    bool read_strike = false;
    component.type = static_cast<InstrumentType>(type);
    switch (component.type) {
        case InstrumentType::C: [[fallthrough]];
        case InstrumentType::O: [[fallthrough]];
        case InstrumentType::P:
            read_strike = true;
            break;
        case InstrumentType::F: [[fallthrough]];
        case InstrumentType::U:
            break;
        case InstrumentType::Unknown: [[fallthrough]];
        default:
            return {};
    }

    strm >> component.ratio;
    if (strm.fail()) {
        return {};
    }

    if (read_strike) {
        strm >> component.strike;
        if (strm.fail()) {
            return {};
        }
    }

    strm >> std::get_time(&component.expiration, "%Y-%m-%d");

    component.expiration.tm_yday = component.expiration.tm_mon * 31 + component.expiration.tm_mday;

    if (strm.fail()) {
        return {};
    }

    return component;
}

Component Component::from_string(const std::string & str)
{
    std::istringstream strm{str};
    return from_stream(strm);
}

long long Component::get_exp_in_days() const {
    return get_exp_in_days(expiration);
}

long long Component::get_exp_in_days( const ttime & time) {
    return time.tm_year * 365 + time.tm_yday;
}

bool Component::operator<( const Component &b ) const {
    if (type != b.type)
        return type < b.type;
    else if (!EQUAL_DOUBLE(ratio, b.ratio))
        return ratio < b.ratio;
    else if (!EQUAL_DOUBLE(strike, b.strike))
        return strike < b.strike;
    else {
        long long fir = get_exp_in_days(), sec = b.get_exp_in_days();
        if (fir != sec)
            return fir < sec;
    }
    return false;
}

bool ttime::operator==( const ttime & b ) const {
    return this->tm_year == b.tm_year && this->tm_mon == b.tm_mon && this->tm_mday == b.tm_mday;
}

bool ttime::operator!=( const ttime & b ) const {
    return this->tm_year != b.tm_year || this->tm_mon != b.tm_mon || this->tm_mday != b.tm_mday;
}

bool ttime::operator<( const ttime & b) const {
    return get_days() < get_days(b);
}

bool ttime::operator>( const ttime & b) const {
    return get_days() > get_days(b);
}

long long ttime::get_days() const {
    return this->tm_year * 366 + this->tm_mon * 31 + this->tm_mday;
}

long long ttime::get_days(const ttime & b) const {
    return b.tm_year * 366 + b.tm_mon * 31 + b.tm_mday;
}

int xmlComponent::count_pluses( const str &s ) {
    int res = s.size();
    if (s.front() == '+')
        return res;
    else if (s.front() == '-')
        return -res;
    return 0;
}

xmlComponent::xmlComponent( xmlComponent::str &_type, xmlComponent::str &_ratio, xmlComponent::str &_strike,
                            xmlComponent::str &_exp, xmlComponent::str &_strike_offset,
                            xmlComponent::str &_exp_offset ) {
    init(_type, _ratio, _strike, _exp, _strike_offset, _exp_offset);
}

void xmlComponent::init( xmlComponent::str &_type, xmlComponent::str &_ratio, xmlComponent::str &_strike,
                         xmlComponent::str &_exp, xmlComponent::str &_strike_offset, xmlComponent::str &_exp_offset ) {
    type = static_cast<InstrumentType>(_type[0]);
    if (_ratio == "+" || _ratio == "-")
        ratio = _ratio[0];
    else {
        double number = 0;
        std::stringstream ratio_stream(_ratio);
        ratio_stream >> number;
        ratio = std::move(number);
    }
    strike = _strike[0];
    expiration = _exp[0];
    strike_offset = count_pluses(_strike_offset);

    if (_exp_offset[0] == '-' || _exp_offset[0] == '+')
        expiration_offset = count_pluses(_exp_offset);
    else {
        char time_period = 'a';
        int amount = 1;
        if ('0' <= _exp_offset[0] && _exp_offset[0] <= '9') {
            std::stringstream offset_string(_exp_offset);
            offset_string >> amount >> time_period;
        } else
            time_period = _exp_offset[0];
        double range;
        switch (time_period) {
            case 'm':
                range = static_cast<double>(timePeriod::month);
                break;
            case 'y':
                range = static_cast<double>(timePeriod::year);
                break;
            case 'd':
                range = static_cast<double>(timePeriod::day);
                break;
            case 'q':
                range = static_cast<double>(timePeriod::quorter);
                break;
            default:
                range = 0;
        }
        expiration_offset = std::make_pair(range * amount, range * (amount + (time_period == 'q' ? 1 : 0)));
    }
}

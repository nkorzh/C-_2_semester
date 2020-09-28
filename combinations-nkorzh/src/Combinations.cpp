#include "Combinations.h"

#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <set>

#define EPSILON 1E-5
#define EQUAL_DOUBLE(A, B) (fabs((A) - (B)) < EPSILON)

static double date_difference_in_days( const ttime & t1, const ttime & t2 ) {
    return static_cast<double>(Component::get_exp_in_days(t1) - Component::get_exp_in_days(t2));
}

bool Combinations::load(const std::filesystem::path & resource)
{
    pugi::xml_document doc;

    if (!std::filesystem::exists(resource))
        return false;

    if (doc.load_file(resource.c_str()).status != pugi::xml_parse_status::status_ok)
        return false;

    auto combinations = doc.first_child();

    for (const auto & comb_root : combinations) {
        combination_types.emplace_back();
        combination_types.back().load(comb_root);
    }
    return true;
}

std::string Combinations::classify(const std::vector<Component> & components, std::vector<int> & order) const
{
    order.clear();
    std::vector<Component> original_seq(components);
    std::vector<Component> compressed_seq;
    compressCombination(components, compressed_seq);

    for (int i = 0; i < original_seq.size(); ++i)
        original_seq[i].personal_id = i + 1;
    for (int i = 0; i < compressed_seq.size(); ++i)
        compressed_seq[i].personal_id = i + 1;

    for (auto & combination : combination_types) {
        resetPermutations(compressed_seq);
        do {
            int cmp_result = combination.cmpCombination(compressed_seq);

            if (cmp_result == 0) {
                if (!checkFullCombination(original_seq, combination)) {
                    resetPermutations(original_seq);
                    break;
                }
                order.reserve(original_seq.size());
                for (int i = 0; i < original_seq.size(); ++i)
                    original_seq[i].second_id = i + 1;
                resetPermutations(original_seq);
                std::for_each(original_seq.begin(), original_seq.end(), [&order]( const Component &element ) {
                    order.push_back(element.second_id);
                });
                return combination.getName();
            } else if (cmp_result == -1)
                break;
        } while (getNextPermutation(compressed_seq));
    }
    return "Unclassified";
}

void Combinations::compressCombination( const std::vector<Component> & components,
        std::vector<Component> & compressedCombination ) const {
    std::set<Component> components_set;

    std::for_each(components.begin(), components.end(), [&components_set] (const Component & c) {
        components_set.emplace(c);
    });
    compressedCombination.clear();
    compressedCombination.reserve(components_set.size());

    std::for_each(components_set.begin(), components_set.end(), [&compressedCombination] (const Component & c) {
        compressedCombination.emplace_back(c);
    });
}

bool Combinations::checkFullCombination( std::vector<Component> &components, const Combination & combination ) const {
    do {
        int cmp_result = combination.cmpCombination(components);
        if (cmp_result == 0) {
            return components.size() >= combination.getMinCount();
        } else if (cmp_result == -1)
            break;
    } while (getNextPermutation(components));
    return false;
}

bool Combinations::getNextPermutation( std::vector<Component> & sequence ) const {
    return std::next_permutation(sequence.begin(), sequence.end(), component_id_comparator);
}

void Combinations::resetPermutations( std::vector<Component> &sequence ) const {
    std::sort(sequence.begin(), sequence.end(), component_id_comparator);
}

int Combination::cmpCombination( std::vector<Component> & seq ) const {
        bool first_loop = true;
        int i = 0;

        std::unordered_map<char, double> strike_set;
        std::unordered_map<char, long long> expiration_set;

        if (cardinality == cardinalityType::fixed && components.size() != seq.size())
            return -1;

        for (auto & component : seq) {
            component.strike_offset = 0;
            component.expiration_offset = 0;
        }

        while (first_loop || (i != seq.size() && (cardinality == cardinalityType::more || cardinality == cardinalityType::multiple))) {
            for (auto & leg : components) {
                if (i >= seq.size())
                    return -1;
                /***
                 * compulsory attributes
                 ***/
                if (leg.type != seq[i].type)
                    if (leg.type != InstrumentType::O || (seq[i].type != InstrumentType::P && seq[i].type != InstrumentType::C))
                        return 1;

                if (auto cur_ratio = std::get_if<char>(&leg.ratio)) {
                    if ((*cur_ratio == '+' && seq[i].ratio < 0) ||
                        (*cur_ratio == '-' && seq[i].ratio > 0))
                        return 1;
                } else {
                    double ratio = std::get<double>(leg.ratio);
                    if (!EQUAL_DOUBLE(seq[i].ratio, ratio))
                        return 1;
                }
                /***
                 * optional attributes
                 ***/
                /*
                 * checking strike & strike offset for options
                 */
                if (leg.type == InstrumentType::C || leg.type == InstrumentType::O
                    || leg.type == InstrumentType::P) {
                    /* check strike value */
                    if (leg.strike != 0) {
                        auto strike_from_set = strike_set.find(leg.strike);
                        if (strike_from_set._M_cur == nullptr)
                            strike_set.emplace(leg.strike, seq[i].strike);
                        else if (!EQUAL_DOUBLE(strike_from_set->second, seq[i].strike))
                            return 1;
                    }
                    seq[i].strike_offset = leg.strike_offset;
                    if (leg.strike_offset != 0) {
                        bool strike_correct = false;
                        for (int prev_comp = i - 1; prev_comp >= 0; prev_comp--) {
                            if (seq[i].strike_offset != 0 && seq[prev_comp].strike_offset == seq[i].strike_offset)
                                if (!EQUAL_DOUBLE(seq[prev_comp].strike, seq[i].strike))
                                    return 1;

                            if (abs(seq[prev_comp].strike_offset - seq[i].strike_offset) == 1)
                                if ((seq[prev_comp].strike < seq[i].strike &&
                                     seq[i].strike_offset - seq[prev_comp].strike_offset == 1) ||
                                    (seq[prev_comp].strike > seq[i].strike &&
                                     seq[i].strike_offset - seq[prev_comp].strike_offset == -1)) {
                                    strike_correct = true;
                                    break;
                                }
                        }
                        if (!strike_correct)
                            return 1;
                    }
                }
                /*
                 * checking expiration
                 */
                if (leg.expiration != 0) {
                    auto exp_from_set = expiration_set.find(leg.expiration);
                    if (exp_from_set._M_cur == nullptr)
                        expiration_set.emplace(leg.expiration, Component::get_exp_in_days(seq[i].expiration));
                    else if (exp_from_set->second != Component::get_exp_in_days(seq[i].expiration))
                        return 1;
                }
                /*
                 * checking expiration offset
                 */
                if (auto exact_exp_offset = std::get_if<int>(&leg.expiration_offset)) {
                    bool offset_correct = false;
                    if (*exact_exp_offset != 0) {
                        seq[i].expiration_offset = *exact_exp_offset;
                        for (int prev_comp = i - 1; prev_comp >= 0; prev_comp--) {
                            if (seq[i].expiration_offset == seq[prev_comp].expiration_offset)
                                if (seq[i].expiration != seq[prev_comp].expiration)
                                    return 1;
                            if (typeid(seq[prev_comp].expiration_offset) == typeid(std::pair<int, int>))
                                continue; // looking for component with undefined offset
                            int prev = std::get<int>(seq[prev_comp].expiration_offset),
                                    cur = std::get<int>(seq[i].expiration_offset);
                            if (abs(prev - cur) == 1)
                                if ((seq[prev_comp].expiration < seq[i].expiration &&
                                     cur - prev == 1) ||
                                    (seq[prev_comp].expiration > seq[i].expiration &&
                                     cur - prev == -1)) {
                                    offset_correct = true;
                                    break;
                                }
                        }
                        if (!offset_correct)
                            return 1;
                    }
                } else {
                    auto range = std::get<std::pair<int, int>>(leg.expiration_offset);
                    bool offset_correct = false;
                    if (range.first != range.second && range.first != 0) {
                        seq[i].expiration_offset = range;
                        for (int rind = i - 1; rind >= 0; rind--) {
                            if (typeid(seq[rind].expiration_offset) == typeid(std::pair<int, int>)) {
                                if (seq[i].expiration_offset == seq[rind].expiration_offset &&
                                    seq[i].expiration != seq[rind].expiration)
                                    return 1;
                            } else {
                                if (auto prev = std::get_if<int>(&seq[rind].expiration_offset))
                                    if (*prev == 0) {
                                        double date_difference = date_difference_in_days(seq[i].expiration,
                                                                                         seq[rind].expiration);
                                        if (range.first <= date_difference &&
                                            date_difference <= range.second) {
                                            offset_correct = true;
                                            break;
                                        }
                                        break;
                                    }
                            }
                        }
                        if (!offset_correct)
                            return 1;
                    }
                }

                i++;
            }
            first_loop = false;
        }
        if (i < seq.size())
            return -1;
        return 0;
    }

Combination::Combination() : mincount(0), name(""), cardinality(cardinalityType::unknown) {}

bool Combination::load( const node & combination_root ) {
    name = combination_root.attribute("name").value();
    node legs = combination_root.child("legs");
    string type = legs.attribute("cardinality").value();

    cardinality = type == "fixed" ? cardinalityType::fixed :
                  (type == "more" ? cardinalityType::more :
                   (type == "multiple" ? cardinalityType::multiple :
                    cardinalityType::unknown));

    mincount = cardinality == cardinalityType::more ?
               std::stoi(legs.attribute("mincount").value()) : 0;

    for (auto & leg : legs) {
        components.emplace_back(xmlComponent(leg.attribute("type").value(),
                                             leg.attribute("ratio").value(),
                                             leg.attribute("strike").value(),
                                             leg.attribute("expiration").value(),
                                             leg.attribute("strike_offset").value(),
                                             leg.attribute("expiration_offset").value()));

    }
    return true;
}

std::string Combination::getName() const {
    return name;
}

int Combination::getMinCount() const {
    return mincount;
}

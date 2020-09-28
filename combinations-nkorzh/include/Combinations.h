#pragma once

#include <filesystem>
#include <string>
#include <vector>
#include <functional>
#include <pugixml.hpp>

#include "Component.h"

class Combination {
    enum cardinalityType {fixed, more, multiple, unknown};
    using node = pugi::xml_node;
    using string = std::string;

    std::vector<xmlComponent> components;
    cardinalityType cardinality;
    int mincount;
    string name;
public:
    Combination();

    bool load( const node & combination_root );
    int cmpCombination( std::vector<Component> & seq ) const;

    std::string getName() const;
    int getMinCount() const;
};


class Combinations
{
    using node = pugi::xml_node;
public:
    Combinations() = default;

    bool load(const std::filesystem::path & resource);
    std::string classify(const std::vector<Component> & components, std::vector<int> & order) const;

private:
    // implementation details
    std::vector<Combination> combination_types;

    void compressCombination( const std::vector<Component> & components,
            std::vector<Component> & compressedCombination ) const;

    bool checkFullCombination( std::vector<Component> & components, const Combination & combination ) const;

    std::function<bool(const Component & , const Component &)> component_id_comparator =
            [] (const Component & a, const Component & b) { return a.personal_id < b.personal_id; };

    bool getNextPermutation( std::vector<Component> & sequence ) const;
    void resetPermutations( std::vector<Component> & sequence ) const;
};


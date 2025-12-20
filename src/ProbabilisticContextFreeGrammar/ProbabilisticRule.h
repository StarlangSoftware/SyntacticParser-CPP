//
// Created by Olcay Taner YILDIZ on 17.02.2024.
//

#ifndef SYNTACTICPARSER_PROBABILISTICRULE_H
#define SYNTACTICPARSER_PROBABILISTICRULE_H


#include "../ContextFreeGrammar/Rule.h"

class ProbabilisticRule : public Rule{
private:
    double probability = 0.0;
    int count = 0;
public:
    ProbabilisticRule(const Symbol& leftHandSide, const vector<Symbol>& rightHandSide);
    ProbabilisticRule(const Symbol& leftHandSide, const vector<Symbol>& rightHandSide, RuleType type, double probability);
    explicit ProbabilisticRule(const string& rule);
    [[nodiscard]] double getProbability() const;
    void increment();
    void normalizeProbability(int total);
    [[nodiscard]] int getCount() const;
    [[nodiscard]] string to_string() const;
};


#endif //SYNTACTICPARSER_PROBABILISTICRULE_H

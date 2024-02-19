//
// Created by Olcay Taner YILDIZ on 17.02.2024.
//

#include "ProbabilisticRule.h"

ProbabilisticRule::ProbabilisticRule(const Symbol &leftHandSide, const vector<Symbol> &rightHandSide) : Rule(leftHandSide, rightHandSide) {
}

ProbabilisticRule::ProbabilisticRule(const Symbol &leftHandSide, const vector<Symbol> &rightHandSide, RuleType type,
                                     double probability): Rule(leftHandSide, rightHandSide, type) {
    this->probability = probability;
}

ProbabilisticRule::ProbabilisticRule(const string& rule){
    string prob = rule.substr(rule.find('[') + 1, rule.find(']') - rule.find('[') - 1);
    string left = Word::trim(rule.substr(0, rule.find("->")));
    string right = Word::trim(rule.substr(rule.find("->") + 2, rule.find('[') - rule.find("->") - 2));
    leftHandSide = Symbol(left);
    vector<string> rightSide = Word::split(right, " ");
    for (const string& s : rightSide){
        rightHandSide.emplace_back(s);
    }
    probability = stod(prob);
}

double ProbabilisticRule::getProbability() const{
    return probability;
}

void ProbabilisticRule::increment(){
    count++;
}

void ProbabilisticRule::normalizeProbability(int total) {
    probability = count / (total + 0.0);
}

int ProbabilisticRule::getCount() const{
    return count;
}

string ProbabilisticRule::to_string() const{
    return Rule::to_string() + "[" + ::to_string(probability) + "]";
}
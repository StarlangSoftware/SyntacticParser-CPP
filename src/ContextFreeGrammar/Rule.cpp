//
// Created by Olcay Taner YILDIZ on 10.02.2024.
//

#include "Rule.h"

Rule::Rule() : Rule(Symbol(""), Symbol("")){
}

Rule::Rule(const Symbol& leftHandSide, const Symbol& rightHandSideSymbol) {
    this->leftHandSide = leftHandSide;
    this->rightHandSide.emplace_back(rightHandSideSymbol);
}

Rule::Rule(const Symbol& leftHandSide, const Symbol& rightHandSideSymbol1, const Symbol& rightHandSideSymbol2) : Rule(leftHandSide, rightHandSideSymbol1){
    this->rightHandSide.emplace_back(rightHandSideSymbol2);
}

Rule::Rule(const Symbol& leftHandSide, const vector<Symbol>& rightHandSide) {
    this->leftHandSide = leftHandSide;
    this->rightHandSide = rightHandSide;
}

Rule::Rule(const Symbol& leftHandSide, const vector<Symbol> &rightHandSide, RuleType type) : Rule(leftHandSide, rightHandSide){
    this->type = type;
}

Rule::Rule(const string &ruleString) {
    string left = ruleString.substr(0, ruleString.find("->"));
    string right = ruleString.substr(ruleString.find("->") + 2);
    leftHandSide = Symbol(left);
    vector<string> rightSide = Word::split(right, " ");
    for (const string& s : rightSide){
        rightHandSide.emplace_back(s);
    }
}

bool Rule::leftRecursive() const{
    return rightHandSide[0] == leftHandSide && type == RuleType::SINGLE_NON_TERMINAL;
}

RuleType Rule::getRuleType() const {
    return type;
}

Symbol Rule::getLeftHandSide() const {
    return leftHandSide;
}

vector<Symbol> Rule::getRightHandSide() const{
    return rightHandSide;
}

int Rule::getRightHandSideSize() const{
    return rightHandSide.size();
}

Symbol Rule::getRightHandSideAt(int index) const{
    return rightHandSide[index];
}

string Rule::to_string() const{
    string result = leftHandSide.getName() + " -> ";
    for (const Symbol& symbol: rightHandSide){
        result += " " + symbol.getName();
    }
    return result;
}

void Rule::setType(RuleType ruleType){
    this->type = ruleType;
}

bool Rule::updateMultipleNonTerminal(const Symbol& first, const Symbol& second, const Symbol& with){
    int i;
    for (i = 0; i < rightHandSide.size() - 1; i++){
        if (rightHandSide[i] == first && rightHandSide[i + 1] == second){
            rightHandSide.erase(rightHandSide.begin() + i + 1);
            rightHandSide.erase(rightHandSide.begin() + i);
            rightHandSide.insert(rightHandSide.begin() + i, with);
            if (rightHandSide.size() == 2){
                type = RuleType::TWO_NON_TERMINAL;
            }
            return true;
        }
    }
    return false;
}
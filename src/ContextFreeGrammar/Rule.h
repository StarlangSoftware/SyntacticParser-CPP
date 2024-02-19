//
// Created by Olcay Taner YILDIZ on 10.02.2024.
//

#ifndef SYNTACTICPARSER_RULE_H
#define SYNTACTICPARSER_RULE_H

#include <Symbol.h>
#include "RuleType.h"

using namespace std;

class Rule{
protected:
    Symbol leftHandSide;
    vector<Symbol> rightHandSide;
    RuleType type;
public:
    Rule();
    Rule(const Symbol& leftHandSide, const Symbol& rightHandSideSymbol);
    Rule(const Symbol& leftHandSide, const Symbol& rightHandSideSymbol1, const Symbol& rightHandSideSymbol2);
    Rule(const Symbol& leftHandSide, const vector<Symbol>& rightHandSide);
    Rule(const Symbol& leftHandSide, const vector<Symbol>& rightHandSide, RuleType type);
    explicit Rule(const string& ruleString);
    bool leftRecursive() const;
    RuleType getRuleType() const;
    Symbol getLeftHandSide() const;
    vector<Symbol> getRightHandSide() const;
    int getRightHandSideSize() const;
    Symbol getRightHandSideAt(int index) const;
    string to_string() const;
    void setType(RuleType ruleType);
    bool updateMultipleNonTerminal(const Symbol& first, const Symbol& second, const Symbol& with);
    bool operator==(const Rule &anotherRule) const{
        if (leftHandSide.getName() == anotherRule.leftHandSide.getName()){
            if (rightHandSide.size() == anotherRule.rightHandSide.size()){
                for (int i = 0; i < rightHandSide.size(); i++){
                    if (rightHandSide[i].getName() != anotherRule.rightHandSide[i].getName()){
                        return false;
                    }
                }
            } else {
                return false;
            }
        } else {
            return false;
        }
        return true;
    }
};
#endif //SYNTACTICPARSER_RULE_H

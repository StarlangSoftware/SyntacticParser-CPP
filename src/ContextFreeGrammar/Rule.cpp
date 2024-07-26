//
// Created by Olcay Taner YILDIZ on 10.02.2024.
//

#include "Rule.h"

/**
 * Empty constructor for the rule class.
 */
Rule::Rule() : Rule(Symbol(""), Symbol("")){
}

/**
 * Constructor for the rule X -> Y.
 * @param leftHandSide Non-terminal symbol X
 * @param rightHandSideSymbol Symbol Y (terminal or non-terminal)
 */
Rule::Rule(const Symbol& leftHandSide, const Symbol& rightHandSideSymbol) {
    this->leftHandSide = leftHandSide;
    this->rightHandSide.emplace_back(rightHandSideSymbol);
}

/**
 * Constructor for the rule X -> YZ.
 * @param leftHandSide Non-terminal symbol X.
 * @param rightHandSideSymbol1 Symbol Y (non-terminal).
 * @param rightHandSideSymbol2 Symbol Z (non-terminal).
 */
Rule::Rule(const Symbol& leftHandSide, const Symbol& rightHandSideSymbol1, const Symbol& rightHandSideSymbol2) : Rule(leftHandSide, rightHandSideSymbol1){
    this->rightHandSide.emplace_back(rightHandSideSymbol2);
}

/**
 * Constructor for the rule X -> beta. beta is a string of symbols from symbols (non-terminal)
 * @param leftHandSide Non-terminal symbol X.
 * @param rightHandSide beta. beta is a string of symbols from symbols (non-terminal)
 */
Rule::Rule(const Symbol& leftHandSide, const vector<Symbol>& rightHandSide) {
    this->leftHandSide = leftHandSide;
    this->rightHandSide = rightHandSide;
}

/**
 * Constructor for the rule X -> beta. beta is a string of symbols from symbols (non-terminal)
 * @param leftHandSide Non-terminal symbol X.
 * @param rightHandSide beta. beta is a string of symbols from symbols (non-terminal)
 * @param type Type of the rule. TERMINAL if the rule is like X -> a, SINGLE_NON_TERMINAL if the rule is like X -> Y,
 *             TWO_NON_TERMINAL if the rule is like X -> YZ, MULTIPLE_NON_TERMINAL if the rule is like X -> YZT..
 */
Rule::Rule(const Symbol& leftHandSide, const vector<Symbol> &rightHandSide, RuleType type) : Rule(leftHandSide, rightHandSide){
    this->type = type;
}

/**
 * Constructor for any rule from a string. The string is of the form X -> .... The method constructs left hand
 * side symbol and right hand side symbol(s) from the input string.
 * @param rule String containing the rule. The string is of the form X -> ....
 */
Rule::Rule(const string &ruleString) {
    string left = ruleString.substr(0, ruleString.find("->"));
    string right = ruleString.substr(ruleString.find("->") + 2);
    leftHandSide = Symbol(left);
    vector<string> rightSide = Word::split(right, " ");
    for (const string& s : rightSide){
        rightHandSide.emplace_back(s);
    }
}

/**
 * Checks if the rule is left recursive or not. A rule is left recursive if it is of the form X -> X..., so its
 * first symbol of the right side is the symbol on the left side.
 * @return True, if the rule is left recursive; false otherwise.
 */
bool Rule::leftRecursive() const{
    return rightHandSide[0] == leftHandSide && type == RuleType::SINGLE_NON_TERMINAL;
}

/**
 * Accessor for the rule type.
 * @return Rule type.
 */
RuleType Rule::getRuleType() const {
    return type;
}

/**
 * Accessor for the left hand side.
 * @return Left hand side.
 */
Symbol Rule::getLeftHandSide() const {
    return leftHandSide;
}

/**
 * Accessor for the right hand side.
 * @return Right hand side.
 */
vector<Symbol> Rule::getRightHandSide() const{
    return rightHandSide;
}

/**
 * Returns number of symbols on the right hand side.
 * @return Number of symbols on the right hand side.
 */
int Rule::getRightHandSideSize() const{
    return rightHandSide.size();
}

/**
 * Returns symbol at position index on the right hand side.
 * @param index Position of the symbol
 * @return Symbol at position index on the right hand side.
 */
Symbol Rule::getRightHandSideAt(int index) const{
    return rightHandSide[index];
}

/**
 * Converts the rule to the form X -> ...
 * @return String form of the rule in the form of X -> ...
 */
string Rule::to_string() const{
    string result = leftHandSide.getName() + " -> ";
    for (const Symbol& symbol: rightHandSide){
        result += " " + symbol.getName();
    }
    return result;
}

/**
 * Mutator for the rule type field.
 * @param ruleType New rule type.
 */
void Rule::setType(RuleType ruleType){
    this->type = ruleType;
}

/**
 * In conversion to Chomsky Normal Form, rules like A -> BC... are replaced with A -> X1... and X1 -> BC. This
 * method replaces B and C non-terminals on the right hand side with X1.
 * @param first Non-terminal symbol B.
 * @param second Non-terminal symbol C.
 * @param with Non-terminal symbol X1.
 * @return True, if any replacements has been made; false otherwise.
 */
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
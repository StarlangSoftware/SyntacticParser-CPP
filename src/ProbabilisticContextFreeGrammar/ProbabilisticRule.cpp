//
// Created by Olcay Taner YILDIZ on 17.02.2024.
//

#include "ProbabilisticRule.h"

/**
 * Constructor for the rule X -> beta. beta is a string of symbols from symbols (non-terminal)
 * @param leftHandSide Non-terminal symbol X.
 * @param rightHandSide beta. beta is a string of symbols from symbols (non-terminal)
 */
ProbabilisticRule::ProbabilisticRule(const Symbol &leftHandSide, const vector<Symbol> &rightHandSide) : Rule(leftHandSide, rightHandSide) {
}

/**
 * Constructor for the probabilistic rule X -> beta. beta is a string of symbols from symbols (non-terminal)
 * @param leftHandSide Non-terminal symbol X.
 * @param rightHandSide beta. beta is a string of symbols from symbols (non-terminal)
 * @param type Type of the rule. TERMINAL if the rule is like X -> a, SINGLE_NON_TERMINAL if the rule is like X -> Y,
 *             TWO_NON_TERMINAL if the rule is like X -> YZ, MULTIPLE_NON_TERMINAL if the rule is like X -> YZT..
 * @param probability Probability of the rule
 */
ProbabilisticRule::ProbabilisticRule(const Symbol &leftHandSide, const vector<Symbol> &rightHandSide, RuleType type,
                                     double probability): Rule(leftHandSide, rightHandSide, type) {
    this->probability = probability;
}

/**
 * Constructor for any probabilistic rule from a string. The string is of the form X -> .... [probability] The
 * method constructs left hand side symbol and right hand side symbol(s) from the input string.
 * @param rule String containing the rule. The string is of the form X -> .... [probability]
 */
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

/**
 * Accessor for the probability attribute.
 * @return Probability attribute.
 */
double ProbabilisticRule::getProbability() const{
    return probability;
}

/**
 * Increments the count attribute.
 */
void ProbabilisticRule::increment(){
    count++;
}

/**
 * Calculates the probability from count and the given total value.
 * @param total Value used for calculating the probability.
 */
void ProbabilisticRule::normalizeProbability(int total) {
    probability = count / (total + 0.0);
}

/**
 * Accessor for the count attribute
 * @return Count attribute
 */
int ProbabilisticRule::getCount() const{
    return count;
}

/**
 * Converts the rule to the form X -> ... [probability]
 * @return String form of the rule in the form of X -> ... [probability]
 */
string ProbabilisticRule::to_string() const{
    return Rule::to_string() + "[" + ::to_string(probability) + "]";
}
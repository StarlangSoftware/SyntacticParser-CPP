//
// Created by Olcay Taner YILDIZ on 18.02.2024.
//

#include "ProbabilisticContextFreeGrammar.h"

#include <algorithm>
#include <cmath>

#include "ProbabilisticRule.h"

/**
 * Constructor for the ProbabilisticContextFreeGrammar class. Reads the rules from the rule file, lexicon rules from
 * the dictionary file and sets the minimum frequency parameter.
 * @param ruleFileName File name for the rule file.
 * @param dictionaryFileName File name for the lexicon file.
 * @param minCount Minimum frequency parameter.
 */
ProbabilisticContextFreeGrammar::ProbabilisticContextFreeGrammar(const string& ruleFileName,
                                                                 const string& dictionaryFileName,
                                                                 int minCount){
    string line;
    ifstream inputFile;
    inputFile.open(ruleFileName, ifstream :: in);
    while (inputFile.good()) {
        getline(inputFile, line);
        Rule* newRule = new ProbabilisticRule(line);
        rules.emplace_back(newRule);
        rulesRightSorted.emplace_back(newRule);
    }
    inputFile.close();
    sort(rules.begin(), rules.end(), compareRuleLeft);
    sort(rulesRightSorted.begin(), rulesRightSorted.end(), compareRuleRight);
    readDictionary(dictionaryFileName);
    updateTypes();
    this->minCount = minCount;
}

/**
 * Another constructor for the ProbabilisticContextFreeGrammar class. Constructs the lexicon from the leaf nodes of
 * the trees in the given treebank. Extracts rules from the non-leaf nodes of the trees in the given treebank. Also
 * sets the minimum frequency parameter.
 * @param treeBank Treebank containing the constituency trees.
 * @param minCount Minimum frequency parameter.
 */
ProbabilisticContextFreeGrammar::ProbabilisticContextFreeGrammar(const TreeBank& treeBank, int minCount){
    vector<Symbol> variables;
    vector<Rule*> candidates;
    int total;
    constructDictionary(treeBank);
    for (int i = 0; i < treeBank.size(); i++){
        ParseTree* parseTree = treeBank.get(i);
        updateTree(parseTree, minCount);
        addRules(parseTree->getRoot());
    }
    variables = getLeftSide();
    for (const Symbol& variable: variables){
        candidates = getRulesWithLeftSideX(variable);
        total = 0;
        for (Rule* candidate: candidates){
            total += ((ProbabilisticRule*)candidate)->getCount();
        }
        for (Rule* candidate: candidates){
            ((ProbabilisticRule*) candidate)->normalizeProbability(total);
        }
    }
    updateTypes();
    this->minCount = minCount;
}

/**
 * Converts a parse node in a tree to a rule. The symbol in the parse node will be the symbol on the leaf side of the
 * rule, the symbols in the child nodes will be the symbols on the right hand side of the rule.
 * @param parseNode Parse node for which a rule will be created.
 * @param trim If true, the tags will be trimmed. If the symbol's data contains '-' or '=', this method trims all
 *             characters after those characters.
 * @return A new rule constructed from a parse node and its children.
 */
ProbabilisticRule* ProbabilisticContextFreeGrammar::toRule(ParseNode* parseNode, bool trim){
    Symbol left;
    vector<Symbol> right;
    if (trim)
        left = parseNode->getData().trimSymbol();
    else
        left = parseNode->getData();
    for (int i = 0; i < parseNode->numberOfChildren(); i++) {
        ParseNode* childNode = parseNode->getChild(i);
        if (childNode->getData().isTerminal()){
            right.emplace_back(childNode->getData());
        } else {
            right.emplace_back(childNode->getData().trimSymbol());
        }
    }
    return new ProbabilisticRule(left, right);
}

/**
 * Recursive method to generate all rules from a subtree rooted at the given node.
 * @param parseNode Root node of the subtree.
 */
void ProbabilisticContextFreeGrammar::addRules(ParseNode* parseNode){
    Rule* existedRule;
    ProbabilisticRule* newRule;
    newRule = toRule(parseNode, true);
    existedRule = searchRule(newRule);
    if (existedRule == nullptr){
        addRule(newRule);
        newRule->increment();
    } else {
        ((ProbabilisticRule*) existedRule)->increment();
    }
    for (int i = 0; i < parseNode->numberOfChildren(); i++) {
        ParseNode* childNode = parseNode->getChild(i);
        if (childNode->numberOfChildren() > 0){
            addRules(childNode);
        }
    }
}

/**
 * Calculates the probability of a parse node.
 * @param parseNode Parse node for which probability is calculated.
 * @return Probability of a parse node.
 */
double ProbabilisticContextFreeGrammar::probability(ParseNode* parseNode){
    Rule* existedRule;
    ProbabilisticRule* rule;
    double sum = 0.0;
    if (parseNode->numberOfChildren() > 0){
        rule = toRule(parseNode, true);
        existedRule = searchRule(rule);
        sum = log(((ProbabilisticRule*)existedRule)->getProbability());
        if (existedRule->getRuleType() != RuleType::TERMINAL){
            for (int i = 0; i < parseNode->numberOfChildren(); i++){
                ParseNode* childNode = parseNode->getChild(i);
                sum += probability(childNode);
            }
        }
    }
    return sum;
}

/**
 * Calculates the probability of a parse tree.
 * @param parseTree Parse tree for which probability is calculated.
 * @return Probability of the parse tree.
 */
double ProbabilisticContextFreeGrammar::probability(ParseTree* parseTree){
    return probability(parseTree->getRoot());
}

/**
 * In conversion to Chomsky Normal Form, rules like X -> Y are removed and new rules for every rule as Y -> beta are
 * replaced with X -> beta. The method first identifies all X -> Y rules. For every such rule, all rules Y -> beta
 * are identified. For every such rule, the method adds a new rule X -> beta. Every Y -> beta rule is then deleted.
 * The method also calculates the probability of the new rules based on the previous rules.
 */
void ProbabilisticContextFreeGrammar::removeSingleNonTerminalFromRightHandSide(){
    vector<Symbol> nonTerminalList;
    Symbol removeCandidate;
    vector<Rule*> ruleList;
    vector<Rule*> candidateList;
    removeCandidate = getSingleNonTerminalCandidateToRemove(nonTerminalList);
    while (!removeCandidate.getName().empty()){
        ruleList = getRulesWithRightSideX(removeCandidate);
        for (Rule* rule: ruleList){
            candidateList = getRulesWithLeftSideX(removeCandidate);
            for (Rule* candidate: candidateList){
                vector<Symbol> clone;
                for (const Symbol& symbol: candidate->getRightHandSide()){
                    clone.emplace_back(symbol.getName());
                }
                addRule(new ProbabilisticRule(rule->getLeftHandSide(),
                                              clone,
                                              candidate->getRuleType(),
                                              ((ProbabilisticRule*) rule)->getProbability() * ((ProbabilisticRule*) candidate)->getProbability()));
            }
            removeRule(rule);
        }
        nonTerminalList.emplace_back(removeCandidate);
        removeCandidate = getSingleNonTerminalCandidateToRemove(nonTerminalList);
    }
}

/**
 * In conversion to Chomsky Normal Form, rules like A -> BC... are replaced with A -> X1... and X1 -> BC. This
 * method determines such rules and for every such rule, it adds new rule X1->BC and updates rule A->BC to A->X1.
 * The method sets the probability of the rules X1->BC to 1, and calculates the probability of the rules A -> X1...
 */
void ProbabilisticContextFreeGrammar::updateMultipleNonTerminalFromRightHandSide(){
    Rule* updateCandidate;
    int newVariableCount = 0;
    updateCandidate = getMultipleNonTerminalCandidateToUpdate();
    while (updateCandidate != nullptr){
        vector<Symbol> newRightHandSide;
        Symbol newSymbol = Symbol("X" + to_string(newVariableCount));
        newRightHandSide.emplace_back(updateCandidate->getRightHandSide()[0]);
        newRightHandSide.emplace_back(updateCandidate->getRightHandSide()[1]);
        updateAllMultipleNonTerminalWithNewRule(updateCandidate->getRightHandSide()[0], updateCandidate->getRightHandSide()[1], newSymbol);
        addRule(new ProbabilisticRule(newSymbol, newRightHandSide, RuleType::TWO_NON_TERMINAL, 1.0));
        updateCandidate = getMultipleNonTerminalCandidateToUpdate();
        newVariableCount++;
    }
}

/**
 * The method converts the grammar into Chomsky normal form. First, rules like X -> Y are removed and new rules for
 * every rule as Y -> beta are replaced with X -> beta. Second, rules like A -> BC... are replaced with A -> X1...
 * and X1 -> BC.
 */
void ProbabilisticContextFreeGrammar::convertToChomskyNormalForm(){
    removeSingleNonTerminalFromRightHandSide();
    updateMultipleNonTerminalFromRightHandSide();
    sort(rules.begin(), rules.end(), compareRuleLeft);
    sort(rulesRightSorted.begin(), rulesRightSorted.end(), compareRuleRight);
}
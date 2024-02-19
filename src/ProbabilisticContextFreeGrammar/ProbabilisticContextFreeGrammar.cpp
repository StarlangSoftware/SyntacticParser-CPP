//
// Created by Olcay Taner YILDIZ on 18.02.2024.
//

#include "ProbabilisticContextFreeGrammar.h"
#include "ProbabilisticRule.h"

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

double ProbabilisticContextFreeGrammar::probability(ParseTree* parseTree){
    return probability(parseTree->getRoot());
}

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

void ProbabilisticContextFreeGrammar::convertToChomskyNormalForm(){
    removeSingleNonTerminalFromRightHandSide();
    updateMultipleNonTerminalFromRightHandSide();
    sort(rules.begin(), rules.end(), compareRuleLeft);
    sort(rulesRightSorted.begin(), rulesRightSorted.end(), compareRuleRight);
}
//
// Created by Olcay Taner YILDIZ on 14.02.2024.
//

#include <regex>
#include "ContextFreeGrammar.h"
#include "NodeCollector.h"
#include "NodeCondition/IsLeaf.h"
#include <unordered_set>

using namespace std;

void ContextFreeGrammar::readDictionary(const string& dictionaryFileName) {
    string line;
    ifstream inputFile;
    inputFile.open(dictionaryFileName, ifstream :: in);
    while (inputFile.good()) {
        getline(inputFile, line);
        vector<string> items = Word::split(line);
        dictionary.putNTimes(items[0], stoi(items[1]));
    }
    inputFile.close();
}

bool ContextFreeGrammar::compareRuleLeft(Rule* ruleA, Rule* ruleB){
    return ruleA->getLeftHandSide().getName() <= ruleB->getLeftHandSide().getName();
}

int ContextFreeGrammar::compareRuleLeft3Way(Rule* ruleA, Rule* ruleB) {
    if (ruleA->getLeftHandSide() < ruleB->getLeftHandSide()){
        return -1;
    } else {
        if (ruleA->getLeftHandSide() > ruleB->getLeftHandSide()){
            return 1;
        } else {
            return 0;
        }
    }
}

bool ContextFreeGrammar::compareRuleRight(Rule* ruleA, Rule* ruleB) {
    int i = 0;
    while (i < ruleA->getRightHandSideSize() && i < ruleB->getRightHandSideSize()){
        if (ruleA->getRightHandSideAt(i) == ruleB->getRightHandSideAt(i)){
            i++;
        } else {
            return ruleA->getRightHandSideAt(i) == ruleB->getRightHandSideAt(i);
        }
    }
    if (ruleA->getRightHandSideSize() <= ruleB->getRightHandSideSize()){
        return true;
    } else {
        return false;
    }
}

int ContextFreeGrammar::compareRuleRight3Way(Rule* ruleA, Rule* ruleB) {
    int i = 0;
    while (i < ruleA->getRightHandSideSize() && i < ruleB->getRightHandSideSize()){
        if (ruleA->getRightHandSideAt(i) == ruleB->getRightHandSideAt(i)){
            i++;
        } else {
            if (ruleA->getRightHandSideAt(i) < ruleB->getRightHandSideAt(i)){
                return -1;
            } else {
                if (ruleA->getRightHandSideAt(i) > ruleB->getRightHandSideAt(i)){
                    return 1;
                } else {
                    return 0;
                }
            }
        }
    }
    if (ruleA->getRightHandSideSize() < ruleB->getRightHandSideSize()){
        return -1;
    } else {
        if (ruleA->getRightHandSideSize() > ruleB->getRightHandSideSize()){
            return 1;
        } else {
            return 0;
        }
    }
}

ContextFreeGrammar::ContextFreeGrammar(const string &ruleFileName,
                                       const string &dictionaryFileName,
                                       int minCount) {
    string line;
    ifstream inputFile;
    inputFile.open(ruleFileName, ifstream :: in);
    while (inputFile.good()) {
        getline(inputFile, line);
        Rule* newRule = new Rule(line);
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

ContextFreeGrammar::ContextFreeGrammar(const TreeBank& treeBank, int minCount){
    constructDictionary(treeBank);
    for (int i = 0; i < treeBank.size(); i++){
        ParseTree* parseTree = treeBank.get(i);
        updateTree(parseTree, minCount);
        addRules(parseTree->getRoot());
    }
    updateTypes();
    this->minCount = minCount;
}

void ContextFreeGrammar::updateTypes(){
    unordered_set<string> nonTerminals;
    for (Rule* rule : rules){
        nonTerminals.emplace(rule->getLeftHandSide().getName());
    }
    for (Rule* rule : rules){
        if (rule->getRightHandSideSize() > 2){
            rule->setType(RuleType::MULTIPLE_NON_TERMINAL);
        } else {
            if (rule->getRightHandSideSize() == 2){
                rule->setType(RuleType::TWO_NON_TERMINAL);
            } else {
                if (rule->getRightHandSideAt(0).isTerminal() || Word::isPunctuation(rule->getRightHandSideAt(0).getName()) || !nonTerminals.contains(rule->getRightHandSideAt(0).getName())){
                    rule->setType(RuleType::TERMINAL);
                } else {
                    rule->setType(RuleType::SINGLE_NON_TERMINAL);
                }
            }
        }
    }
}

void ContextFreeGrammar::constructDictionary(const TreeBank& treeBank){
    for (int i = 0; i < treeBank.size(); i++){
        ParseTree* parseTree = treeBank.get(i);
        NodeCollector nodeCollector = NodeCollector(parseTree->getRoot(), new IsLeaf());
        vector<ParseNode*> leafList = nodeCollector.collect();
        for (const ParseNode* parseNode : leafList){
            dictionary.put(parseNode->getData().getName());
        }
    }
}

void ContextFreeGrammar::updateTree(ParseTree* parseTree, int _minCount){
    NodeCollector nodeCollector = NodeCollector(parseTree->getRoot(), new IsLeaf());
    vector<ParseNode*> leafList = nodeCollector.collect();
    regex pattern1 = regex("\\+?\\d+");
    regex pattern2 = regex("\\+?(\\d+)?\\.\\d*");
    for (ParseNode* parseNode : leafList){
        string data = parseNode->getData().getName();
        if (regex_match(data, pattern1) || (regex_match(data, pattern2) && data != ".")){
            parseNode->setData(Symbol("_num_"));
        } else {
            if (dictionary.count(data) < _minCount){
                parseNode->setData(Symbol("_rare_"));
            }
        }
    }
}

void ContextFreeGrammar::removeExceptionalWordsFromSentence(Sentence* sentence) const{
    regex pattern1 = regex("\\+?\\d+");
    regex pattern2 = regex("\\+?(\\d+)?\\.\\d*");
    for (int i = 0; i < sentence->wordCount(); i++){
        Word* word = sentence->getWord(i);
        if (regex_match(word->getName(), pattern1) || (regex_match(word->getName(), pattern2) && word->getName() != ".")){
            word->setName("_num_");
        } else {
            if (dictionary.count(word->getName()) < minCount){
                word->setName("_rare_");
            }
        }
    }
}

void ContextFreeGrammar::reinsertExceptionalWordsFromSentence(ParseTree* parseTree, Sentence* sentence) const{
    NodeCollector nodeCollector = NodeCollector(parseTree->getRoot(), new IsLeaf());
    vector<ParseNode*> leafList = nodeCollector.collect();
    for (int i = 0; i < leafList.size(); i++){
        ParseNode* parseNode = leafList[i];
        string treeWord = parseNode->getData().getName();
        string sentenceWord = sentence->getWord(i)->getName();
        if (treeWord == "_rare_" || treeWord == "_num_"){
            parseNode->setData(Symbol(sentenceWord));
        }
    }
}

Rule* ContextFreeGrammar::toRule(ParseNode* parseNode, bool trim){
    Symbol left;
    vector<Symbol> right;
    if (trim)
        left = parseNode->getData().trimSymbol();
    else
        left = parseNode->getData();
    for (int i = 0; i < parseNode->numberOfChildren(); i++) {
        ParseNode* childNode = parseNode->getChild(i);
        if (childNode->getData().isTerminal() || !trim){
            right.emplace_back(childNode->getData());
        } else {
            right.emplace_back(childNode->getData().trimSymbol());
        }
    }
    return new Rule(left, right);
}

void ContextFreeGrammar::addRules(ParseNode *parseNode) {
    Rule* newRule;
    newRule = toRule(parseNode, true);
    addRule(newRule);
    for (int i = 0; i < parseNode->numberOfChildren(); i++) {
        ParseNode* childNode = parseNode->getChild(i);
        if (childNode->numberOfChildren() > 0){
            addRules(childNode);
        }
    }
}

int ContextFreeGrammar::binarySearch(vector<Rule*> ruleList, Rule* rule, int compareRule(Rule* ruleA, Rule* ruleB)) const {
    int lo = 0;
    int hi = ruleList.size() - 1;
    while (lo <= hi){
        int mid = (lo + hi) / 2;
        if (compareRule(ruleList[mid], rule) == 0){
            return mid;
        }
        if (compareRule(ruleList[mid], rule) < 0) {
            lo = mid + 1;
        } else {
            hi = mid - 1;
        }
    }
    return -(lo + 1);
}

void ContextFreeGrammar::addRule(Rule* newRule){
    int pos;
    pos = binarySearch(rules, newRule, compareRuleLeft3Way);
    if (pos < 0){
        rules.insert(rules.begin() - pos - 1, newRule);
        pos = binarySearch(rulesRightSorted, newRule, compareRuleRight3Way);
        if (pos >= 0){
            rulesRightSorted.insert(rulesRightSorted.begin() + pos, newRule);
        } else {
            rulesRightSorted.insert(rulesRightSorted.begin() - pos - 1, newRule);
        }
    }
}

void ContextFreeGrammar::removeRule(Rule* rule){
    int pos, posUp, posDown;
    pos = binarySearch(rules, rule, compareRuleLeft3Way);
    if (pos >= 0){
        rules.erase(rules.begin() + pos);
        pos = binarySearch(rulesRightSorted, rule, compareRuleRight3Way);
        posUp = pos;
        while (posUp >= 0 && compareRuleLeft3Way(rulesRightSorted[posUp], rule) == 0){
            if (rule == rulesRightSorted[posUp]){
                rulesRightSorted.erase(rulesRightSorted.begin() + posUp);
                return;
            }
            posUp--;
        }
        posDown = pos + 1;
        while (posDown < rulesRightSorted.size() && compareRuleRight3Way(rulesRightSorted[posDown], rule) == 0){
            if (rule == rulesRightSorted[posDown]){
                rulesRightSorted.erase(rulesRightSorted.begin() + posDown);
                return;
            }
            posDown++;
        }
    }
}

/*Return rules such as X -> ... */
vector<Rule*> ContextFreeGrammar::getRulesWithLeftSideX(const Symbol& X){
    int middle, middleUp, middleDown;
    vector<Rule*> result;
    Rule* dummyRule = new Rule(X, X);
    middle = binarySearch(rules, dummyRule, compareRuleLeft3Way);
    if (middle >= 0){
        middleUp = middle;
        while (middleUp >= 0 && rules[middleUp]->getLeftHandSide() == X){
            result.emplace_back(rules[middleUp]);
            middleUp--;
        }
        middleDown = middle + 1;
        while (middleDown < rules.size() && rules[middleDown]->getLeftHandSide() == X){
            result.emplace_back(rules[middleDown]);
            middleDown++;
        }
    }
    return result;
}

/*Return symbols X from terminal rules such as X -> a */
vector<Symbol> ContextFreeGrammar::partOfSpeechTags() const{
    vector<Symbol> result;
    for (Rule* rule : rules) {
        if (rule->getRuleType() == RuleType::TERMINAL && find(result.begin(), result.end(), rule->getLeftHandSide()) == result.end()) {
            result.emplace_back(rule->getLeftHandSide());
        }
    }
    return result;
}

/*Return symbols X from all rules such as X -> ... */
vector<Symbol> ContextFreeGrammar::getLeftSide() const{
    vector<Symbol> result;
    for (Rule* rule : rules) {
        if (find(result.begin(), result.end(), rule->getLeftHandSide()) == result.end()) {
            result.emplace_back(rule->getLeftHandSide());
        }
    }
    return result;
}

/*Return terminal rules such as X -> s*/
vector<Rule*> ContextFreeGrammar::getTerminalRulesWithRightSideX(const Symbol& s) const{
    int middle, middleUp, middleDown;
    vector<Rule*> result;
    Rule* dummyRule = new Rule(s, s);
    middle = binarySearch(rulesRightSorted, dummyRule, compareRuleRight3Way);
    if (middle >= 0){
        middleUp = middle;
        while (middleUp >= 0 && rulesRightSorted[middleUp]->getRightHandSideAt(0) == s){
            if (rulesRightSorted[middleUp]->getRuleType() == RuleType::TERMINAL){
                result.emplace_back(rulesRightSorted[middleUp]);
            }
            middleUp--;
        }
        middleDown = middle + 1;
        while (middleDown < rulesRightSorted.size() && rulesRightSorted[middleDown]->getRightHandSideAt(0) == s){
            if (rulesRightSorted[middleDown]->getRuleType() == RuleType::TERMINAL){
                result.emplace_back(rulesRightSorted[middleDown]);
            }
            middleDown++;
        }
    }
    return result;
}

/*Return terminal rules such as X -> S*/
vector<Rule*> ContextFreeGrammar::getRulesWithRightSideX(const Symbol& S) const{
    int pos, posUp, posDown;
    vector<Rule*> result;
    Rule* dummyRule = new Rule(S, S);
    pos = binarySearch(rulesRightSorted, dummyRule, compareRuleRight3Way);
    if (pos >= 0){
        posUp = pos;
        while (posUp >= 0 && rulesRightSorted[posUp]->getRightHandSideAt(0) == S && rulesRightSorted[posUp]->getRightHandSideSize() == 1){
            result.emplace_back(rulesRightSorted[posUp]);
            posUp--;
        }
        posDown = pos + 1;
        while (posDown < rulesRightSorted.size() && rulesRightSorted[posDown]->getRightHandSideAt(0) == S && rulesRightSorted[posDown]->getRightHandSideSize() == 1){
            result.emplace_back(rulesRightSorted[posDown]);
            posDown++;
        }
    }
    return result;
}

/*Return rules such as X -> AB */
vector<Rule*> ContextFreeGrammar::getRulesWithTwoNonTerminalsOnRightSide(const Symbol& A, const Symbol& B) const{
    int pos, posUp, posDown;
    vector<Rule*> result;
    Rule* dummyRule = new Rule(A, A, B);
    pos = binarySearch(rulesRightSorted, dummyRule, compareRuleRight3Way);
    if (pos >= 0){
        posUp = pos;
        while (posUp >= 0 && rulesRightSorted[posUp]->getRightHandSideSize() == 2 && rulesRightSorted[posUp]->getRightHandSideAt(0) == A && rulesRightSorted[posUp]->getRightHandSideAt(1) == B){
            result.emplace_back(rulesRightSorted[posUp]);
            posUp--;
        }
        posDown = pos + 1;
        while (posDown < rulesRightSorted.size() && rulesRightSorted[posDown]->getRightHandSideSize() == 2 && rulesRightSorted[posDown]->getRightHandSideAt(0) == A && rulesRightSorted[posDown]->getRightHandSideAt(1) == B){
            result.emplace_back(rulesRightSorted[posDown]);
            posDown++;
        }

    }
    return result;
}

/*Return Y of the first rule such as X -> Y */
Symbol ContextFreeGrammar::getSingleNonTerminalCandidateToRemove(vector<Symbol> removedList) const{
    Symbol removeCandidate = Symbol("");
    for (Rule* rule:rules) {
        if (rule->getRuleType() == RuleType::SINGLE_NON_TERMINAL && !rule->leftRecursive() && find(removedList.begin(), removedList.end(), rule->getRightHandSideAt(0)) == removedList.end()) {
            removeCandidate = rule->getRightHandSideAt(0);
            break;
        }
    }
    return removeCandidate;
}

/*Return the first rule such as X -> ABC... */
Rule* ContextFreeGrammar::getMultipleNonTerminalCandidateToUpdate() const{
    Rule* removeCandidate = nullptr;
    for (Rule* rule:rules) {
        if (rule->getRuleType() == RuleType::MULTIPLE_NON_TERMINAL) {
            removeCandidate = rule;
            break;
        }
    }
    return removeCandidate;
}

void ContextFreeGrammar::removeSingleNonTerminalFromRightHandSide(){
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
                addRule(new Rule(rule->getLeftHandSide(), clone, candidate->getRuleType()));
            }
            removeRule(rule);
        }
        nonTerminalList.emplace_back(removeCandidate);
        removeCandidate = getSingleNonTerminalCandidateToRemove(nonTerminalList);
    }
}

void ContextFreeGrammar::updateAllMultipleNonTerminalWithNewRule(const Symbol& first, const Symbol& second, const Symbol& with){
    for (Rule* rule : rules) {
        if (rule->getRuleType() == RuleType::MULTIPLE_NON_TERMINAL){
            rule->updateMultipleNonTerminal(first, second, with);
        }
    }
}

void ContextFreeGrammar::updateMultipleNonTerminalFromRightHandSide(){
    Rule* updateCandidate;
    int newVariableCount = 0;
    updateCandidate = getMultipleNonTerminalCandidateToUpdate();
    while (updateCandidate != nullptr){
        vector<Symbol> newRightHandSide;
        Symbol newSymbol = Symbol("X" + to_string(newVariableCount));
        newRightHandSide.emplace_back(updateCandidate->getRightHandSide()[0]);
        newRightHandSide.emplace_back(updateCandidate->getRightHandSide()[1]);
        updateAllMultipleNonTerminalWithNewRule(updateCandidate->getRightHandSide()[0], updateCandidate->getRightHandSide()[1], newSymbol);
        addRule(new Rule(newSymbol, newRightHandSide, RuleType::TWO_NON_TERMINAL));
        updateCandidate = getMultipleNonTerminalCandidateToUpdate();
        newVariableCount++;
    }
}

void ContextFreeGrammar::convertToChomskyNormalForm(){
    removeSingleNonTerminalFromRightHandSide();
    updateMultipleNonTerminalFromRightHandSide();
    sort(rules.begin(), rules.end(), compareRuleLeft);
    sort(rulesRightSorted.begin(), rulesRightSorted.end(), compareRuleRight);
}

Rule* ContextFreeGrammar::searchRule(Rule* rule) const{
    int pos;
    pos = binarySearch(rules, rule, compareRuleLeft3Way);
    if (pos >= 0){
        return rules[pos];
    } else {
        return nullptr;
    }
}

int ContextFreeGrammar::size() const{
    return rules.size();
}
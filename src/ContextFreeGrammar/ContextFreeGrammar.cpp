//
// Created by Olcay Taner YILDIZ on 14.02.2024.
//

#include <regex>
#include "ContextFreeGrammar.h"
#include "NodeCollector.h"
#include "NodeCondition/IsLeaf.h"
#include <unordered_set>

using namespace std;

/**
 * Reads the lexicon for the grammar. Each line consists of two items, the terminal symbol and the frequency of
 * that symbol. The method fills the dictionary counter hash map according to this data.
 * @param dictionaryFileName File name of the lexicon.
 */
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

/**
 * Compares two rules based on their left sides lexicographically.
 * @param ruleA the first rule to be compared.
 * @param ruleB the second rule to be compared.
 * @return -1 if the first rule is less than the second rule lexicographically, 1 if the first rule is larger than
 *          the second rule lexicographically, 0 if they are the same rule.
 */
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

/**
 * Compares two rules based on their right sides lexicographically.
 * @param ruleA the first rule to be compared.
 * @param ruleB the second rule to be compared.
 * @return -1 if the first rule is less than the second rule lexicographically, 1 if the first rule is larger than
 *          the second rule lexicographically, 0 if they are the same rule.
 */
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

/**
 * Constructor for the ContextFreeGrammar class. Reads the rules from the rule file, lexicon rules from the
 * dictionary file and sets the minimum frequency parameter.
 * @param ruleFileName File name for the rule file.
 * @param dictionaryFileName File name for the lexicon file.
 * @param minCount Minimum frequency parameter.
 */
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

/**
 * Another constructor for the ContextFreeGrammar class. Constructs the lexicon from the leaf nodes of the trees
 * in the given treebank. Extracts rules from the non-leaf nodes of the trees in the given treebank. Also sets the
 * minimum frequency parameter.
 * @param treeBank Treebank containing the constituency trees.
 * @param minCount Minimum frequency parameter.
 */
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

/**
 * Updates the types of the rules according to the number of symbols on the right hand side. Rule type is TERMINAL
 * if the rule is like X -> a, SINGLE_NON_TERMINAL if the rule is like X -> Y, TWO_NON_TERMINAL if the rule is like
 * X -> YZ, MULTIPLE_NON_TERMINAL if the rule is like X -> YZT...
 */
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

/**
 * Constructs the lexicon from the given treebank. Reads each tree and for each leaf node in each tree puts the
 * symbol in the dictionary.
 * @param treeBank Treebank containing the constituency trees.
 */
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

/**
 * Updates the exceptional symbols of the leaf nodes in the trees. Constituency trees consists of rare symbols and
 * numbers, which are usually useless in creating constituency grammars. This is due to the fact that, numbers may
 * not occur exactly the same both in the train and/or test set, although they have the same meaning in general.
 * Similarly, when a symbol occurs in the test set but not in the training set, there will not be any rule covering
 * that symbol and therefore no parse tree will be generated. For those reasons, the leaf nodes containing numerals
 * are converted to the same terminal symbol, i.e. _num_; the leaf nodes containing rare symbols are converted to
 * the same terminal symbol, i.e. _rare_.
 * @param parseTree Parse tree to be updated.
 * @param minCount Minimum frequency for the terminal symbols to be considered as rare.
 */
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

/**
 * Updates the exceptional words in the sentences for which constituency parse trees will be generated. Constituency
 * trees consist of rare symbols and numbers, which are usually useless in creating constituency grammars. This is
 * due to the fact that, numbers may not occur exactly the same both in the train and/or test set, although they have
 * the same meaning in general. Similarly, when a symbol occurs in the test set but not in the training set, there
 * will not be any rule covering that symbol and therefore no parse tree will be generated. For those reasons, the
 * words containing numerals are converted to the same terminal symbol, i.e. _num_; thewords containing rare symbols
 * are converted to the same terminal symbol, i.e. _rare_.
 * @param sentence Sentence to be updated.
 */
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

/**
 * After constructing the constituency tree with a parser for a sentence, it contains exceptional words such as
 * rare words and numbers, which are represented as _rare_ and _num_ symbols in the tree. Those words should be
 * converted to their original forms. This method replaces the exceptional symbols to their original forms by
 * replacing _rare_ and _num_ symbols.
 * @param parseTree Parse tree to be updated.
 * @param sentence Original sentence for which constituency tree is generated.
 */
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

/**
 * Converts a parse node in a tree to a rule. The symbol in the parse node will be the symbol on the leaf side of the
 * rule, the symbols in the child nodes will be the symbols on the right hand side of the rule.
 * @param parseNode Parse node for which a rule will be created.
 * @param trim If true, the tags will be trimmed. If the symbol's data contains '-' or '=', this method trims all
 *             characters after those characters.
 * @return A new rule constructed from a parse node and its children.
 */
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

/**
 * Recursive method to generate all rules from a subtree rooted at the given node.
 * @param parseNode Root node of the subtree.
 */
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

/**
 * Searches the given rule from the rule list according to the given rule comparator.
 * @param ruleList List of rules to search
 * @param rule Rule to be searched
 * @param compareRule Comparator function to compare two rules.
 * @return Position of the rule, if the rule exists in the rule list. If the rule does not exist, it returns the
 * position of the rule to be inserted.
 */
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

/**
 * Inserts a new rule into the correct position in the sorted rules and rulesRightSorted array lists.
 * @param newRule Rule to be inserted into the sorted array lists.
 */
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

/**
 * Removes a given rule from the sorted rules and rulesRightSorted array lists.
 * @param rule Rule to be removed from the sorted array lists.
 */
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

/**
 * Returns rules formed as X -> ... Since there can be more than one rule, which have X on the left side, the method
 * first binary searches the rule to obtain the position of such a rule, then goes up and down to obtain others
 * having X on the left side.
 * @param X Left side of the rule
 * @return Rules of the form X -> ...
 */
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

/**
 * Returns all symbols X from terminal rules such as X -> a.
 * @return All symbols X from terminal rules such as X -> a.
 */
vector<Symbol> ContextFreeGrammar::partOfSpeechTags() const{
    vector<Symbol> result;
    for (Rule* rule : rules) {
        if (rule->getRuleType() == RuleType::TERMINAL && find(result.begin(), result.end(), rule->getLeftHandSide()) == result.end()) {
            result.emplace_back(rule->getLeftHandSide());
        }
    }
    return result;
}

/**
 * Returns all symbols X from all rules such as X -> ...
 * @return All symbols X from all rules such as X -> ...
 */
vector<Symbol> ContextFreeGrammar::getLeftSide() const{
    vector<Symbol> result;
    for (Rule* rule : rules) {
        if (find(result.begin(), result.end(), rule->getLeftHandSide()) == result.end()) {
            result.emplace_back(rule->getLeftHandSide());
        }
    }
    return result;
}

/**
 * Returns all rules with the given terminal symbol on the right hand side, that is it returns all terminal rules
 * such as X -> s
 * @param s Terminal symbol on the right hand side.
 * @return All rules with the given terminal symbol on the right hand side
 */
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

/**
 * Returns all rules with the given non-terminal symbol on the right hand side, that is it returns all non-terminal
 * rules such as X -> S
 * @param S Non-terminal symbol on the right hand side.
 * @return All rules with the given non-terminal symbol on the right hand side
 */
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

/**
 * Returns all rules with the given two non-terminal symbols on the right hand side, that is it returns all
 * non-terminal rules such as X -> AB.
 * @param A First non-terminal symbol on the right hand side.
 * @param B Second non-terminal symbol on the right hand side.
 * @return All rules with the given two non-terminal symbols on the right hand side
 */
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

/**
 * Returns the symbol on the right side of the first rule with one non-terminal symbol on the right hand side, that
 * is it returns S of the first rule such as X -> S. S should also not be in the given removed list.
 * @param removedList Discarded list for symbol S.
 * @return The symbol on the right side of the first rule with one non-terminal symbol on the right hand side. The
 * symbol to be returned should also not be in the given discarded list.
 */
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

/**
 * Returns all rules with more than two non-terminal symbols on the right hand side, that is it returns all
 * non-terminal rules such as X -> ABC...
 * @return All rules with more than two non-terminal symbols on the right hand side.
 */
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

/**
 * In conversion to Chomsky Normal Form, rules like X -> Y are removed and new rules for every rule as Y -> beta are
 * replaced with X -> beta. The method first identifies all X -> Y rules. For every such rule, all rules Y -> beta
 * are identified. For every such rule, the method adds a new rule X -> beta. Every Y -> beta rule is then deleted.
 */
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

/**
 * In conversion to Chomsky Normal Form, rules like A -> BC... are replaced with A -> X1... and X1 -> BC. This
 * method replaces B and C non-terminals on the right hand side with X1 for all rules in the grammar.
 * @param first Non-terminal symbol B.
 * @param second Non-terminal symbol C.
 * @param with Non-terminal symbol X1.
 */
void ContextFreeGrammar::updateAllMultipleNonTerminalWithNewRule(const Symbol& first, const Symbol& second, const Symbol& with){
    for (Rule* rule : rules) {
        if (rule->getRuleType() == RuleType::MULTIPLE_NON_TERMINAL){
            rule->updateMultipleNonTerminal(first, second, with);
        }
    }
}

/**
 * In conversion to Chomsky Normal Form, rules like A -> BC... are replaced with A -> X1... and X1 -> BC. This
 * method determines such rules and for every such rule, it adds new rule X1->BC and updates rule A->BC to A->X1.
 */
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

/**
 * The method converts the grammar into Chomsky normal form. First, rules like X -> Y are removed and new rules for
 * every rule as Y -> beta are replaced with X -> beta. Second, rules like A -> BC... are replaced with A -> X1...
 * and X1 -> BC.
 */
void ContextFreeGrammar::convertToChomskyNormalForm(){
    removeSingleNonTerminalFromRightHandSide();
    updateMultipleNonTerminalFromRightHandSide();
    sort(rules.begin(), rules.end(), compareRuleLeft);
    sort(rulesRightSorted.begin(), rulesRightSorted.end(), compareRuleRight);
}

/**
 * Searches a given rule in the grammar.
 * @param rule Rule to be searched.
 * @return Rule if found, null otherwise.
 */
Rule* ContextFreeGrammar::searchRule(Rule* rule) const{
    int pos;
    pos = binarySearch(rules, rule, compareRuleLeft3Way);
    if (pos >= 0){
        return rules[pos];
    } else {
        return nullptr;
    }
}

/**
 * Returns number of rules in the grammar.
 * @return Number of rules in the Context Free Grammar.
 */
int ContextFreeGrammar::size() const{
    return rules.size();
}
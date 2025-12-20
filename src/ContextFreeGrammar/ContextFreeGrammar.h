//
// Created by Olcay Taner YILDIZ on 14.02.2024.
//

#ifndef SYNTACTICPARSER_CONTEXTFREEGRAMMAR_H
#define SYNTACTICPARSER_CONTEXTFREEGRAMMAR_H

#include <string>
#include "CounterHashMap.h"
#include "Rule.h"
#include "TreeBank.h"
#include "Sentence.h"

using namespace std;

class ContextFreeGrammar {
protected:
    CounterHashMap<string> dictionary;
    vector<Rule*> rules;
    vector<Rule*> rulesRightSorted;
    int minCount = 1;
    void readDictionary(const string& dictionaryFileName);
    void updateTypes() const;
    void constructDictionary(const TreeBank& treeBank);
    [[nodiscard]] Symbol getSingleNonTerminalCandidateToRemove(vector<Symbol> removedList) const;
    [[nodiscard]] Rule* getMultipleNonTerminalCandidateToUpdate() const;
    void updateAllMultipleNonTerminalWithNewRule(const Symbol& first, const Symbol& second, const Symbol& with) const;
public:
    ContextFreeGrammar() = default;
    ContextFreeGrammar(const string& ruleFileName,
                       const string& dictionaryFileName,
                       int minCount);
    ContextFreeGrammar(const TreeBank& treeBank, int minCount);
    void updateTree(const ParseTree* parseTree, int _minCount) const;
    void removeExceptionalWordsFromSentence(const Sentence* sentence) const;
    void reinsertExceptionalWordsFromSentence(const ParseTree* parseTree, const Sentence* sentence) const;
    static Rule* toRule(const ParseNode* parseNode, bool trim);
    void addRule(Rule* newRule);
    void removeRule(Rule* rule);
    vector<Rule*> getRulesWithLeftSideX(const Symbol& X);
    [[nodiscard]] vector<Symbol> partOfSpeechTags() const;
    [[nodiscard]] vector<Symbol> getLeftSide() const;
    [[nodiscard]] vector<Rule*> getTerminalRulesWithRightSideX(const Symbol& s) const;
    [[nodiscard]] vector<Rule*> getRulesWithRightSideX(const Symbol& S) const;
    [[nodiscard]] vector<Rule*> getRulesWithTwoNonTerminalsOnRightSide(const Symbol& A, const Symbol& B) const;
    void convertToChomskyNormalForm();
    Rule* searchRule(const Rule* rule) const;
    [[nodiscard]] int size() const;
    static bool compareRuleLeft(const Rule* ruleA, const Rule* ruleB);
    static int compareRuleLeft3Way(const Rule* ruleA, const Rule* ruleB);
    static bool compareRuleRight(const Rule* ruleA, const Rule* ruleB);
    static int compareRuleRight3Way(const Rule* ruleA, const Rule* ruleB);
private:
    void addRules(const ParseNode* parseNode);
    int binarySearch(const vector<Rule*> &ruleList, const Rule* rule, int compareRule(const Rule* ruleA, const Rule* ruleB)) const;
    void removeSingleNonTerminalFromRightHandSide();
    void updateMultipleNonTerminalFromRightHandSide();
};


#endif //SYNTACTICPARSER_CONTEXTFREEGRAMMAR_H

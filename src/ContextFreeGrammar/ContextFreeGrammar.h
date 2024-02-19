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
    void updateTypes();
    void constructDictionary(const TreeBank& treeBank);
    Symbol getSingleNonTerminalCandidateToRemove(vector<Symbol> removedList) const;
    Rule* getMultipleNonTerminalCandidateToUpdate() const;
    void updateAllMultipleNonTerminalWithNewRule(const Symbol& first, const Symbol& second, const Symbol& with);
public:
    ContextFreeGrammar() = default;
    ContextFreeGrammar(const string& ruleFileName,
                       const string& dictionaryFileName,
                       int minCount);
    ContextFreeGrammar(const TreeBank& treeBank, int minCount);
    void updateTree(ParseTree* parseTree, int _minCount);
    void removeExceptionalWordsFromSentence(Sentence* sentence) const;
    void reinsertExceptionalWordsFromSentence(ParseTree* parseTree, Sentence* sentence) const;
    static Rule* toRule(ParseNode* parseNode, bool trim);
    void addRule(Rule* newRule);
    void removeRule(Rule* rule);
    vector<Rule*> getRulesWithLeftSideX(const Symbol& X);
    vector<Symbol> partOfSpeechTags() const;
    vector<Symbol> getLeftSide() const;
    vector<Rule*> getTerminalRulesWithRightSideX(const Symbol& s) const;
    vector<Rule*> getRulesWithRightSideX(const Symbol& S) const;
    vector<Rule*> getRulesWithTwoNonTerminalsOnRightSide(const Symbol& A, const Symbol& B) const;
    void convertToChomskyNormalForm();
    Rule* searchRule(Rule* rule) const;
    int size() const;
    static bool compareRuleLeft(Rule* ruleA, Rule* ruleB);
    static int compareRuleLeft3Way(Rule* ruleA, Rule* ruleB);
    static bool compareRuleRight(Rule* ruleA, Rule* ruleB);
    static int compareRuleRight3Way(Rule* ruleA, Rule* ruleB);
private:
    void addRules(ParseNode* parseNode);
    int binarySearch(vector<Rule*> ruleList, Rule* rule, int compareRule(Rule* ruleA, Rule* ruleB)) const;
    void removeSingleNonTerminalFromRightHandSide();
    void updateMultipleNonTerminalFromRightHandSide();
};


#endif //SYNTACTICPARSER_CONTEXTFREEGRAMMAR_H

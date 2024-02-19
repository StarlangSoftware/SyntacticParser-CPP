//
// Created by Olcay Taner YILDIZ on 18.02.2024.
//

#ifndef SYNTACTICPARSER_PROBABILISTICCONTEXTFREEGRAMMAR_H
#define SYNTACTICPARSER_PROBABILISTICCONTEXTFREEGRAMMAR_H


#include "../ContextFreeGrammar/ContextFreeGrammar.h"
#include "../ProbabilisticContextFreeGrammar/ProbabilisticRule.h"

class ProbabilisticContextFreeGrammar : public ContextFreeGrammar{
public:
    ProbabilisticContextFreeGrammar() = default;
    ProbabilisticContextFreeGrammar(const string& ruleFileName,
                                    const string& dictionaryFileName,
                                    int minCount);
    ProbabilisticContextFreeGrammar(const TreeBank& treeBank, int minCount);
    static ProbabilisticRule* toRule(ParseNode* parseNode, bool trim);
    double probability(ParseTree* parseTree);
    void convertToChomskyNormalForm();
private:
    void addRules(ParseNode* parseNode);
    double probability(ParseNode* parseNode);
    void removeSingleNonTerminalFromRightHandSide();
    void updateMultipleNonTerminalFromRightHandSide();
};


#endif //SYNTACTICPARSER_PROBABILISTICCONTEXTFREEGRAMMAR_H

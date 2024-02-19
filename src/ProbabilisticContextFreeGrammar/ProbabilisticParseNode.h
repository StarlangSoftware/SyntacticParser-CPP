//
// Created by Olcay Taner YILDIZ on 17.02.2024.
//

#ifndef SYNTACTICPARSER_PROBABILISTICPARSENODE_H
#define SYNTACTICPARSER_PROBABILISTICPARSENODE_H

#include <ParseNode.h>

class ProbabilisticParseNode : public ParseNode{
private:
    double logProbability;
public:
    ProbabilisticParseNode(ParseNode* left, ParseNode* right, const Symbol& data, double logProbability);
    ProbabilisticParseNode(ParseNode* left, const Symbol& data, double logProbability);
    ProbabilisticParseNode(const Symbol& data, double logProbability);
    double getLogProbability();
};

#endif //SYNTACTICPARSER_PROBABILISTICPARSENODE_H

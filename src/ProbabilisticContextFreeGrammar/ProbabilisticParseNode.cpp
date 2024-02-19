//
// Created by Olcay Taner YILDIZ on 17.02.2024.
//

#include "ProbabilisticParseNode.h"

ProbabilisticParseNode::ProbabilisticParseNode(ParseNode *left, ParseNode *right, const Symbol& data, double logProbability) : ParseNode(left, right, data){
    this->logProbability = logProbability;
}

ProbabilisticParseNode::ProbabilisticParseNode(ParseNode *left, const Symbol& data, double logProbability) : ParseNode(left, data){
    this->logProbability = logProbability;
}

ProbabilisticParseNode::ProbabilisticParseNode(const Symbol &data, double logProbability) : ParseNode(data){
    this->logProbability = logProbability;
}

double ProbabilisticParseNode::getLogProbability() {
    return logProbability;
}

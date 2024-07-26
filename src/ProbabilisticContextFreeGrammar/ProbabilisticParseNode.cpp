//
// Created by Olcay Taner YILDIZ on 17.02.2024.
//

#include "ProbabilisticParseNode.h"

/**
 * Constructor for the ProbabilisticParseNode class. Extends the parse node with a probability.
 * @param left Left child of this node.
 * @param right Right child of this node.
 * @param data Data for this node.
 * @param logProbability Logarithm of the probability of the node.
 */
ProbabilisticParseNode::ProbabilisticParseNode(ParseNode *left, ParseNode *right, const Symbol& data, double logProbability) : ParseNode(left, right, data){
    this->logProbability = logProbability;
}

/**
 * Another constructor for the ProbabilisticParseNode class.
 * @param left Left child of this node.
 * @param data Data for this node.
 * @param logProbability Logarithm of the probability of the node.
 */
ProbabilisticParseNode::ProbabilisticParseNode(ParseNode *left, const Symbol& data, double logProbability) : ParseNode(left, data){
    this->logProbability = logProbability;
}

/**
 * Another constructor for the ProbabilisticParseNode class.
 * @param data Data for this node.
 * @param logProbability Logarithm of the probability of the node.
 */
ProbabilisticParseNode::ProbabilisticParseNode(const Symbol &data, double logProbability) : ParseNode(data){
    this->logProbability = logProbability;
}

/**
 * Accessor for the logProbability attribute.
 * @return logProbability attribute.
 */
double ProbabilisticParseNode::getLogProbability() {
    return logProbability;
}

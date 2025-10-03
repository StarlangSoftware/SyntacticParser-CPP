//
// Created by Olcay Taner YILDIZ on 19.02.2024.
//

#include "ProbabilisticCYKParser.h"

#include <cmath>

#include "../SyntacticParser/PartialParseList.h"

/**
 * Constructs an array of most probable parse trees for a given sentence according to the given grammar. CYK parser
 * is based on a dynamic programming algorithm.
 * @param pCfg Probabilistic context free grammar used in parsing.
 * @param sentence Sentence to be parsed.
 * @return Array list of most probable parse trees for the given sentence.
 */
vector<ParseTree*> ProbabilisticCYKParser::parse(const ProbabilisticContextFreeGrammar& pCfg, Sentence* sentence){
    int i, j, k, x, y;
    PartialParseList table[sentence->wordCount()][sentence->wordCount()];
    ProbabilisticParseNode* leftNode, *rightNode;
    double bestProbability, probability;
    vector<Rule*> candidates;
    vector<ParseTree*> parseTrees;
    auto* backUp = new Sentence();
    for (i = 0; i < sentence->wordCount(); i++){
        backUp->addWord(new Word(sentence->getWord(i)->getName()));
    }
    pCfg.removeExceptionalWordsFromSentence(sentence);
    for (i = 0; i < sentence->wordCount(); i++)
        for (j = i; j < sentence->wordCount(); j++)
            table[i][j] = PartialParseList();
    for (i = 0; i < sentence->wordCount(); i++){
        candidates = pCfg.getTerminalRulesWithRightSideX(Symbol(sentence->getWord(i)->getName()));
        for (Rule* candidate: candidates){
            table[i][i].addPartialParse(new ProbabilisticParseNode(new ParseNode(Symbol(sentence->getWord(i)->getName())), candidate->getLeftHandSide(), log(((ProbabilisticRule*) candidate)->getProbability())));
        }
    }
    for (j = 1; j < sentence->wordCount(); j++){
        for (i = j - 1; i >= 0; i--){
            for (k = i; k < j; k++)
                for (x = 0; x < table[i][k].size(); x++)
                    for (y = 0; y < table[k + 1][j].size(); y++){
                        leftNode = (ProbabilisticParseNode*) table[i][k].getPartialParse(x);
                        rightNode = (ProbabilisticParseNode*) table[k + 1][j].getPartialParse(y);
                        candidates = pCfg.getRulesWithTwoNonTerminalsOnRightSide(leftNode->getData(), rightNode->getData());
                        for (Rule* candidate: candidates){
                            probability = log(((ProbabilisticRule*) candidate)->getProbability()) + leftNode->getLogProbability() + rightNode->getLogProbability();
                            table[i][j].updatePartialParse(new ProbabilisticParseNode(leftNode, rightNode, candidate->getLeftHandSide(), probability));
                        }
                    }
        }
    }
    bestProbability = -MAXFLOAT;
    for (i = 0; i < table[0][sentence->wordCount() - 1].size(); i++){
        if (table[0][sentence->wordCount() - 1].getPartialParse(i)->getData().getName() == "S" && ((ProbabilisticParseNode*) table[0][sentence->wordCount() - 1].getPartialParse(i))->getLogProbability() > bestProbability) {
            bestProbability = ((ProbabilisticParseNode*) table[0][sentence->wordCount() - 1].getPartialParse(i))->getLogProbability();
        }
    }
    for (i = 0; i < table[0][sentence->wordCount() - 1].size(); i++){
        if (table[0][sentence->wordCount() - 1].getPartialParse(i)->getData().getName() == "S" && ((ProbabilisticParseNode*) table[0][sentence->wordCount() - 1].getPartialParse(i))->getLogProbability() == bestProbability) {
            auto* parseTree = new ParseTree(table[0][sentence->wordCount() - 1].getPartialParse(i));
            parseTree->correctParents();
            parseTree->removeXNodes();
            parseTrees.emplace_back(parseTree);
        }
    }
    for (ParseTree* parseTree : parseTrees){
        pCfg.reinsertExceptionalWordsFromSentence(parseTree, backUp);
    }
    return parseTrees;
}
//
// Created by Olcay Taner YILDIZ on 19.02.2024.
//

#include "CYKParser.h"
#include "PartialParseList.h"

vector<ParseTree*> CYKParser::parse(const ContextFreeGrammar& cfg, Sentence* sentence){
    int i, j, k, x, y;
    PartialParseList table[sentence->wordCount()][sentence->wordCount()];
    ParseNode *leftNode, *rightNode;
    vector<Rule*> candidates;
    vector<ParseTree*> parseTrees;
    auto* backUp = new Sentence();
    for (i = 0; i < sentence->wordCount(); i++){
        backUp->addWord(new Word(sentence->getWord(i)->getName()));
    }
    cfg.removeExceptionalWordsFromSentence(sentence);
    for (i = 0; i < sentence->wordCount(); i++)
        for (j = i; j < sentence->wordCount(); j++)
            table[i][j] = PartialParseList();
    for (i = 0; i < sentence->wordCount(); i++){
        candidates = cfg.getTerminalRulesWithRightSideX(Symbol(sentence->getWord(i)->getName()));
        for (Rule* candidate: candidates){
            table[i][i].addPartialParse(new ParseNode(new ParseNode(Symbol(sentence->getWord(i)->getName())), candidate->getLeftHandSide()));
        }
    }
    for (j = 1; j < sentence->wordCount(); j++){
        for (i = j - 1; i >= 0; i--)
            for (k = i; k < j; k++){
                for (x = 0; x < table[i][k].size(); x++)
                    for (y = 0; y < table[k + 1][j].size(); y++){
                        leftNode = table[i][k].getPartialParse(x);
                        rightNode = table[k + 1][j].getPartialParse(y);
                        candidates = cfg.getRulesWithTwoNonTerminalsOnRightSide(leftNode->getData(), rightNode->getData());
                        for (Rule* candidate: candidates){
                            table[i][j].addPartialParse(new ParseNode(leftNode, rightNode, candidate->getLeftHandSide()));
                        }
                    }
            }
    }
    for (i = 0; i < table[0][sentence->wordCount() - 1].size(); i++){
        if (table[0][sentence->wordCount() - 1].getPartialParse(i)->getData().getName() == "S") {
            auto* parseTree = new ParseTree(table[0][sentence->wordCount() - 1].getPartialParse(i));
            parseTree->correctParents();
            parseTree->removeXNodes();
            parseTrees.emplace_back(parseTree);
        }
    }
    for (ParseTree* parseTree : parseTrees){
        cfg.reinsertExceptionalWordsFromSentence(parseTree, backUp);
    }
    return parseTrees;
}
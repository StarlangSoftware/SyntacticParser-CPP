//
// Created by Olcay Taner YILDIZ on 19.02.2024.
//

#ifndef SYNTACTICPARSER_PROBABILISTICCYKPARSER_H
#define SYNTACTICPARSER_PROBABILISTICCYKPARSER_H


#include "ProbabilisticParser.h"

class ProbabilisticCYKParser : public ProbabilisticParser{
public:
    vector<ParseTree*> parse(const ProbabilisticContextFreeGrammar& pCfg, Sentence* sentence);
};


#endif //SYNTACTICPARSER_PROBABILISTICCYKPARSER_H

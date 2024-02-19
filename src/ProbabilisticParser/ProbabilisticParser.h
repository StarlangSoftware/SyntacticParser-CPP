//
// Created by Olcay Taner YILDIZ on 19.02.2024.
//

#ifndef SYNTACTICPARSER_PROBABILISTICPARSER_H
#define SYNTACTICPARSER_PROBABILISTICPARSER_H
#include "ParseTree.h"
#include "../ProbabilisticContextFreeGrammar/ProbabilisticContextFreeGrammar.h"
#include "Sentence.h"

class ProbabilisticParser {
public:
    vector<ParseTree*> parse(const ProbabilisticContextFreeGrammar& pCfg, Sentence* sentence);
};


#endif //SYNTACTICPARSER_PROBABILISTICPARSER_H

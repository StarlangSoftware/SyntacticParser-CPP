//
// Created by Olcay Taner YILDIZ on 19.02.2024.
//

#ifndef SYNTACTICPARSER_SYNTACTICPARSER_H
#define SYNTACTICPARSER_SYNTACTICPARSER_H
#include "ParseTree.h"
#include "../ContextFreeGrammar/ContextFreeGrammar.h"
#include "Sentence.h"

class SyntacticParser {
public:
    virtual vector<ParseTree*> parse(const ContextFreeGrammar& cfg, Sentence* sentence);
};


#endif //SYNTACTICPARSER_SYNTACTICPARSER_H

//
// Created by Olcay Taner YILDIZ on 19.02.2024.
//

#ifndef SYNTACTICPARSER_CYKPARSER_H
#define SYNTACTICPARSER_CYKPARSER_H


#include "SyntacticParser.h"

class CYKParser : public SyntacticParser{
public:
    vector<ParseTree*> parse(const ContextFreeGrammar& cfg, Sentence* sentence);
};


#endif //SYNTACTICPARSER_CYKPARSER_H

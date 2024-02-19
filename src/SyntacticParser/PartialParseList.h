//
// Created by Olcay Taner YILDIZ on 19.02.2024.
//

#ifndef SYNTACTICPARSER_PARTIALPARSELIST_H
#define SYNTACTICPARSER_PARTIALPARSELIST_H


#include <ParseNode.h>
#include "../ProbabilisticContextFreeGrammar/ProbabilisticParseNode.h"

class PartialParseList {
private:
    vector<ParseNode*> partialParses;
public:
    PartialParseList() = default;
    void addPartialParse(ParseNode* parseNode);
    void updatePartialParse(ProbabilisticParseNode* parseNode);
    ParseNode* getPartialParse(int index);
    int size();
};


#endif //SYNTACTICPARSER_PARTIALPARSELIST_H

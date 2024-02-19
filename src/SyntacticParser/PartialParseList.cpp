//
// Created by Olcay Taner YILDIZ on 19.02.2024.
//

#include "PartialParseList.h"

void PartialParseList::addPartialParse(ParseNode* parseNode){
    partialParses.emplace_back(parseNode);
}

void PartialParseList::updatePartialParse(ProbabilisticParseNode* parseNode){
    bool found = false;
    for (int i = 0; i < partialParses.size(); i++){
        ParseNode* partialParse = partialParses[i];
        if (partialParse->getData().getName() == parseNode->getData().getName()){
            if (((ProbabilisticParseNode *) partialParse)->getLogProbability() < parseNode->getLogProbability()){
                partialParses.erase(partialParses.begin() + i);
                partialParses.emplace_back(parseNode);
            }
            found = true;
            break;
        }
    }
    if (!found){
        partialParses.emplace_back(parseNode);
    }
}

ParseNode* PartialParseList::getPartialParse(int index){
    return partialParses[index];
}

int PartialParseList::size(){
    return partialParses.size();
}
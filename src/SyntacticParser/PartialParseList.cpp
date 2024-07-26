//
// Created by Olcay Taner YILDIZ on 19.02.2024.
//

#include "PartialParseList.h"

/**
 * Adds a new partial parse (actually a parse node representing the root of the subtree of the partial parse)
 * @param parseNode Root of the subtree showing the partial parse.
 */
void PartialParseList::addPartialParse(ParseNode* parseNode){
    partialParses.emplace_back(parseNode);
}

/**
 * Updates the partial parse by removing less probable nodes with the given parse node.
 * @param parseNode Parse node to be added to the partial parse.
 */
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

/**
 * Accessor for the partialParses array list.
 * @param index Position of the parse node.
 * @return Parse node at the given position.
 */
ParseNode* PartialParseList::getPartialParse(int index){
    return partialParses[index];
}

/**
 * Returns size of the partial parse.
 * @return Size of the partial parse.
 */
int PartialParseList::size(){
    return partialParses.size();
}
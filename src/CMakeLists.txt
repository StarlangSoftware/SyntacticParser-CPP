cmake_minimum_required(VERSION 3.12)
project(SyntacticParser)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-O3")

find_package(parse_tree REQUIRED)
find_package(data_structure REQUIRED)
find_package(corpus REQUIRED)

add_library(SyntacticParser
        ContextFreeGrammar/RuleType.h
        ContextFreeGrammar/Rule.cpp
        ContextFreeGrammar/Rule.h
        ContextFreeGrammar/ContextFreeGrammar.cpp
        ContextFreeGrammar/ContextFreeGrammar.h
        ProbabilisticContextFreeGrammar/ProbabilisticParseNode.cpp
        ProbabilisticContextFreeGrammar/ProbabilisticParseNode.h
        ProbabilisticContextFreeGrammar/ProbabilisticRule.cpp
        ProbabilisticContextFreeGrammar/ProbabilisticRule.h
        ProbabilisticContextFreeGrammar/ProbabilisticContextFreeGrammar.cpp
        ProbabilisticContextFreeGrammar/ProbabilisticContextFreeGrammar.h
        SyntacticParser/PartialParseList.cpp
        SyntacticParser/PartialParseList.h
        SyntacticParser/SyntacticParser.h
        ProbabilisticParser/ProbabilisticParser.h
        SyntacticParser/CYKParser.cpp
        SyntacticParser/CYKParser.h
        ProbabilisticParser/ProbabilisticCYKParser.cpp
        ProbabilisticParser/ProbabilisticCYKParser.h)
target_link_libraries(SyntacticParser parse_tree::parse_tree data_structure::data_structure corpus::corpus)

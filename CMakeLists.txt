cmake_minimum_required(VERSION 3.12)
project(SyntacticParser)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-O3")

find_package(parse_tree REQUIRED)
find_package(data_structure REQUIRED)
find_package(corpus REQUIRED)

add_library(SyntacticParser
        src/ContextFreeGrammar/RuleType.h
        src/ContextFreeGrammar/Rule.cpp
        src/ContextFreeGrammar/Rule.h
        src/ContextFreeGrammar/ContextFreeGrammar.cpp
        src/ContextFreeGrammar/ContextFreeGrammar.h
        src/ProbabilisticContextFreeGrammar/ProbabilisticParseNode.cpp
        src/ProbabilisticContextFreeGrammar/ProbabilisticParseNode.h
        src/ProbabilisticContextFreeGrammar/ProbabilisticRule.cpp
        src/ProbabilisticContextFreeGrammar/ProbabilisticRule.h
        src/ProbabilisticContextFreeGrammar/ProbabilisticContextFreeGrammar.cpp
        src/ProbabilisticContextFreeGrammar/ProbabilisticContextFreeGrammar.h
        src/SyntacticParser/PartialParseList.cpp
        src/SyntacticParser/PartialParseList.h
        src/SyntacticParser/SyntacticParser.h
        src/ProbabilisticParser/ProbabilisticParser.h
        src/SyntacticParser/CYKParser.cpp
        src/SyntacticParser/CYKParser.h
        src/ProbabilisticParser/ProbabilisticCYKParser.cpp
        src/ProbabilisticParser/ProbabilisticCYKParser.h)
target_link_libraries(SyntacticParser parse_tree::parse_tree data_structure::data_structure corpus::corpus)

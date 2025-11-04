/*
 * File: program.cpp
 * -----------------
 * This file is a stub implementation of the program.h interface
 * in which none of the methods do anything beyond returning a
 * value of the correct type.  Your job is to fill in the bodies
 * of each of these methods with an implementation that satisfies
 * the performance guarantees specified in the assignment.
 */

#include "program.hpp"



Program::Program() {
    currentLine = -1;
    shouldJump = false;
    jumpTarget = -1;
}

Program::~Program() {
    clear();
}

void Program::clear() {
    for (auto& pair : parsedStatements) {
        delete pair.second;
    }
    parsedStatements.clear();
    sourceLines.clear();
    currentLine = -1;
    shouldJump = false;
    jumpTarget = -1;
}

void Program::addSourceLine(int lineNumber, const std::string &line) {
    if (parsedStatements.find(lineNumber) != parsedStatements.end()) {
        delete parsedStatements[lineNumber];
        parsedStatements.erase(lineNumber);
    }
    sourceLines[lineNumber] = line;
}

void Program::removeSourceLine(int lineNumber) {
    if (parsedStatements.find(lineNumber) != parsedStatements.end()) {
        delete parsedStatements[lineNumber];
        parsedStatements.erase(lineNumber);
    }
    sourceLines.erase(lineNumber);
}

std::string Program::getSourceLine(int lineNumber) {
    if (sourceLines.find(lineNumber) != sourceLines.end()) {
        return sourceLines[lineNumber];
    }
    return "";
}

void Program::setParsedStatement(int lineNumber, Statement *stmt) {
    if (parsedStatements.find(lineNumber) != parsedStatements.end()) {
        delete parsedStatements[lineNumber];
    }
    parsedStatements[lineNumber] = stmt;
}


Statement *Program::getParsedStatement(int lineNumber) {
    if (parsedStatements.find(lineNumber) != parsedStatements.end()) {
        return parsedStatements[lineNumber];
    }
    return nullptr;
}

int Program::getFirstLineNumber() {
    if (sourceLines.empty()) {
        return -1;
    }
    return sourceLines.begin()->first;
}

int Program::getNextLineNumber(int lineNumber) {
    auto it = sourceLines.find(lineNumber);
    if (it == sourceLines.end()) {
        return -1;
    }
    ++it;
    if (it == sourceLines.end()) {
        return -1;
    }
    return it->first;
}

/*
 * File: Basic.cpp
 * ---------------
 * This file is the starter project for the BASIC interpreter.
 */

#include <cctype>
#include <iostream>
#include <string>
#include "exp.hpp"
#include "parser.hpp"
#include "program.hpp"
#include "Utils/error.hpp"
#include "Utils/tokenScanner.hpp"
#include "Utils/strlib.hpp"


/* Function prototypes */

void processLine(std::string line, Program &program, EvalState &state);
Statement* parseStatement(TokenScanner &scanner, const std::string &type);

/* Main program */

int main() {
    EvalState state;
    Program program;
    //cout << "Stub implementation of BASIC" << endl;
    while (true) {
        try {
            std::string input;
            getline(std::cin, input);
            if (input.empty())
                continue;
            processLine(input, program, state);
        } catch (ErrorException &ex) {
            std::cout << ex.getMessage() << std::endl;
        }
    }
    return 0;
}

/*
 * Function: processLine
 * Usage: processLine(line, program, state);
 * -----------------------------------------
 * Processes a single line entered by the user.  In this version of
 * implementation, the program reads a line, parses it as an expression,
 * and then prints the result.  In your implementation, you will
 * need to replace this method with one that can respond correctly
 * when the user enters a program line (which begins with a number)
 * or one of the BASIC commands, such as LIST or RUN.
 */

void processLine(std::string line, Program &program, EvalState &state) {
    TokenScanner scanner;
    scanner.ignoreWhitespace();
    scanner.scanNumbers();
    scanner.setInput(line);

    if (!scanner.hasMoreTokens()) {
        return;
    }

    std::string token = scanner.nextToken();
    
    // Check if the first token is a line number
    if (scanner.getTokenType(token) == NUMBER) {
        int lineNumber = stringToInteger(token);
        
        // If no more tokens, delete the line
        if (!scanner.hasMoreTokens()) {
            program.removeSourceLine(lineNumber);
            return;
        }
        
        // Otherwise, store the line and parse it
        program.addSourceLine(lineNumber, line);
        
        std::string stmtType = scanner.nextToken();
        if (stmtType == "REM" || stmtType == "LET" || stmtType == "PRINT" || 
            stmtType == "INPUT" || stmtType == "GOTO" || stmtType == "IF" || stmtType == "END") {
            // Save current position to restore for statement parsing
            scanner.saveToken(stmtType);
            TokenScanner stmtScanner;
            stmtScanner.ignoreWhitespace();
            stmtScanner.scanNumbers();
            
            // Get remaining part of the line after line number
            std::string remaining = line.substr(line.find(stmtType));
            stmtScanner.setInput(remaining);
            stmtScanner.nextToken(); // consume statement type
            
            Statement *stmt = parseStatement(stmtScanner, stmtType);
            program.setParsedStatement(lineNumber, stmt);
        } else {
            error("SYNTAX ERROR");
        }
    } else if (token == "RUN") {
        // Execute the stored program
        int currentLine = program.getFirstLineNumber();
        while (currentLine != -1) {
            program.shouldJump = false;
            program.jumpTarget = -1;
            program.currentLine = currentLine;
            
            Statement *stmt = program.getParsedStatement(currentLine);
            if (stmt != nullptr) {
                stmt->execute(state, program);
            }
            
            if (program.shouldJump) {
                if (program.jumpTarget == -1) {
                    // END statement
                    break;
                }
                currentLine = program.jumpTarget;
            } else {
                currentLine = program.getNextLineNumber(currentLine);
            }
        }
    } else if (token == "LIST") {
        // List the program
        int currentLine = program.getFirstLineNumber();
        while (currentLine != -1) {
            std::cout << program.getSourceLine(currentLine) << std::endl;
            currentLine = program.getNextLineNumber(currentLine);
        }
    } else if (token == "CLEAR") {
        program.clear();
        state.Clear();
    } else if (token == "QUIT") {
        exit(0);
    } else if (token == "HELP") {
        // Optional, do nothing
    } else if (token == "LET" || token == "PRINT" || token == "INPUT") {
        // Direct execution of statement
        scanner.saveToken(token);
        TokenScanner stmtScanner;
        stmtScanner.ignoreWhitespace();
        stmtScanner.scanNumbers();
        stmtScanner.setInput(line);
        stmtScanner.nextToken(); // consume statement type
        
        Statement *stmt = parseStatement(stmtScanner, token);
        stmt->execute(state, program);
        delete stmt;
    } else {
        error("SYNTAX ERROR");
    }
}

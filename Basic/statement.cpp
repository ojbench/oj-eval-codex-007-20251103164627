/*
 * File: statement.cpp
 * -------------------
 * This file implements the constructor and destructor for
 * the Statement class itself.  Your implementation must do
 * the same for the subclasses you define for each of the
 * BASIC statements.
 */

#include "statement.hpp"


/* Implementation of the Statement class */

int stringToInt(std::string str);

Statement::Statement() = default;

Statement::~Statement() = default;

// REM statement
class RemStmt : public Statement {
public:
    RemStmt() {}
    void execute(EvalState &state, Program &program) override {}
};

// LET statement
class LetStmt : public Statement {
private:
    std::string var;
    Expression *exp;
public:
    LetStmt(const std::string &variable, Expression *expression) : var(variable), exp(expression) {}
    ~LetStmt() { delete exp; }
    void execute(EvalState &state, Program &program) override {
        int value = exp->eval(state);
        state.setValue(var, value);
    }
};

// PRINT statement
class PrintStmt : public Statement {
private:
    Expression *exp;
public:
    PrintStmt(Expression *expression) : exp(expression) {}
    ~PrintStmt() { delete exp; }
    void execute(EvalState &state, Program &program) override {
        std::cout << exp->eval(state) << std::endl;
    }
};

// INPUT statement
class InputStmt : public Statement {
private:
    std::string var;
public:
    InputStmt(const std::string &variable) : var(variable) {}
    void execute(EvalState &state, Program &program) override {
        std::cout << " ? ";
        std::string line;
        int value;
        while (true) {
            getline(std::cin, line);
            try {
                std::stringstream ss(line);
                ss >> value;
                if (!ss.fail() && ss.eof()) {
                    break;
                }
                std::cout << "INVALID NUMBER" << std::endl;
                std::cout << " ? ";
            } catch (...) {
                std::cout << "INVALID NUMBER" << std::endl;
                std::cout << " ? ";
            }
        }
        state.setValue(var, value);
    }
};

// GOTO statement
class GotoStmt : public Statement {
private:
    int lineNumber;
public:
    GotoStmt(int line) : lineNumber(line) {}
    void execute(EvalState &state, Program &program) override {
        if (program.getSourceLine(lineNumber).empty()) {
            error("LINE NUMBER ERROR");
        }
        program.shouldJump = true;
        program.jumpTarget = lineNumber;
    }
};

// IF statement
class IfStmt : public Statement {
private:
    Expression *lhs;
    Expression *rhs;
    std::string op;
    int lineNumber;
public:
    IfStmt(Expression *left, Expression *right, const std::string &oper, int line) 
        : lhs(left), rhs(right), op(oper), lineNumber(line) {}
    ~IfStmt() {
        delete lhs;
        delete rhs;
    }
    void execute(EvalState &state, Program &program) override {
        int leftValue = lhs->eval(state);
        int rightValue = rhs->eval(state);
        bool condition = false;
        if (op == "=") {
            condition = (leftValue == rightValue);
        } else if (op == "<") {
            condition = (leftValue < rightValue);
        } else if (op == ">") {
            condition = (leftValue > rightValue);
        }
        if (condition) {
            if (program.getSourceLine(lineNumber).empty()) {
                error("LINE NUMBER ERROR");
            }
            program.shouldJump = true;
            program.jumpTarget = lineNumber;
        }
    }
};

// END statement
class EndStmt : public Statement {
public:
    EndStmt() {}
    void execute(EvalState &state, Program &program) override {
        program.shouldJump = true;
        program.jumpTarget = -1;
    }
};

Statement* parseStatement(TokenScanner &scanner, const std::string &type) {
    if (type == "REM") {
        return new RemStmt();
    } else if (type == "LET") {
        std::string var = scanner.nextToken();
        if (var == "REM" || var == "LET" || var == "PRINT" || var == "INPUT" || 
            var == "END" || var == "GOTO" || var == "IF" || var == "THEN" || 
            var == "RUN" || var == "LIST" || var == "CLEAR" || var == "QUIT" || var == "HELP") {
            error("SYNTAX ERROR");
        }
        std::string eq = scanner.nextToken();
        if (eq != "=") {
            error("SYNTAX ERROR");
        }
        Expression *exp = parseExp(scanner);
        return new LetStmt(var, exp);
    } else if (type == "PRINT") {
        Expression *exp = parseExp(scanner);
        return new PrintStmt(exp);
    } else if (type == "INPUT") {
        std::string var = scanner.nextToken();
        if (var == "REM" || var == "LET" || var == "PRINT" || var == "INPUT" || 
            var == "END" || var == "GOTO" || var == "IF" || var == "THEN" || 
            var == "RUN" || var == "LIST" || var == "CLEAR" || var == "QUIT" || var == "HELP") {
            error("SYNTAX ERROR");
        }
        return new InputStmt(var);
    } else if (type == "GOTO") {
        std::string lineStr = scanner.nextToken();
        int lineNumber = stringToInteger(lineStr);
        return new GotoStmt(lineNumber);
    } else if (type == "IF") {
        Expression *lhs = parseExp(scanner);
        std::string op = scanner.nextToken();
        if (op != "=" && op != "<" && op != ">") {
            error("SYNTAX ERROR");
        }
        Expression *rhs = parseExp(scanner);
        std::string then = scanner.nextToken();
        if (then != "THEN") {
            error("SYNTAX ERROR");
        }
        std::string lineStr = scanner.nextToken();
        int lineNumber = stringToInteger(lineStr);
        return new IfStmt(lhs, rhs, op, lineNumber);
    } else if (type == "END") {
        return new EndStmt();
    }
    error("SYNTAX ERROR");
    return nullptr;
}

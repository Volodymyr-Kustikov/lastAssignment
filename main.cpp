#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cctype>
#include <cmath>
#include <algorithm>
#include <map>
#include <functional>

using namespace std;

struct CustomFunction {
    string name;
    string param1;
    string param2;
    string body;
};

class SimpleInterpreter {
private:
    string input;
    size_t pos;
    map<string, double> variables;
    map<string, CustomFunction> customFunctions;

public:
    SimpleInterpreter() : pos(0) {}

    double evaluate(const string& expression) {
        input = expression;
        pos = 0;

        if (isVariableAssignment()) {
            return handleVariableAssignment();
        }

        if (isFunctionDefinition()) {
            return handleFunctionDefinition();
        }

        return parseExpression();
    }

private:
    void skipWhitespace() {
        while (pos < input.length() && isspace(input[pos])) {
            pos++;
        }
    }

    bool isVariableAssignment() {
        size_t savePos = pos;
        skipWhitespace();

        if (pos + 3 < input.length() && input.substr(pos, 3) == "var") {
            pos = savePos;
            return true;
        }

        pos = savePos;
        return false;
    }

    bool isFunctionDefinition() {
        size_t savePos = pos;
        skipWhitespace();

        if (pos + 3 < input.length() && input.substr(pos, 3) == "def") {
            pos = savePos;
            return true;
        }

        pos = savePos;
        return false;
    }

    double handleVariableAssignment() {
        skipWhitespace();

        pos += 3;
        skipWhitespace();

        string varName = parseIdentifier();
        skipWhitespace();

        if (pos >= input.length() || input[pos] != '=') {
            cerr << "Error: Expected '=' in variable assignment" << endl;
            return 0;
        }
        pos++;

        double value = parseExpression();

        variables[varName] = value;

        return value;
    }

    double handleFunctionDefinition() {
        skipWhitespace();

        pos += 3;
        skipWhitespace();

        string funcName = parseIdentifier();
        skipWhitespace();

        if (pos >= input.length() || input[pos] != '(') {
            cerr << "Error: Expected '(' in function definition" << endl;
            return 0;
        }
        pos++;

        string param1 = parseIdentifier();
        skipWhitespace();

        if (pos >= input.length() || input[pos] != ',') {
            cerr << "Error: Expected ',' between parameters" << endl;
            return 0;
        }
        pos++;

        string param2 = parseIdentifier();
        skipWhitespace();

        if (pos >= input.length() || input[pos] != ')') {
            cerr << "Error: Expected ')' in function definition" << endl;
            return 0;
        }
        pos++;

        skipWhitespace();

        // Expect '{'
        if (pos >= input.length() || input[pos] != '{') {
            cerr << "Error: Expected '{' in function definition" << endl;
            return 0;
        }
        pos++;

        string body;
        while (pos < input.length() && input[pos] != '}') {
            body += input[pos];
            pos++;
        }

        if (pos >= input.length()) {
            cerr << "Error: Expected '}' to close function definition" << endl;
            return 0;
        }
        pos++;

        CustomFunction func;
        func.name = funcName;
        func.param1 = param1;
        func.param2 = param2;
        func.body = body;
        customFunctions[funcName] = func;

        return 0;
    }

    string parseIdentifier() {
        skipWhitespace();
        string name;

        while (pos < input.length() && (isalnum(input[pos]) || input[pos] == '_')) {
            name += input[pos];
            pos++;
        }

        return name;
    }

    double parseNumber() {
        skipWhitespace();

        double result = 0;
        bool hasDecimal = false;
        double decimalPlace = 0.1;

        bool isNegative = false;
        if (pos < input.length() && input[pos] == '-') {
            isNegative = true;
            pos++;
        }

        while (pos < input.length() && isdigit(input[pos])) {
            result = result * 10 + (input[pos] - '0');
            pos++;
        }

        if (pos < input.length() && input[pos] == '.') {
            pos++;
            hasDecimal = true;
            while (pos < input.length() && isdigit(input[pos])) {
                result += (input[pos] - '0') * decimalPlace;
                decimalPlace *= 0.1;
                pos++;
            }
        }

        return isNegative ? -result : result;
    }

    double parseFunctionCall(const string& functionName) {
        skipWhitespace();

        if (pos >= input.length() || input[pos] != '(') {
            cerr << "Error: Expected '(' after function name" << endl;
            return 0;
        }
        pos++;

        double param1 = parseExpression();

        skipWhitespace();

        if (pos >= input.length() || input[pos] != ',') {
            cerr << "Error: Expected ',' between function parameters" << endl;
            return 0;
        }
        pos++;

        double param2 = parseExpression();

        skipWhitespace();

        if (pos >= input.length() || input[pos] != ')') {
            cerr << "Error: Expected ')' after function parameters" << endl;
            return 0;
        }
        pos++;

        return executeFunction(functionName, param1, param2);
    }

    double executeFunction(const string& name, double param1, double param2) {
        if (name == "pow") {
            return pow(param1, param2);
        } else if (name == "abs") {
            return abs(param1);
        } else if (name == "max") {
            return max(param1, param2);
        } else if (name == "min") {
            return min(param1, param2);
        }
        else if (customFunctions.find(name) != customFunctions.end()) {
            return executeCustomFunction(name, param1, param2);
        } else {
            cerr << "Error: Unknown function '" << name << "'" << endl;
            return 0;
        }
    }

    double executeCustomFunction(const string& name, double param1, double param2) {
        CustomFunction func = customFunctions[name];

        map<string, double> savedVars = variables;

        variables[func.param1] = param1;
        variables[func.param2] = param2;

        SimpleInterpreter bodyInterpreter;
        bodyInterpreter.variables = variables;
        bodyInterpreter.customFunctions = customFunctions;

        double result = bodyInterpreter.evaluate(func.body);

        variables = savedVars;

        return result;
    }

    double parseTerm() {
        double result = parseFactor();

        while (pos < input.length()) {
            skipWhitespace();

            if (pos >= input.length()) break;

            char op = input[pos];
            if (op == '*' || op == '/') {
                pos++;
                double right = parseFactor();

                if (op == '*') {
                    result *= right;
                } else if (op == '/') {
                    if (right == 0) {
                        cerr << "Error: Division by zero" << endl;
                        return 0;
                    }
                    result /= right;
                }
            } else {
                break;
            }
        }

        return result;
    }

    double parseFactor() {
        skipWhitespace();

        if (pos >= input.length()) {
            return 0;
        }

        if (input[pos] == '(') {
            pos++;
            double result = parseExpression();
            skipWhitespace();
            if (pos < input.length() && input[pos] == ')') {
                pos++;
            }
            return result;
        }

        if (isalpha(input[pos])) {
            size_t savePos = pos;
            string name = parseIdentifier();

            skipWhitespace();

            if (pos < input.length() && input[pos] == '(') {
                return parseFunctionCall(name);
            }
            else {
                if (variables.find(name) != variables.end()) {
                    return variables[name];
                } else {
                    cerr << "Error: Unknown variable '" << name << "'" << endl;
                    return 0;
                }
            }
        }

        return parseNumber();
    }

    double parseExpression() {
        double result = parseTerm();

        while (pos < input.length()) {
            skipWhitespace();

            if (pos >= input.length()) break;

            char op = input[pos];
            if (op == '+' || op == '-') {
                pos++;
                double right = parseTerm();

                if (op == '+') {
                    result += right;
                } else {
                    result -= right;
                }
            } else {
                break;
            }
        }

        return result;
    }
};

int main() {
    SimpleInterpreter interpreter;
    string input;

    cout << "Advanced Arithmetic Interpreter" << endl;
    cout << "Supported features:" << endl;
    cout << "- Basic arithmetic: +, -, *, /" << endl;
    cout << "- Built-in functions: pow(x,y), abs(x,y), max(x,y), min(x,y)" << endl;
    cout << "- Variables: var name = expression" << endl;
    cout << "- Custom functions: def name(a,b) { expression }" << endl;
    cout << "- Expression combinations and nested calls" << endl;
    cout << "Enter expressions (type 'quit' to exit):" << endl;

    while (true) {
        cout << "> ";
        getline(cin, input);

        if (input == "quit" || input == "exit") {
            break;
        }

        if (input.empty()) {
            continue;
        }

        try {
            double result = interpreter.evaluate(input);

            if (input.find("def ") == string::npos) {
                cout << result << endl;
            }
        } catch (const exception& e) {
            cout << "Error: " << e.what() << endl;
        }
    }

    return 0;
}
#include <iostream>
#include <stack>
#include <vector>
#include <sstream>
#include <cctype>
#include <cmath>
using namespace std;

// Determinăm prioritatea operatorilor
int precedence(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    if (op == '^') return 3; // Ridicare la putere
    return 0;
}

// Verificăm dacă un caracter este operator
bool isOperator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '^';
}

// Conversie expresie infixată -> forma poloneză inversă (postfixată)
vector<string> infixToPostfix(const string& expr) {
    stack<char> ops;
    vector<string> postfix;
    string number;

    for (size_t i = 0; i < expr.length(); i++) {
        if (isdigit(expr[i]) || expr[i] == '.') { // Construim numere reale
            number += expr[i];
        } else {
            if (!number.empty()) { // Dacă avem un număr în construcție
                postfix.push_back(number);
                number.clear();
            }

            if (expr[i] == '(') {
                ops.push(expr[i]);
            } else if (expr[i] == ')') {
                while (!ops.empty() && ops.top() != '(') {
                    postfix.push_back(string(1, ops.top()));
                    ops.pop();
                }
                ops.pop(); // Scoatem '(' din stivă
            } else if (isOperator(expr[i])) {
                while (!ops.empty() && precedence(ops.top()) >= precedence(expr[i])) {
                    postfix.push_back(string(1, ops.top()));
                    ops.pop();
                }
                ops.push(expr[i]);
            }
        }
    }

    // Adăugăm ultimul număr, dacă există
    if (!number.empty()) {
        postfix.push_back(number);
    }

    // Golim operatorii rămași
    while (!ops.empty()) {
        postfix.push_back(string(1, ops.top()));
        ops.pop();
    }

    return postfix;
}

// Evaluăm expresia în forma poloneză inversă (postfixată)
double evaluatePostfix(const vector<string>& postfix) {
    stack<double> values;

    for (const string& token : postfix) {
        if (isdigit(token[0]) || (token[0] == '-' && token.length() > 1)) { // Dacă este operand (număr)
            values.push(stod(token));
        } else { // Dacă este operator
            double b = values.top(); values.pop();
            double a = values.top(); values.pop();
            switch (token[0]) {
                case '+': values.push(a + b); break;
                case '-': values.push(a - b); break;
                case '*': values.push(a * b); break;
                case '/': values.push(a / b); break;
                case '^': values.push(pow(a, b)); break;
            }
        }
    }

    return values.top(); // Rezultatul final
}

// Înlocuiește substring-ul "**" cu "^"
void replaceSubstring(string& str, const string& from, const string& to) {
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Avansează după înlocuire
    }
}

int main() {
    string expr = "2.5*(54-52)**2+6/2-2**2*10"; // Expresia inițială
    replaceSubstring(expr, "**", "^"); // Înlocuim "**" cu "^"

    cout << "Expresia inițială: " << expr << endl;

    // Pasul 1: Conversie în forma poloneză inversă
    vector<string> postfix = infixToPostfix(expr);

    // Afișăm forma poloneză inversă
    cout << "Forma poloneză inversă (postfixată): ";
    for (const string& token : postfix) {
        cout << token << " ";
    }
    cout << endl;

    // Pasul 2: Evaluare postfixată
    double result = evaluatePostfix(postfix);
    cout << "Rezultatul: " << result << endl;

    return 0;
}

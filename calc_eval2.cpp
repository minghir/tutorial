#include <iostream>
#include <stack>
#include <string>
#include <cmath>
#include <cctype>
using namespace std;

int precedence(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    if (op == '^') return 3; // Prioritate ridicată pentru ridicare la putere.
    return 0;
}

double applyOperation(double a, double b, char op) {
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': return a / b;
        case '^': return pow(a, b); // Funcția pow() pentru ridicare la putere.
    }
    return 0;
}

double evaluateExpression(const string& expression) {
    stack<double> values; // Schimbăm de la int la double.
    stack<char> ops;

    for (size_t i = 0; i < expression.length(); i++) {
        if (isdigit(expression[i]) || expression[i] == '.') { // Gestionează numere reale.
            string number;
            while (i < expression.length() && (isdigit(expression[i]) || expression[i] == '.')) {
                number += expression[i];
                i++;
            }
            values.push(stod(number)); // Convertim șirul în double.
            i--; // Revine un pas.
        } else if (expression[i] == '(') {
            ops.push(expression[i]);
        } else if (expression[i] == ')') {
            while (!ops.empty() && ops.top() != '(') {
                double b = values.top(); values.pop();
                double a = values.top(); values.pop();
                char op = ops.top(); ops.pop();
                values.push(applyOperation(a, b, op));
            }
            ops.pop(); // Scoatem '(' din stivă.
        } else if (expression[i] == '+' || expression[i] == '-' ||
                   expression[i] == '*' || expression[i] == '/' ||
                   expression[i] == '^') { // Operatorii matematici.
            while (!ops.empty() && precedence(ops.top()) >= precedence(expression[i])) {
                double b = values.top(); values.pop();
                double a = values.top(); values.pop();
                char op = ops.top(); ops.pop();
                values.push(applyOperation(a, b, op));
            }
            ops.push(expression[i]);
        }
    }

    while (!ops.empty()) {
        double b = values.top(); values.pop();
        double a = values.top(); values.pop();
        char op = ops.top(); ops.pop();
        values.push(applyOperation(a, b, op));
    }

    return values.top();
}

void replaceSubstring(string& str, const string& from, const string& to) {
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Avansează după înlocuire.
    }
}

int main() {
    string expr = "2.5*(54-52)**2+6/2-2**2*10"; // Expresie cu numere reale.
    replaceSubstring(expr, "**", "^"); // Înlocuiește ** cu ^.
    cout << "Rezultatul expresiei \"" << expr << "\" este: " << evaluateExpression(expr) << endl;
    return 0;
}

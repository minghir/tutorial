#include <iostream>
#include <stack>
#include <string>
#include <cctype>
#include <cmath>
using namespace std;

void replaceSubstring(string& str, const string& from, const string& to) {
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Avansăm după înlocuire.
    }
}


int precedence(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    if (op == '^') return 3; // Pentru ** (ridicare la putere).
    return 0;
}

int applyOperation(int a, int b, char op) {
    switch(op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': return a / b;
        case '^': return pow(a, b); // Funcția pow() pentru ridicare la putere.
    }
    return 0;
}

int evaluateExpression(const string& expression) {
    stack<int> values;
    stack<char> ops;

    for (size_t i = 0; i < expression.length(); i++) {
        if (isdigit(expression[i])) {
            int value = 0;
            while (i < expression.length() && isdigit(expression[i])) {
                value = (value * 10) + (expression[i] - '0');
                i++;
            }
            values.push(value);
            i--; // Revine un pas, pentru că avansăm prea mult.
        } else if (expression[i] == '(') {
            ops.push(expression[i]);
        } else if (expression[i] == ')') {
            while (!ops.empty() && ops.top() != '(') {
                int b = values.top(); values.pop();
                int a = values.top(); values.pop();
                char op = ops.top(); ops.pop();
                values.push(applyOperation(a, b, op));
            }
            ops.pop(); // Scoate '(' din stivă.
        } else if (expression[i] == '+' || expression[i] == '-' || 
                   expression[i] == '*' || expression[i] == '/' || 
                   expression[i] == '^') { // Adăugat pentru operatorul `^`.
            while (!ops.empty() && precedence(ops.top()) >= precedence(expression[i])) {
                int b = values.top(); values.pop();
                int a = values.top(); values.pop();
                char op = ops.top(); ops.pop();
                values.push(applyOperation(a, b, op));
            }
            ops.push(expression[i]);
        }
    }

    while (!ops.empty()) {
        int b = values.top(); values.pop();
        int a = values.top(); values.pop();
        char op = ops.top(); ops.pop();
        values.push(applyOperation(a, b, op));
    }

    return values.top();
}

int main() {
	string expr = "2*(5-3)**2+6/2-2**2";
    //string expression = replaceSubstring(expr,"**","^"); // Exemplu cu operatorul `^` pentru ridicare la putere
    replaceSubstring(expr,"**","^"); 
    cout << "Rezultatul expresiei \"" << expr << "\" este: " << evaluateExpression(expr) << endl;
    return 0;
}

#include <iostream>
#include <string>


using namespace std;

//Структура узла стека
struct StackNode {
    char data;
    StackNode* next;

    StackNode(char value) : data(value), next(nullptr) {}
};

//Класс стека
class Stack {
private:
    StackNode* top;

public:
    Stack() : top(nullptr) {}

    ~Stack() {
        while (!isEmpty()) {
            pop();
        }
    }

    //Проверка, является ли стек пустым
    bool isEmpty() {
        return top == nullptr;
    }

    //Добавление элемента в стек
    void push(char value) {
        StackNode* newNode = new StackNode(value);
        newNode->next = top;
        top = newNode;
    }

    //Извлечение верхнего элемента из стека
    char pop() {
        if (isEmpty()) {
            cout << "STACK IS EMPTY" << endl;
            return '\0'; //Возвращаем значение по умолчанию в случае ошибки
        }
        StackNode* temp = top;
        char data = temp->data;
        top = top->next;
        delete temp;
        return data;
    }

    //Получение верхнего элемента стека без удаления
    char peek() {
        if (isEmpty()) {
            cout << "STACK IS EMPTY" << endl;
            return '\0'; //Возвращаем значение по умолчанию в случае ошибки
        }
        return top->data;
    }
};

//Функция, проверяющая, является ли символ оператором
bool isOperator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '%';
}

//Функция, возвращающая приоритет оператора
int getPriority(char c) {
    if (c == '+' || c == '-')
        return 1;
    if (c == '*' || c == '/' || c == '%')
        return 2;
    return 0;
}

//Функция, преобразующая инфиксное выражение в постфиксное
string infixToPostfix(string infix) {
    Stack operatorStack;
    string postfix;

    for (char c : infix) {
        if (isalnum(c)) {
            postfix += c;
        } else if (c == '(') {
            operatorStack.push(c);
        } else if (c == ')') {
            while (!operatorStack.isEmpty() && operatorStack.peek() != '(') {
                postfix += operatorStack.pop();
            }
            if (operatorStack.isEmpty()) {
                cout << "ERROR: NOT MATCHING BRACKETS" << endl;
                return "";
            }
            operatorStack.pop(); //Удаляем открывающую скобку
        } else if (isOperator(c)) {
            while (!operatorStack.isEmpty() && operatorStack.peek() != '(' &&
                   getPriority(operatorStack.peek()) >= getPriority(c)) {
                postfix += operatorStack.pop();
            }
            operatorStack.push(c);
        } else {
            cout << "ERROR: WRONG SYMBOL" << endl;
            return "";
        }
    }

    while (!operatorStack.isEmpty()) {
        if (operatorStack.peek() == '(') {
            cout << "ERROR: NOT MATCHING BRACKETS" << endl;
            return "";
        }
        postfix += operatorStack.pop();
    }

    return postfix;
}

//Функция, вычисляющая результат постфиксного выражения
double evaluatePostfix(string postfix) {
    Stack operandStack;

    for (char c : postfix) {
        if (isdigit(c)) {
            operandStack.push(c - '0');
        } else if (isOperator(c)) {
            double operand2 = operandStack.pop();
            double operand1 = operandStack.pop();

            double result;
            switch (c) {
                case '+':
                    result = operand1 + operand2;
                    break;
                case '-':
                    result = operand1 - operand2;
                    break;
                case '*':
                    result = operand1 * operand2;
                    break;
                case '/':
                    if (operand2 == 0) {
                        cout << "ERROR: DIVIDING BY ZERO" << endl;
                        return 0;
                    }
                    result = operand1 / operand2;
                    break;
				case '%':
                    if (operand2 == 0) {
                        cout << "ERROR: DIVIDING BY ZERO" << endl;
                        return 0;
                    }
                    result = operand1 / operand2;
                    break;
            }

            operandStack.push(result);
        } else {
            cout << "ERROR: WRONG SYMBOL" << endl;
            return 0;
        }
    }

    if (operandStack.isEmpty()) {
        cout << "ERROR: EPRTY EXPRESSION" << endl;
        return 0;
    }

    return operandStack.pop();
}

int main() {
    string infixExpression;
    cout << "Enter an infix expression: ";
    getline(cin, infixExpression);

    if (infixExpression.empty()) {
        cout << "ERROR: EMPTY STRING!" << endl;
        return 0;
    }

    string postfixExpression = infixToPostfix(infixExpression);

    if (postfixExpression.empty()) {
        return 0;
    }

    cout << "Postfix expression: " << postfixExpression << endl;

    double result = evaluatePostfix(postfixExpression);
    cout << "Expression result: " << result << endl;

    return 0;
}
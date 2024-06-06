#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

typedef struct Expression Expression;

typedef struct 
{
    char *message;
} CalculatorError;

typedef double (*EvaluateFunction)(Expression *, double *, CalculatorError *);

struct Expression 
{
    EvaluateFunction evaluate;
    void *data;
};

typedef struct 
{
    double value;
} Number;

typedef struct 
{
    char *name;
} Variable;

typedef struct 
{
    Expression *left;
    Expression *right;
    char operator;
} BinaryOperation;

typedef struct 
{
    Variable *variable;
    Expression *expression;
} Assignment;

typedef struct 
{
    char *function;
    Expression *argument;
} Function;

void setError(CalculatorError *error, const char *message) 
{
    if (error != NULL && error->message == NULL) 
    {
        error->message = strdup(message);
    }
}

double evaluateNumber(Expression *expr, double *variables, CalculatorError *error) 
{
    Number *number = (Number *)expr->data;
    return number->value;
}

double evaluateVariable(Expression *expr, double *variables, CalculatorError *error) 
{
    Variable *variable = (Variable *)expr->data;
    if (variables[variable->name[0] - 'a'] != -1) 
    {
        return variables[variable->name[0] - 'a'];
    } 
    else 
    {
        setError(error, "Undefined variable");
        return 0;
    }
}

double evaluateBinaryOperation(Expression *expr, double *variables, CalculatorError *error) 
{
    BinaryOperation *operation = (BinaryOperation *)expr->data;

    double leftValue = operation->left->evaluate(operation->left, variables, error);
    if (error->message != NULL) 
    {
        return 0;
    }

    double rightValue = operation->right->evaluate(operation->right, variables, error);
    if (error->message != NULL) 
    {
        return 0;
    }

    switch (operation->operator) 
    {
        case '+':
            return leftValue + rightValue;
        case '-':
            return leftValue - rightValue;
        case '*':
            return leftValue * rightValue;
        case '/':
            if (rightValue == 0) 
            {
                setError(error, "Division by zero");
                return 0;
            }
            return leftValue / rightValue;
        default:
            setError(error, "Unknown operator");
            return 0;
    }
}

double evaluateAssignment(Expression *expr, double *variables, CalculatorError *error) 
{
    Assignment *assignment = (Assignment *)expr->data;
    double result = assignment->expression->evaluate(assignment->expression, variables, error);
    if (error->message != NULL) 
    {
        return 0;
    }
    variables[assignment->variable->name[0] - 'a'] = result;
    return result;
}

double evaluateFunction(Expression *expr, double *variables, CalculatorError *error) {
    Function *func = (Function *)expr->data;
    double argValue = func->argument->evaluate(func->argument, variables, error);
    if (error->message != NULL) 
    {
        return 0;
    }

    if (strcmp(func->function, "pow") == 0) 
    {
        return pow(argValue, 2);
    } 
    else if (strcmp(func->function, "log") == 0) 
    {
        return log(argValue);
    } 
    else if (strcmp(func->function, "sin") == 0) 
    {
        return sin(argValue);
    } 
    else if (strcmp(func->function, "sqrt") == 0) 
    {
        return sqrt(argValue);
    } 
    else if (strcmp(func->function, "abs") == 0) 
    {
        return fabs(argValue);
    } 
    else 
    {
        setError(error, "Unknown function");
        return 0;
    }
}

Expression *createNumber(double value) 
{
    Expression *expr = malloc(sizeof(Expression));
    Number *number = malloc(sizeof(Number));
    number->value = value;
    expr->evaluate = evaluateNumber;
    expr->data = number;
    return expr;
}

Expression *createVariable(char *name) 
{
    Expression *expr = malloc(sizeof(Expression));
    Variable *variable = malloc(sizeof(Variable));
    variable->name = name;
    expr->evaluate = evaluateVariable;
    expr->data = variable;
    return expr;
}

Expression *createBinaryOperation(Expression *left, Expression *right, char operator) 
{
    Expression *expr = malloc(sizeof(Expression));
    BinaryOperation *operation = malloc(sizeof(BinaryOperation));
    operation->left = left;
    operation->right = right;
    operation->operator = operator;
    expr->evaluate = evaluateBinaryOperation;
    expr->data = operation;
    return expr;
}

Expression *createAssignment(Variable *variable, Expression *expression) 
{
    Expression *expr = malloc(sizeof(Expression));
    Assignment *assignment = malloc(sizeof(Assignment));
    assignment->variable = variable;
    assignment->expression = expression;
    expr->evaluate = evaluateAssignment;
    expr->data = assignment;
    return expr;
}

Expression *createFunction(char *function, Expression *argument) 
{
    Expression *expr = malloc(sizeof(Expression));
    Function *func = malloc(sizeof(Function));
    func->function = function;
    func->argument = argument;
    expr->evaluate = evaluateFunction;
    expr->data = func;
    return expr;
}

char *strndup(const char *str, size_t n) 
{
    char *result;
    size_t len = strlen(str);
    if (n < len) 
    {
        len = n;
    }
    result = (char *)malloc(len + 1);
    if (!result) 
    {
        return NULL;
    }
    result[len] = '\0';
    return (char *)memcpy(result, str, len);
}

Expression *parseExpression(const char *expression, double *variables);

Expression *parse(const char *expression, double *variables) 
{
    char *trimmed = strdup(expression);
    while (isspace(*trimmed)) 
    {
        trimmed++;
    }
    char *end = trimmed + strlen(trimmed) - 1;
    while (end > trimmed && isspace(*end)) 
    {
        end--;
    }
    *(end + 1) = '\0';

    char *equalSign = strchr(trimmed, '=');
    if (equalSign) 
    {
        char *var_name = strndup(trimmed, equalSign - trimmed);
        char *expr = equalSign + 1;
        Variable *var = malloc(sizeof(Variable));
        var->name = var_name;
        return createAssignment(var, parseExpression(expr, variables));
    }

    char *openParen = strchr(trimmed, '(');
    char *closeParen = strchr(trimmed, ')');
    if (openParen && closeParen && openParen < closeParen) 
    {
        char *func_name = strndup(trimmed, openParen - trimmed);
        char *arg_expr = strndup(openParen + 1, closeParen - openParen - 1);
        return createFunction(func_name, parseExpression(arg_expr, variables));
    }

    for (char *op = "+-*/"; *op; op++) 
    {
        char *operatorPos = strchr(trimmed, *op);
        if (operatorPos) 
        {
            char *left = strndup(trimmed, operatorPos - trimmed);
            char *right = operatorPos + 1;
            return createBinaryOperation(parseExpression(left, variables), parseExpression(right, variables), *op);
        }
    }

    char *endPtr;
    double value = strtod(trimmed, &endPtr);
    if (*endPtr == '\0') 
    {
        return createNumber(value);
    } 
    else 
    {
        return createVariable(trimmed);
    }
}

Expression *parseExpression(const char *expression, double *variables) 
{
    return parse(expression, variables);
}

double evaluateExpression(Expression *expression, double *variables, CalculatorError *error) 
{
    return expression->evaluate(expression, variables, error);
}

void run(const char *expressions, double *variables) 
{
    char *exprs = strdup(expressions);
    char *token = strtok(exprs, ";");

    while (token) 
    {
        CalculatorError error = {NULL};
        double result = evaluateExpression(parseExpression(token, variables), variables, &error);
        if (error.message != NULL) 
        {
            printf("%s\n", error.message);
            free(error.message);
        } 
        else 
        {
            printf("%lf\n", result);
        }
        token = strtok(NULL, ";");
    }
    free(exprs);
}

int main() {
    double variables[26];
    for (int i = 0; i < 26; i++) 
    {
        variables[i] = -1;
    }

    char expressions[256];
    printf("Enter the expressions (separated by ';'): ");
    fgets(expressions, sizeof(expressions), stdin);
    printf("Input: %s\n", expressions);
    run(expressions, variables);

    return 0;
}

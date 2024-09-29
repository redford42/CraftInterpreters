#include <stdio.h>
#include <string.h>


typedef enum
{
    // Single-character tokens.
    TOKEN_LEFT_PAREN,
    TOKEN_RIGHT_PAREN,
    TOKEN_LEFT_BRACE,
    TOKEN_RIGHT_BRACE,
    TOKEN_COMMA,
    TOKEN_DOT,
    TOKEN_MINUS,
    TOKEN_PLUS,
    TOKEN_SEMICOLON,
    TOKEN_SLASH,
    TOKEN_STAR,
    // One or two character tokens.
    TOKEN_BANG,
    TOKEN_BANG_EQUAL,
    TOKEN_EQUAL,
    TOKEN_EQUAL_EQUAL,
    TOKEN_GREATER,
    TOKEN_GREATER_EQUAL,
    TOKEN_LESS,
    TOKEN_LESS_EQUAL,
    // Literals.
    TOKEN_IDENTIFIER,
    TOKEN_STRING,
    TOKEN_NUMBER,
    // Keywords.
    TOKEN_AND,
    TOKEN_CLASS,
    TOKEN_ELSE,
    TOKEN_FALSE,
    TOKEN_FOR,
    TOKEN_FUN,
    TOKEN_IF,
    TOKEN_NIL,
    TOKEN_OR,
    TOKEN_PRINT,
    TOKEN_RETURN,
    TOKEN_SUPER,
    TOKEN_THIS,
    TOKEN_TRUE,
    TOKEN_VAR,
    TOKEN_WHILE,

    TOKEN_ERROR,
    TOKEN_EOF
} TokenType;

typedef struct
{
    TokenType type;
    const char *start;
    int length;
    int line;
} Token;

// 噫，搞个链表存Token，噫
typedef struct
{
    Token current;
    Token previous;
    //> had-error-field
    bool hadError;
    //< had-error-field
} Parser;

typedef struct
{
    const char *start; // 指向本次token扫描的开头，来确定常量时该从哪里截断
    const char *current;
    int line;
} Scanner;

Scanner scanner;
//> init-scanner
void initScanner(const char *source)
{
    scanner.start = source;
    scanner.current = source;
    scanner.line = 1;
}

//> advance
static char advance()
{
    scanner.current++;
    return scanner.current[-1];
}
//< advance
//> peek
static char peek()
{
    return *scanner.current;
}

static bool isAtEnd()
{
    return *scanner.current == '\0';
}
//< peek
//> peek-next
static char peekNext()
{
    if (isAtEnd())
        return '\0';
    return scanner.current[1];
}

static Token makeToken(TokenType type)
{
    Token token;
    token.type = type;
    token.start = scanner.start;
    token.length = (int)(scanner.current - scanner.start);
    token.line = scanner.line;
    return token;
}

static bool match(char expected)
{
    if (isAtEnd())
        return false;
    if (*scanner.current != expected)
        return false;
    scanner.current++;
    return true;
}

//> 字符判断
static bool isAlpha(char c)
{
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
           c == '_';
}

//> 数字判断
static bool isDigit(char c)
{
    return c >= '0' && c <= '9';
}

static TokenType checkKeyword(int start, int length,
                              const char *rest, TokenType type)
{
    if (scanner.current - scanner.start == start + length &&
        memcmp(scanner.start + start, rest, length) == 0)
    {
        return type;
    }

    return TOKEN_IDENTIFIER;
}

static TokenType identifierType()
{
    //> keywords
    switch (scanner.start[0])
    {
    case 'a':
        return checkKeyword(1, 2, "nd", TOKEN_AND);
    case 'c':
        return checkKeyword(1, 4, "lass", TOKEN_CLASS);
    case 'e':
        return checkKeyword(1, 3, "lse", TOKEN_ELSE);
        //> keyword-f
    case 'f':
        if (scanner.current - scanner.start > 1)
        {
            switch (scanner.start[1])
            {
            case 'a':
                return checkKeyword(2, 3, "lse", TOKEN_FALSE);
            case 'o':
                return checkKeyword(2, 1, "r", TOKEN_FOR);
            case 'u':
                return checkKeyword(2, 1, "n", TOKEN_FUN);
            }
        }
        break;
        //< keyword-f
    case 'i':
        return checkKeyword(1, 1, "f", TOKEN_IF);
    case 'n':
        return checkKeyword(1, 2, "il", TOKEN_NIL);
    case 'o':
        return checkKeyword(1, 1, "r", TOKEN_OR);
    case 'p':
        return checkKeyword(1, 4, "rint", TOKEN_PRINT);
    case 'r':
        return checkKeyword(1, 5, "eturn", TOKEN_RETURN);
    case 's':
        return checkKeyword(1, 4, "uper", TOKEN_SUPER);
        //> keyword-t
    case 't':
        if (scanner.current - scanner.start > 1)
        {
            switch (scanner.start[1])
            {
            case 'h':
                return checkKeyword(2, 2, "is", TOKEN_THIS);
            case 'r':
                return checkKeyword(2, 2, "ue", TOKEN_TRUE);
            }
        }
        break;
        //< keyword-t
    case 'v':
        return checkKeyword(1, 2, "ar", TOKEN_VAR);
    case 'w':
        return checkKeyword(1, 4, "hile", TOKEN_WHILE);
    }

    //< keywords
    return TOKEN_IDENTIFIER;
}
static Token identifier()
{
    while (isAlpha(peek()) || isDigit(peek()))
        advance();
    return makeToken(identifierType());
}

static Token number()
{
    while (isDigit(peek()))
        advance();

    // Look for a fractional part.
    if (peek() == '.' && isDigit(peekNext()))
    {
        // Consume the ".".
        advance();

        while (isDigit(peek()))
            advance();
    }

    return makeToken(TOKEN_NUMBER);
}

static void skipWhitespace()
{
    for (;;)
    {
        char c = peek();
        switch (c)
        {
        case ' ':
        case '\r':
        case '\t':
            advance();
            break;
            //> newline
        case '\n':
            scanner.line++;
            advance();
            break;
            //< newline
            //> comment
        case '/':
            if (peekNext() == '/')
            {
                // A comment goes until the end of the line.
                while (peek() != '\n' && !isAtEnd())
                    advance();
            }
            else
            {
                return;
            }
            break;
            //< comment
        default:
            return;
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc == 2)
    {
        printf("goes on files %s\n", argv[1]);
        // =======================读取文件
        char *path = argv[1];
        FILE *file = fopen(path, "rb");
        if (file == NULL)
        {
            fprintf(stderr, "Could not open file \"%s\".\n", path);
            exit(74);
        }
        fseek(file, 0L, SEEK_END);
        size_t fileSize = ftell(file);
        rewind(file);

        char *buffer = (char *)malloc(fileSize + 1);
        if (buffer == NULL)
        {
            fprintf(stderr, "Not enough memory to read \"%s\".\n", path);
            exit(74);
        }
        //< no-buffer
        size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
        //> no-read
        if (bytesRead < fileSize)
        {
            fprintf(stderr, "Could not read file \"%s\".\n", path);
            exit(74);
        }

        //< no-read
        buffer[bytesRead] = '\0';

        fclose(file);
        // =======================读取文件

        initScanner(buffer);
        fprintf(stderr, "buffer \"%s\".\n", buffer);

        Token tokenArray[bytesRead];
        // 完成文件字节读取，开始分析
        int interpret_pointer = 0;
        // Parser parser;
        while (!isAtEnd())
        {
            skipWhitespace();
            char current = advance(); // 获取当前的字符

            if (isAlpha(current))
            {
                tokenArray[interpret_pointer] = identifier();
            }
            else if (isDigit(current))
            {
                tokenArray[interpret_pointer] = number();
            }
            else
            {
                switch (current)
                {
                case '(':
                    tokenArray[interpret_pointer] = makeToken(TOKEN_LEFT_PAREN);
                case ')':
                    tokenArray[interpret_pointer] = makeToken(TOKEN_RIGHT_PAREN);
                case '{':
                    tokenArray[interpret_pointer] = makeToken(TOKEN_LEFT_BRACE);
                case '}':
                    tokenArray[interpret_pointer] = makeToken(TOKEN_RIGHT_BRACE);
                case ';':
                    tokenArray[interpret_pointer] = makeToken(TOKEN_SEMICOLON);
                case ',':
                    tokenArray[interpret_pointer] = makeToken(TOKEN_COMMA);
                case '.':
                    tokenArray[interpret_pointer] = makeToken(TOKEN_DOT);
                case '-':
                    tokenArray[interpret_pointer] = makeToken(TOKEN_MINUS);
                case '+':
                    tokenArray[interpret_pointer] = makeToken(TOKEN_PLUS);
                case '/':
                    tokenArray[interpret_pointer] = makeToken(TOKEN_SLASH);
                case '*':
                    tokenArray[interpret_pointer] = makeToken(TOKEN_STAR);
                    //> two-char
                case '!':
                    tokenArray[interpret_pointer] = makeToken(
                        match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
                case '=':
                    tokenArray[interpret_pointer] = makeToken(
                        match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
                case '<':
                    tokenArray[interpret_pointer] = makeToken(
                        match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
                case '>':
                    tokenArray[interpret_pointer] = makeToken(
                        match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
                    //< two-char
                    //> scan-string
                case '"':
                    // tokenArray[interpret_pointer] = string();
                    //< scan-string
                }
            }
            interpret_pointer++;
        }
        fprintf(stderr, "bytesRead \"%d\".\n", bytesRead);

        for (int i = 0; i < bytesRead; i++)
        {
            fprintf(stderr, "zzzzzz \"%d\".\n", tokenArray[i].type);
        }
    }
    else if (argc > 2)
    {
        printf("Too many arguments supplied.\n");
        // maybe runPrompt
    }
    else
    {
        printf("One argument expected.\n");
    }
    system("pause");
}
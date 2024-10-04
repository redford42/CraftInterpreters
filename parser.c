#include <stdio.h>
//> Compiling Expressions compiler-include-stdlib
#include <stdlib.h>
//< Compiling Expressions compiler-include-stdlib
//> Local Variables compiler-include-string
#include <string.h>
//< Local Variables compiler-include-string

#include "common.h"
#include "scanner.h"
#include "parser.h"
typedef struct
{
    Token current;
    Token previous;
    bool hadError;
    int line;
} Parser;
Parser parser;
int tokenCounter;

Token *tokenArray;

void initParser(const Token *source)
{
    tokenArray = source;
    tokenCounter = 0;
}

void parserAllToken()
{
    parser.hadError = false;
    parser.panicMode = false;
    advance();
    while (!match(TOKEN_EOF))
    {
        declaration();
    }
}

static void declaration()
{
  //> Classes and Instances match-class
  if (match(TOKEN_CLASS))
  {
    // classDeclaration();
  }
  else if (match(TOKEN_FUN))
  {
    // funDeclaration();
  }
  else if (match(TOKEN_VAR))
  {
    // varDeclaration();
  }
  else
  {
    statement();
  }


//   if (parser.panicMode)
//     synchronize();
  
}

static void statement()
{ 
    if (match(TOKEN_LEFT_BRACE))
  {
    block();
  }
  else
  {
    expressionStatement();
  }
}

static void expressionStatement()
{
  expression();
  consume(TOKEN_SEMICOLON, "Expect ';' after expression.");
}

static void expression()
{
  /* Compiling Expressions expression < Compiling Expressions expression-body
    // What goes here?
  */
  //> expression-body
  parsePrecedence(PREC_ASSIGNMENT);
  //< expression-body
}

static Token advance()
{
    parser.previous = parser.current;
    parser.current = tokenArray[tokenCounter++];
    return parser.current;
}

static void consume(TokenType type, const char *message)
{
    if (parser.current.type == type)
    {
        advance();
        return;
    }

    errorAtCurrent(message);
}

static bool check(TokenType type)
{
    return parser.current.type == type;
}

static bool match(TokenType type)
{
    if (!check(type))
        return false;
    advance();
    return true;
}

static void emitByte(uint8_t byte) {
  writeChunk(currentChunk(), byte, parser.previous.line);
}
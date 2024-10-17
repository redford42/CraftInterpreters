// #include <stdio.h>
// //> Compiling Expressions compiler-include-stdlib
// #include <stdlib.h>
// //< Compiling Expressions compiler-include-stdlib
// //> Local Variables compiler-include-string
// #include <string.h>
// //< Local Variables compiler-include-string

// #include "common.h"
// #include "scanner.h"
// #include "parser.h"
// #include "chunk.h"
// #include "value.h"
// #include "debug.h"
// #include "vm.h"

// typedef struct
// {
//   Token current;
//   Token previous;
//   bool hadError;
//   int line;
//   bool panicMode;
// } Parser;

// // 运算优先级
// typedef enum
// {
//   PREC_NONE,
//   PREC_ASSIGNMENT, // =
//   PREC_OR,         // or
//   PREC_AND,        // and
//   PREC_EQUALITY,   // == !=
//   PREC_COMPARISON, // < > <= >=
//   PREC_TERM,       // + -
//   PREC_FACTOR,     // * /
//   PREC_UNARY,      // ! -
//   PREC_CALL,       // . ()
//   PREC_PRIMARY
// } Precedence;

// typedef void (*ParseFn)();

// typedef struct
// {
//   ParseFn prefix;
//   ParseFn infix;
//   Precedence precedence;
// } ParseRule;

// Parser parser;
// int tokenCounter = 0;

// Token *tokenArray;

// Chunk chunk;

// static void expression();
// static ParseRule *getRule(TokenType type);
// static void parsePrecedence(Precedence precedence);

// Chunk *compilingChunk;

// static Chunk *currentChunk()
// {
//   return compilingChunk;
// }

// static void emitByte(uint8_t byte)
// {
//   writeChunk(currentChunk(), byte, parser.previous.line);
// }

// static void emitBytes(uint8_t byte1, uint8_t byte2)
// {
//   emitByte(byte1);
//   emitByte(byte2);
// }

// static void emitReturn()
// {
//   emitByte(OP_RETURN);
// }

// static void errorAt(Token *token, const char *message)
// {
//   //> check-panic-mode
//   if (parser.panicMode)
//     return;
//   //< check-panic-mode
//   //> set-panic-mode
//   parser.panicMode = true;
//   //< set-panic-mode
//   fprintf(stderr, "[line %d] Error", token->line);

//   if (token->type == TOKEN_EOF)
//   {
//     fprintf(stderr, " at end");
//   }
//   else if (token->type == TOKEN_ERROR)
//   {
//     // Nothing.
//   }
//   else
//   {
//     fprintf(stderr, " at '%.*s'", token->length, token->start);
//   }

//   fprintf(stderr, ": %s\n", message);
//   parser.hadError = true;
// }

// static void error(const char *message)
// {
//   errorAt(&parser.previous, message);
// }
// static uint8_t makeConstant(Value value)
// {
//   int constant = addConstant(currentChunk(), value);
//   if (constant > UINT8_MAX)
//   {
//     error("Too many constants in one chunk.");
//     return 0;
//   }

//   return (uint8_t)constant;
// }

// static void emitConstant(Value value)
// {
//   emitBytes(OP_CONSTANT, makeConstant(value));
// }

// static void number()
// {
//   double value = strtod(parser.previous.start, NULL);
//   emitConstant(value);
// }

// static void errorAtCurrent(const char *message)
// {
//   errorAt(&parser.current, message);
// }

// static Token advance()
// {
//   while (parser.current.type != TOKEN_EOF)
//   {
//     fprintf(stderr, "advance tokenCounter %d \n", tokenCounter);
//     parser.previous = parser.current;
//     fprintf(stderr, "advance tokenCounter111 %d \n", tokenArray[tokenCounter]);
//     parser.current = tokenArray[tokenCounter++];
//     return parser.current;
//   }
//   fprintf(stderr, "advance tokenCounter outside %d \n", tokenCounter);
//   return parser.current;
// }

// static void consume(TokenType type, const char *message)
// {
//   if (parser.current.type == type)
//   {
//     advance();
//     return;
//   }

//   errorAtCurrent(message);
// }

// static void grouping()
// {
//   expression();
//   consume(TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
// }

// static void unary()
// {
//   TokenType operatorType = parser.previous.type;

//   // Compile the operand.
//   parsePrecedence(PREC_UNARY);
//   expression();

//   // Emit the operator instruction.
//   switch (operatorType)
//   {
//   case TOKEN_MINUS:
//     emitByte(OP_NEGATE);
//     break;
//   default:
//     return; // Unreachable.
//   }
// }

// static void binary()
// {
//   TokenType operatorType = parser.previous.type;
//   ParseRule *rule = getRule(operatorType);
//   parsePrecedence((Precedence)(rule->precedence + 1));

//   switch (operatorType)
//   {
//   case TOKEN_PLUS:
//     emitByte(OP_ADD);
//     break;
//   case TOKEN_MINUS:
//     emitByte(OP_SUBTRACT);
//     break;
//   case TOKEN_STAR:
//     emitByte(OP_MULTIPLY);
//     break;
//   case TOKEN_SLASH:
//     emitByte(OP_DIVIDE);
//     break;
//   default:
//     return; // Unreachable.
//   }
// }

// static void parsePrecedence(Precedence precedence)
// {
//   advance();
//   ParseFn prefixRule = getRule(parser.previous.type)->prefix;
//   if (prefixRule == NULL)
//   {
//     fprintf(stderr, "prefixRule == NULL \n");
//     error("Expect expression.");
//     return;
//   }

//   prefixRule();

//   while (precedence <= getRule(parser.current.type)->precedence)
//   {
//     advance();
//     ParseFn infixRule = getRule(parser.previous.type)->infix;
//     infixRule();
//   }
//   // fprintf(stderr, "disassembleChunk!\n");
//   // disassembleChunk(&chunk, "test chunk");
// }

// ParseRule rules[] = {
//     [TOKEN_LEFT_PAREN] = {grouping, NULL, PREC_NONE},
//     [TOKEN_RIGHT_PAREN] = {NULL, NULL, PREC_NONE},
//     [TOKEN_LEFT_BRACE] = {NULL, NULL, PREC_NONE},
//     [TOKEN_RIGHT_BRACE] = {NULL, NULL, PREC_NONE},
//     [TOKEN_COMMA] = {NULL, NULL, PREC_NONE},
//     [TOKEN_DOT] = {NULL, NULL, PREC_NONE},
//     [TOKEN_MINUS] = {unary, binary, PREC_TERM},
//     [TOKEN_PLUS] = {NULL, binary, PREC_TERM},
//     [TOKEN_SEMICOLON] = {NULL, NULL, PREC_NONE},
//     [TOKEN_SLASH] = {NULL, binary, PREC_FACTOR},
//     [TOKEN_STAR] = {NULL, binary, PREC_FACTOR},
//     [TOKEN_BANG] = {NULL, NULL, PREC_NONE},
//     [TOKEN_BANG_EQUAL] = {NULL, NULL, PREC_NONE},
//     [TOKEN_EQUAL] = {NULL, NULL, PREC_NONE},
//     [TOKEN_EQUAL_EQUAL] = {NULL, NULL, PREC_NONE},
//     [TOKEN_GREATER] = {NULL, NULL, PREC_NONE},
//     [TOKEN_GREATER_EQUAL] = {NULL, NULL, PREC_NONE},
//     [TOKEN_LESS] = {NULL, NULL, PREC_NONE},
//     [TOKEN_LESS_EQUAL] = {NULL, NULL, PREC_NONE},
//     [TOKEN_IDENTIFIER] = {NULL, NULL, PREC_NONE},
//     [TOKEN_STRING] = {NULL, NULL, PREC_NONE},
//     [TOKEN_NUMBER] = {number, NULL, PREC_NONE},
//     [TOKEN_AND] = {NULL, NULL, PREC_NONE},
//     [TOKEN_CLASS] = {NULL, NULL, PREC_NONE},
//     [TOKEN_ELSE] = {NULL, NULL, PREC_NONE},
//     [TOKEN_FALSE] = {NULL, NULL, PREC_NONE},
//     [TOKEN_FOR] = {NULL, NULL, PREC_NONE},
//     [TOKEN_FUN] = {NULL, NULL, PREC_NONE},
//     [TOKEN_IF] = {NULL, NULL, PREC_NONE},
//     [TOKEN_NIL] = {NULL, NULL, PREC_NONE},
//     [TOKEN_OR] = {NULL, NULL, PREC_NONE},
//     [TOKEN_PRINT] = {NULL, NULL, PREC_NONE},
//     [TOKEN_RETURN] = {NULL, NULL, PREC_NONE},
//     [TOKEN_SUPER] = {NULL, NULL, PREC_NONE},
//     [TOKEN_THIS] = {NULL, NULL, PREC_NONE},
//     [TOKEN_TRUE] = {NULL, NULL, PREC_NONE},
//     [TOKEN_VAR] = {NULL, NULL, PREC_NONE},
//     [TOKEN_WHILE] = {NULL, NULL, PREC_NONE},
//     [TOKEN_ERROR] = {NULL, NULL, PREC_NONE},
//     [TOKEN_EOF] = {NULL, NULL, PREC_NONE},
// };
// static ParseRule *getRule(TokenType type)
// {
//   return &rules[type];
// }

// static void expressionStatement()
// {
//   fprintf(stderr, "expressionStatement!\n");
//   expression();
//   consume(TOKEN_SEMICOLON, "Expect ';' after expression.");
// }

// static void expression()
// {
//   parsePrecedence(PREC_ASSIGNMENT);
// }

// //< Compiling Expressions error
// //> Compiling Expressions error-at-current

// static bool check(TokenType type)
// {
//   return parser.current.type == type;
// }

// static bool match(TokenType type)
// {
//   if (!check(type))
//     return false;
//   advance();
//   return true;
// }
// static void statement()
// {
//   fprintf(stderr, "statement\n");
//   if (match(TOKEN_LEFT_BRACE))
//   {
//     // block();
//   }
//   else
//   {
//     expressionStatement();
//   }
// }

// static void declaration()
// {
//   //> Classes and Instances match-class
//   if (match(TOKEN_CLASS))
//   {
//     // classDeclaration();
//   }
//   else if (match(TOKEN_FUN))
//   {
//     // funDeclaration();
//   }
//   else if (match(TOKEN_VAR))
//   {
//     // varDeclaration();
//   }
//   else
//   {
//     statement();
//   }

//   //   if (parser.panicMode)
//   //     synchronize();
// }

// void initParser(const Token *source)
// {
//   tokenArray = source;
//   tokenCounter = 0;
// }

// void parserAllToken()
// {
  
//   initChunk(&chunk);
//   compilingChunk = &chunk;
//   parser.hadError = false;
//   parser.panicMode = false;
//   fprintf(stderr, "advance!\n");
//   advance();
//   fprintf(stderr, "advance over\n");
//   while (!match(TOKEN_EOF))
//   {
//     declaration();
//   }

//   fprintf(stderr, "disassembleChunk!\n");
//   disassembleChunk(&chunk, "test chunk");

//   // interpret(&chunk);

//   freeVM();
//   freeChunk(&chunk);
// }

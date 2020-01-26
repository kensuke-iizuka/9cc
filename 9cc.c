#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Token kind
typedef enum {
  TK_RESERVED,
  TK_NUM,
  TK_EOF,
} TokenKind;

typedef struct Token Token;

// Token type
struct Token {
  TokenKind kind;
  Token *next;
  int val;
  char *str;
};
Token *token;

// Assert error
void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

// if next token is expected op, go to next token
// and return true, or not, return false
bool consume(char op) {
  if(token->kind != TK_RESERVED || token->str[0] != op)
    return false;
  token = token->next;
  return true;
}

// if next toke is expected op, go to next token
// or not, assert error
void expect(char op) {
  if(token->kind != TK_RESERVED || token->str[0] != op)
    error("Token is not %c", op);
  token = token->next;
}

// if next toke is number, go to next token
// and return this value or not, assert error
int expect_number() {
  if(token->kind != TK_NUM)
    error("Token is not number");
  int val = token->val;
  token = token->next;
  return val;
}

bool at_eof() {
  return token->kind == TK_EOF;
}

// Create new tokne and connect cur
Token *new_token(TokenKind kind, Token *cur, char *str) {
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  cur->next = tok;
  return tok;
}

// Tokenize input string and return tokenized
Token *tokenize(char *p) {
  Token head;
  head.next = NULL;
  Token *cur = &head;

  while(*p) {
    // Skip space　char
    if (isspace(*p)) {
      p++;
      continue;
    }

    if (*p == '+' || *p == '-') {
      cur = new_token(TK_RESERVED, cur, p++);
      continue;
    }

    if (isdigit(*p)) {
      cur = new_token(TK_NUM, cur, p);
      cur->val = strtol(p, &p, 10);
      continue;
    }

    error("Cannot tokenize");
  }

  new_token(TK_EOF, cur, p);
  return head.next;
}


int main(int argc, char **argv)
{
  if (argc != 2) {
    fprintf(stderr, "Wrong number of arguments\n");
    return 1;
  }

  token = tokenize(argv[1]);

  // Output the former of assembly
  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  // Check whether the first token in expression is number
  // and if then, output mov operation
  printf("  mov rax, %d\n", expect_number());

  // Consume tokens like `+ <number>` or `- <number>` and
  // output assembly
  while (!at_eof()) {
    if (consume('+')) {
      printf("  add rax, %d\n", expect_number());
      continue;
    }

    expect('-');
    printf("  sub rax, %d\n", expect_number());
  }

  printf("  ret\n");
  return 0;
}

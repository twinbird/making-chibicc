#include "chibicc.h"

int main(int argc, char **argv) {
  if (argc != 2)
    error("%s: invalid number of arguments", argv[0]);

  // Tokenize and parse
  Token *tok = tokenize(argv[1]);
  Node *node = parse(tok);
  codegen(node);

  return 0;
}

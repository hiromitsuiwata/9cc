#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "9cc.h"

// 新しくノードを作る。左辺右辺には子ノードを付けない
Node *new_node(NodeKind kind) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  return node;
}

// 新しくノードを作る。左辺右辺には子ノードを付ける
Node *new_binary(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = new_node(kind);
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

// 数値だけのノードを作る。
Node *new_node_num(int val) {
  Node *node = new_node(ND_NUM);
  node->val = val;
  return node;
}

// 式
// expr = equality
Node *expr() { return equality(); }

// 等式
// equality   = relational ("==" relational | "!=" relational)*
Node *equality() {
  Node *node = relational();

  for (;;) {
    if (consume("=="))
      node = new_binary(ND_EQ, node, relational());
    else if (consume("!="))
      node = new_binary(ND_NE, node, relational());
    else
      return node;
  }
}

// 不等式
// relational = add ("<" add | "<=" add | ">" add | ">=" add)*
Node *relational() {
  Node *node = add();

  for (;;) {
    if (consume("<"))
      node = new_binary(ND_LT, node, add());
    else if (consume("<="))
      node = new_binary(ND_LE, node, add());
    if (consume(">"))
      node = new_binary(ND_LT, add(), node);  // 左辺右辺を逆にする
    else if (consume(">="))
      node = new_binary(ND_LE, add(), node);  // 左辺右辺を逆にする
    else
      return node;
  }
}

// 和と差
// add = mul ("+" mul | "-" mul)*
Node *add() {
  Node *node = mul();

  for (;;) {
    if (consume("+"))
      node = new_binary(ND_ADD, node, mul());
    else if (consume("-"))
      node = new_binary(ND_SUB, node, mul());
    else
      return node;
  }
}

// 積と商
// mul = unary ("*" unary | "/" unary)*
Node *mul() {
  Node *node = unary();

  for (;;) {
    if (consume("*")) {
      node = new_binary(ND_MUL, node, unary());
    } else if (consume("/")) {
      node = new_binary(ND_DIV, node, unary());
    } else {
      return node;
    }
  }
}

// 符号付きの数
// unary = ("+" | "-")? unary | primary
Node *unary() {
  if (consume("+")) return unary();
  if (consume("-")) return new_binary(ND_SUB, new_node_num(0), unary());
  return primary();
}

// 数または括弧で囲まれた式
// primary = "(" expr ")" | num
Node *primary() {
  if (consume("(")) {
    Node *node = expr();
    expect(")");
    return node;
  }

  return new_node_num(expect_number());
}

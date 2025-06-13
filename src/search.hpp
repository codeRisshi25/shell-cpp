#pragma once
#include <string>
#include <unordered_map>
#include <vector>

struct SearchNode {
  bool isEndOfCmd;
  std::unordered_map<char, SearchNode *> children;

  SearchNode() : isEndOfCmd(false) {};
};

extern SearchNode* globalCommandTrie;

void insertSearchNode(SearchNode *root, const std::string &word);
std::vector<std::string> getAuthCompleteSuggestions(SearchNode *root,
                                                    const std::string &prefix);
void collectAllCmds(SearchNode *node, const std::string currentWord,
                    std::vector<std::string> &suggestions);

void initializeBuiltInTrie();
void cleanupCommandTrie();
void cleanupSearchNodeRecursive(SearchNode* node); 
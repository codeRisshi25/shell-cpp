#include <string>
#include <unordered_map>
#include <vector>

struct SearchNode {
  bool isEndOfCmd;
  std::unordered_map<char, SearchNode *> children;

  SearchNode() : isEndOfCmd(false) {};
};

void insertSearchNode(SearchNode *root, const std::string &word) {
  SearchNode *node = root;
  for (char ch : word) {
    if (node->children.find(ch) == node->children.end()) {
      node->children[ch] = new SearchNode();
    }
    node = node->children[ch];
  }
  node->isEndOfCmd = true;
}

std::vector<std::string> getAuthCompleteSuggestions(SearchNode *root,
                                                    const std::string &prefix) {
  SearchNode *node = root;

  // Navigating to the prefix node
  for (char ch : prefix) {
    if (node->children.find(ch) == node->children.end())
      return {};
    node = node->children[ch];
  }

  // Collecting all completions for that node
  std::vector<std::string> suggestions;
  collectAllCmds(node, prefix, suggestions);
  return suggestions;
}

void collectAllCmds(SearchNode *node, const std::string currentWord,
                    std::vector<std::string> &suggestions) {
  if (node->isEndOfCmd) {
    suggestions.push_back(currentWord);
  }
  for (const auto& pair : node->children) {
    char ch = pair.first;
    SearchNode* childNode = pair.second;
    collectAllCmds(childNode, currentWord+ch, suggestions);
  }
}
#include <iostream>

#include "graph.hpp"

int main() {
  sta::Graph* graph = new sta::Graph();
  graph->read("../sdc/adder.fakesdc", "../lib/tsmc65.fakelib",
              "../verilog/new.fakev");
  graph->init();
  sta::Graph::PinMap* map = graph->pin_map();
  for (const auto& it : *map) {
    std::cout << it.first << " --- " << it.second->arrival() << ", "
              << it.second->require() << ", " << it.second->slack() << "\n";
  }
  delete graph;
  return 0;
}
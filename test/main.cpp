#include <iostream>

#include "graph.hpp"

int main() {
  sta::Graph* graph = new sta::Graph();
  graph->read("../sdc/adder.fakesdc", "../lib/tsmc65.fakelib", "../verilog/adder.fakev");
  graph->init();
  delete graph;
  return 0;
}
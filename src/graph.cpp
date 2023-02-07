#include "graph.hpp"

#include <fstream>
#include <iostream>

namespace sta {

bool Graph::checkPre(const Pins& is_pin, const Pin* p) const {
  for (Edge* e : *(p->pres())) {
    if (!is_pin.count(e->to_)) return false;
  }
  return true;
}

bool Graph::checkNext(const Pins& is_pin, const Pin* p) const {
  for (Edge* e : *(p->nexts())) {
    if (!is_pin.count(e->to_)) return false;
  }
  return true;
}

void Graph::bfsPre() const {
  Pins is_pin;
  PinQueue q_pin;
  q_pin.push(out_);
  while (!q_pin.empty()) {
    Pin* p = q_pin.front();
    q_pin.pop();
    is_pin.insert(p);
    Require p_require = p->require();
    for (Edge* e : *(p->pres())) {
      Pin* to = e->to_;
      Delay e_delay = e->delay_;
      Require to_require = to->require();
      if (to_require == INF || (p_require - e_delay < to_require))
        to->setrequire(p_require - e_delay);
      if (checkNext(is_pin, to)) q_pin.push(to);
    }
  }
}

void Graph::bfsNext() const {
  Pins is_pin;
  PinQueue q_pin;
  q_pin.push(in_);
  while (!q_pin.empty()) {
    Pin* p = q_pin.front();
    q_pin.pop();
    is_pin.insert(p);
    Arrival p_arrival = p->arrival();
    for (Edge* e : *(p->nexts())) {
      Pin* to = e->to_;
      Delay e_delay = e->delay_;
      Arrival to_arrival = to->arrival();
      if (to_arrival == INF || (p_arrival + e_delay > to_arrival))
        to->setarrival(p_arrival + e_delay);
      if (checkPre(is_pin, to)) q_pin.push(to);
    }
  }
}

void Graph::connectIn() {
  in_ = new Pin();
  in_->setarrival(0);
  for (size_t i = 0; i < pin_size_; ++i) {
    Pin* p = &pin_[i];
    if (!p->pres()->size()) {
      in_->addNext(new Edge{p, 0});
      p->addPre(new Edge{in_, 0});
    }
  }
}

void Graph::connectOut() {
  out_ = new Pin();
  out_->setrequire(t_);
  for (size_t i = 0; i < pin_size_; ++i) {
    Pin* p = &pin_[i];
    if (!p->nexts()->size()) {
      p->addNext(new Edge{out_, 0});
      out_->addPre(new Edge{p, 0});
    }
  }
}

void Graph::init() {
  connectIn();
  connectOut();
  bfsNext();
  bfsPre();
}

void Graph::readSdc(const Path& path) {
  std::ifstream ifs;
  ifs.open(path, std::ios::in);
  if (!ifs.is_open()) {
    std::cout << "fail open file sdc" << std::endl;
    t_ = -1;
  }
  std::string str;
  Tclock t;
  ifs >> str >> t;
  t_ = t;
}

void Graph::readLib(const Path& path) {
  std::ifstream ifs;
  ifs.open(path, std::ios::in);
  if (!ifs.is_open()) {
    std::cout << "fail open file lib" << std::endl;
    return;
  }
  int n;
  ifs >> n;
  cell_size_ = n;
  cell_ = new Cell[cell_size_];
  for (int i = 0; i < cell_size_; ++i) {
    CellName name;
    int m;
    ifs >> name >> m;
    while (m--) {
      std::string p_in, p_out, dis;
      ifs >> p_in >> p_out >> dis;
      Delay d = INF;
      if (dis != "INF") d = stod(dis);
      cell_[i].addMap(p_in, d);
      if (!m) {
        cell_[i].addMap(p_out, -1);
      }
    }
    cell_map_->insert({name, &cell_[i]});
  }
}

void Graph::readNetlists(const Path& path) {
  std::ifstream ifs;
  ifs.open(path, std::ios::in);
  if (!ifs.is_open()) {
    std::cout << "fail open file netlists" << std::endl;
    return;
  }
  std::unordered_map<std::string, CellName> maps;
  size_t pin_nums = 0;
  int n;
  ifs >> n;
  while (n--) {
    std::string name;
    CellName type;
    ifs >> name >> type;
    maps.insert({name, type});
    if (type == "INPUT" || type == "OUTPUT")
      pin_nums++;
    else {
      if (!cell_map_->count(type)) {
        std::cout << "lib don't have the cell: " << type << std::endl;
        return;
      }
      Cell* cell = (*cell_map_)[type];
      size_t sz = cell->size();
      pin_nums += sz;
    }
  }
  pin_size_ = pin_nums;
  pin_nums = 0;
  pin_ = new Pin[pin_size_];
  for (const auto& it : maps) {
    std::string name = it.first;
    CellName type = it.second;
    if (type == "INPUT" || type == "OUTPUT") {
      PinName pin_name = name;
      pin_map_->insert({name, &pin_[pin_nums]});
      pin_nums++;
    } else {
      Cell* cell = (*cell_map_)[type];
      size_t nums = 0;
      Cell::Maps* maps = cell->maps();
      Pin* pin_out = nullptr;
      for (const auto& it : *maps) {
        Delay delay = it.second;
        if (delay == -1) pin_out = &pin_[pin_nums + nums];
        nums++;
      }
      nums = 0;
      for (const auto& it : *maps) {
        std::string type_name = it.first;
        Delay delay = it.second;
        PinName pin_name = name + "/" + type_name;
        pin_map_->insert({pin_name, &pin_[pin_nums + nums]});
        if (delay != -1) {
          pin_[pin_nums + nums].addNext(new Edge{pin_out, delay});
          pin_out->addPre(new Edge{&pin_[pin_nums + nums], delay});
        }
        nums++;
      }
      pin_nums += nums;
    }
  }
  int m;
  ifs >> m;
  while (m--) {
    PinName from, to;
    ifs >> from >> to;
    Pin *pin_from = (*pin_map_)[from], *pin_to = (*pin_map_)[to];
    pin_from->addNext(new Edge{pin_to, 0});
    pin_to->addPre(new Edge{pin_from, 0});
  }
}

void Graph::read(const Path& sdc_path, const Path& lib_path,
                 const Path& netlists_path) {
  readSdc(sdc_path);
  readLib(lib_path);
  readNetlists(netlists_path);
}

}  // namespace sta
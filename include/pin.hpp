#include "cell.hpp"

namespace sta {

const double INF = 1E9;

typedef double Arrival;
typedef double Require;
typedef double Slack;

class Pin;

struct Edge {
  Pin* to_;
  Delay delay_;
};

class Pin {
 public:
  typedef std::unordered_set<Edge*> Edges;
  Pin() : arrival_(INF), require_(INF) {
    pres_ = new Edges();
    nexts_ = new Edges();
  }
  virtual ~Pin() {
    for (Edge* e : *pres_) delete e;
    for (Edge* e : *nexts_) delete e;
    delete pres_;
    delete nexts_;
  }
  Edges* pres() const { return pres_; }
  void addPre(Edge* p) { pres_->insert(p); }
  Edges* nexts() const { return nexts_; }
  void addNext(Edge* p) { nexts_->insert(p); }
  Arrival arrival() const { return arrival_; }
  void setarrival(const Arrival& a) { arrival_ = a; }
  Require require() const { return require_; }
  void setrequire(const Require& r) { require_ = r; }
  Slack slack() const { return require_ - arrival_; }

 protected:
  Edges *pres_, *nexts_;
  Arrival arrival_;
  Require require_;
};

}  // namespace sta
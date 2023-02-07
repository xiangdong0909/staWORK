#include <unordered_map>
#include <queue>

#include "pin.hpp"

namespace sta {

typedef double Tclock;
typedef std::string Path;
typedef std::string CellName;
typedef std::string PinName;

class Graph {
 public:
  typedef std::unordered_map<CellName, Cell*> CellMap;
  typedef std::unordered_map<PinName, Pin*> PinMap;
  typedef std::unordered_set<Pin*> Pins;
  typedef std::queue<Pin*> PinQueue;
  Graph()
      : t_(0),
        cell_size_(0),
        in_(nullptr),
        out_(nullptr),
        cell_(nullptr),
        pin_(nullptr) {
    cell_map_ = new CellMap();
    pin_map_ = new PinMap();
  }
  virtual ~Graph() {
    if (in_) delete in_;
    if (out_) delete out_;
    if (cell_) delete[] cell_;
    if (pin_) delete[] pin_;
    delete cell_map_;
    delete pin_map_;
  }
  Pin* in() const { return in_; }
  Pin* out() const { return out_; }
  Cell* cell() const { return cell_; }
  size_t cell_size() const { return cell_size_; }
  Pin* pin() const { return pin_; }
  size_t pin_size() const { return pin_size_; }
  CellMap* cell_map() const { return cell_map_; }
  PinMap* pin_map() const { return pin_map_; }
  bool checkPre(const std::unordered_set<Pin*>& is_pin, const Pin* p) const;
  bool checkNext(const std::unordered_set<Pin*>& is_pin, const Pin* p) const;
  void bfsPre() const;
  void bfsNext() const;
  void connectIn();
  void connectOut();
  void init();
  Tclock t() const { return t_; }
  void setT(const Tclock& t) { t_ = t; }
  void readSdc(const Path& path);
  void readLib(const Path& path);
  void readNetlists(const Path& path);
  void read(const Path& sdc_path, const Path& lib_path,
            const Path& netlists_path);

 protected:
  Pin *in_, *out_;
  Tclock t_;
  Cell* cell_;
  size_t cell_size_;
  Pin* pin_;
  size_t pin_size_;
  CellMap* cell_map_;
  PinMap* pin_map_;
};

}  // namespace sta
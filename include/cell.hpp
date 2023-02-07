#include <string>
#include <unordered_set>

namespace sta {

typedef double Delay;

class Cell {
 public:
  typedef std::unordered_map<std::string, Delay> Maps;
  Cell() {
    maps_ = new Maps();
  }
  virtual ~Cell() {
    delete maps_;
  }
  std::size_t size() const { return maps_->size(); }
  Maps* maps() const { return maps_; }
  void addMap(const std::string& s, const Delay& d) { maps_->insert({s, d}); }

 protected:
  Maps* maps_;
};

}  // namespace sta
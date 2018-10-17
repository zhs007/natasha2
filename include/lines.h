#ifndef __NATASHA_LINES_H__
#define __NATASHA_LINES_H__

#include <assert.h>
#include <vector>
#include "array.h"
#include "utils.h"

namespace natasha {

template <int Width, typename PositionType>
class Lines {
 public:
  typedef StaticArray<Width, PositionType> LineInfoT;
  typedef std::vector<LineInfoT> List;

 public:
  Lines() {}
  ~Lines() {}

 public:
  void addLine(LineInfoT& line) { m_lst.push_back(line); }

  const LineInfoT& get(int index) const {
    assert(index >= 0);
    assert(index < m_lst.size());

    return m_lst[index];
  }

  int getNums() const { return m_lst.size(); }

  void clear() { m_lst.clear(); }

  bool isEmpty() const { return m_lst.empty(); }

 protected:
  List m_lst;
};

}  // namespace natasha

#endif  // __NATASHA_LINES_H__
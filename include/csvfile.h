#ifndef __NATASHA_CSVFILE_H__
#define __NATASHA_CSVFILE_H__

#include <assert.h>
#include <map>
#include <string>
#include <vector>

namespace natasha {

class CSVFile {
 public:
  typedef std::map<std::string, std::string> Row;
  typedef std::vector<Row*> List;
  typedef List::iterator ListIter;

 public:
  CSVFile() {}
  ~CSVFile() { clear(); }

 public:
  bool load(const char* filename);

  void clear();

  const char* get(int y, const char* str) const;

  int getLength() const { return m_lst.size(); }

 protected:
  List m_lst;
};

}  // namespace natasha

#endif  // __NATASHA_CSVFILE_H__
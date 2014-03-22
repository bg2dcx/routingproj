#ifndef _table
#define _table


#include <iostream>

using namespace std;

#if defined(GENERIC)
class Table {
  // Students should write this class

 public:
  ostream & Print(ostream &os) const;
};
#endif


#if defined(LINKSTATE)
class Table {
  // Students should write this class

 public:
  ostream & Print(ostream &os) const;
};
#endif

#if defined(DISTANCEVECTOR)

#include <deque>

struct Row {
  unsigned dest;
  unsigned next;
  double cost;
  ostream &Print(ostream &os) const;
  
  Row(unsigned dest, unsigned next, double cost);
};

inline ostream & operator<<(ostream &os, const Row &r) { return r.Print(os);}

class Table {
  deque<Row> m;
 public:
  Table();
  Table(const Table &rhs);
  deque<Row>::iterator getQueueStart(){
	return m.begin();
  }
  deque<Row>::iterator getQueueEnd(){
	return m.end();
  }
  double getCost(const unsigned dest);
  void updateRow(const unsigned next, const unsigned dest, const double cost);
  double getLinkCost(const unsigned dest);
  unsigned getNext(const unsigned dest) const;
  ostream & Print(ostream &os) const;
};
#endif

inline ostream & operator<<(ostream &os, const Table &t) { return t.Print(os);}

#endif

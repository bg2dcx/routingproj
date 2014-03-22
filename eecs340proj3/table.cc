#include "table.h"

#if defined(GENERIC)
ostream & Table::Print(ostream &os) const 
{
  // WRITE THIS
  os << "Table()";
  return os;
}
#endif

#if defined(LINKSTATE)

#endif

#if defined(DISTANCEVECTOR)

Table::Table(){}

Table::Table(const Table &rhs) : m(rhs.m) {}

double Table::getCost(const unsigned dest) 
 {
	deque<Row>::iterator i = m.begin();
	double minCost = -1;
	for (i; i != m.end(); i++)
	   if (i->dest == dest)
	   {
		if (minCost < 0)
			minCost = i->cost;
		else if (i->cost < minCost)
			minCost = i->cost;
		}
	return minCost;
 }
void Table::updateRow(const unsigned next, const unsigned dest, const double cost)
 {
	deque<Row>::iterator i = m.begin();
	for (i; i != m.end(); i++)
	  if (i->next == next && i->dest == dest)
	  {
		i->cost = cost;
		return;
	  }
	m.push_back(*new Row(dest,next,cost));
 }
	
double Table::getLinkCost(const unsigned dest)
 {
	deque<Row>::iterator i = m.begin();
	double minCost = -1;
	for (i;i != m.end();i++)
	   if (i->next == dest && i->dest == dest)
	      minCost = i->cost;
	return minCost;
 }
unsigned Table::getNext(const unsigned dest) const
 {
	deque<Row>::const_iterator i = m.begin();
	unsigned minNext = 0;
	double minCost = -1;
	for (i; i!= m.end(); i++)
	  if (i->dest == dest)
	{
		if (minCost < 0)
		{
			minCost = i->cost;
			minNext = i->next;
		}
	    else if (i->cost < minCost)
		{
			minCost = i->cost;
			minNext = i->next;
		}
	}
	return minNext;
}



Row::Row(const unsigned d, const unsigned n, const double c) :
  dest(d), next(n), cost(c)
{}

ostream & Row::Print(ostream &os) const 
{
  os <<"Row(dest="<<dest<<", next="<<next<<", cost="<<cost<<")";
  return os;
}

ostream & Table::Print(ostream &os) const 
{
  os<<"Table(rows={";
  for (deque<Row>::const_iterator i=m.begin();i!=m.end();++i) { 
    os <<(*i)<<", ";
  }
  os<<"})";
  return os;
}
    
#endif

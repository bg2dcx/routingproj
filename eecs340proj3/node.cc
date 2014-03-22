#include "node.h"
#include "context.h"
#include "error.h"


Node::Node(const unsigned n, SimulationContext *c, double b, double l) : 
    number(n), context(c), bw(b), lat(l) 
{}

Node::Node() 
{ throw GeneralException(); }

Node::Node(const Node &rhs) : 
  number(rhs.number), context(rhs.context), bw(rhs.bw), lat(rhs.lat) {}

Node & Node::operator=(const Node &rhs) 
{
  return *(new(this)Node(rhs));
}

void Node::SetNumber(const unsigned n) 
{ number=n;}

unsigned Node::GetNumber() const
{ return number;}

void Node::SetLatency(const double l) 
{ lat=l;}

double Node::GetLatency() const 
{ return lat;}

void Node::SetBW(const double b) 
{ bw=b;}

double Node::GetBW() const 
{ return bw;}

Node::~Node()
{}

void Node::SendToNeighbors(const RoutingMessage *m)
{
  context->SendToNeighbors(this,m);
}

void Node::SendToNeighbor(const Node *n, const RoutingMessage *m)
{
  context->SendToNeighbor(this,n,m);
}

deque<Node*> *Node::GetNeighbors() const
{
  return context->GetNeighbors(this);
}

void Node::SetTimeOut(const double timefromnow)
{
  context->TimeOut(this,timefromnow);
}


bool Node::Matches(const Node &rhs) const
{
  return number==rhs.number;
}


#if defined(GENERIC)
void Node::LinkUpdate(const Link *l)
{
  cerr << *this << " got a link update: "<<*l<<endl;
  //Do Something generic:
  SendToNeighbors(new RoutingMessage);
}


void Node::ProcessIncomingRoutingMessage(const RoutingMessage *m)
{
  cerr << *this << " got a routing messagee: "<<*m<<" Ignored "<<endl;
}


void Node::TimeOut()
{
  cerr << *this << " got a timeout: ignored"<<endl;
}

Node *Node::GetNextHop(const Node *destination) const
{
  return 0;
}

Table *Node::GetRoutingTable() const
{
  return new Table;
}


ostream & Node::Print(ostream &os) const
{
  os << "Node(number="<<number<<", lat="<<lat<<", bw="<<bw<<")";
  return os;
}

#endif

#if defined(LINKSTATE)

const unsigned Node::maxttl=5;

void Node::LinkUpdate(const Link *l)
{
  // Add To table and update graph
  // Now flood to rest of network
  cerr << *this<<": Link Update: "<<*l<<endl;
  RoutingMessage *m = new RoutingMessage(*this,*l,maxttl,seqno++);
  SendToNeighbors(m);
}


void Node::ProcessIncomingRoutingMessage(const RoutingMessage *m)
{
  cerr << *this << " Routing Message: "<<*m;
  if (m->srcnode.Matches(*this)) {
    // skip, one of ours
    cerr << " IGNORED"<<endl;
  } else {
    // update our table
    if (m->ttl==0) { 
      cerr << " DROPPED, TTL=0"<<endl;
    } else {
      // Forward to neighbors
      RoutingMessage *out=new RoutingMessage(*m);
      out->ttl--;
      cerr << " FORWARDED"<<endl;
      SendToNeighbors(out);
    }
  }
}

void Node::TimeOut()
{
  cerr << *this << " got a timeout: ignored"<<endl;
}

Node *Node::GetNextHop(const Node *destination) const
{
  // WRITE
  return 0;
}

Table *Node::GetRoutingTable() const
{
  // WRITE
  return 0;
}


ostream & Node::Print(ostream &os) const
{
  os << "Node(number="<<number<<", lat="<<lat<<", bw="<<bw<<")";
  return os;
}
#endif


#if defined(DISTANCEVECTOR)

void Node::LinkUpdate(const Link *l)
{
  // a change on a link may affect multiple rows
  for (deque<Row>::iterator i = table.getQueueStart(); i!= table.getQueueEnd(); i++)
    if (i->next == l->GetDest() && i->dest != l->GetDest())
  {
	double oldBest = table.getCost(i->dest);
	table.updateRow(i->next,i->dest, i->cost - table.getLinkCost(l->GetDest()) + l->GetLatency());
	double newBest = table.getCost(i->dest);
	if (oldBest != newBest)
	{
		RoutingMessage *newm = new RoutingMessage(number,i->dest, newBest);
		SendToNeighbors(newm);
	}
  }
  double oldBest = table.getCost(l->GetDest());
  table.updateRow(l->GetDest(),l->GetDest(),l->GetLatency());
  double newBest = table.getCost(l->GetDest());
  if (oldBest != newBest)
  {
		RoutingMessage *newm = new RoutingMessage(number,l->GetDest(), newBest);
		SendToNeighbors(newm);
   }   
  cerr << *this<<": Link Update: "<<*l<<endl;
}


void Node::ProcessIncomingRoutingMessage(const RoutingMessage *m)
{
  //a routing message may change only one row
  cerr << "Node "<<GetNumber()<<": "<<m->srcnode<<" has new cost "<<m->cost
       <<" path to "<<m->dest<<" Action: ";

  if (m->dest==GetNumber()) { 
    cerr << " ourself - ignored\n";
    return;
  }
  double oldBest = table.getCost(m->dest);
  table.updateRow(m->srcnode, m->dest, m->cost + table.getLinkCost(m->srcnode));
  double newBest = table.getCost(m->dest);
  if (oldBest != newBest)
  {
	RoutingMessage *newm = new RoutingMessage(number, m->dest, newBest);
	SendToNeighbors(newm);
  }
}

void Node::TimeOut()
{
  cerr << *this << " got a timeout: ignored"<<endl;
}


Node *Node::GetNextHop(const Node *destination) const
{
	deque<Node*> *nodes = GetNeighbors();
	unsigned nextHopNum = table.getNext(destination->GetNumber());
	for (deque<Node*>::const_iterator i = nodes->begin();i!= nodes->end(); ++i)
	   if (nextHopNum == (**i).GetNumber())
	{
		return new Node(**i);
	}
}



Table *Node::GetRoutingTable() const 
{ 
		
	return new Table(this->table);  
}

ostream & Node::Print(ostream &os) const
{
  os << "Node(number="<<number<<", lat="<<lat<<", bw="<<bw;
  os << ", table="<<table<<")";
  return os;
}
#endif

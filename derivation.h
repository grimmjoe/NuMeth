#ifndef DERIVATION_H
#define DERIVATION_H

#include <string>
#include "function.h"

class DerivationNode;

class Derivator
{
public:
	Derivator(const Func& _f);
	Func derive();
	Func derive(int n);
	~Derivator();
private:
	Func f;
	DerivationNode* derNode;
	void makeDerivationNodes();
	DerivationNode* _derive(const DerivationNode* derN);
	DerivationNode* getDerivative(const DerivationNode* d);
	void getRidOfTavtals(DerivationNode*& dn);
};

class DerivationNode
{
public:
	friend class Derivator;
	DerivationNode( std::string ss) : first(0), last(0), op(char()), s(ss) { }
	DerivationNode(DerivationNode* f, DerivationNode* l, char o = char())
		: first(f), last(l), op(o), s("") { }
	DerivationNode* getFirst() const { return first; }
	DerivationNode* getLast() const { return last; }
	char getOperator() const { return op; }
	std::string getString() const { return s; }
	std::string getStringRep() const;
	void getListRep(std::list<std::string>& l);
	~DerivationNode()
	{
                if (!first) {
			delete first;
		}
                if (!last) {
			delete last;
		}
	}
private:
	DerivationNode* first;
	DerivationNode* last;
	char op;
	std::string s;
};

#endif

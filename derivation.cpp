#include "derivation.h"
#include "functionparser.h"
#include <stack>
#include <cmath>
#include <iostream>

Derivator::Derivator(const Func &_f) : f(_f)
{
	makeDerivationNodes();
}

Derivator::~Derivator()
{
	if (derNode) {
		delete derNode;
	}
}

void Derivator::makeDerivationNodes()
{
	std::stack<DerivationNode*> s;
	typedef std::list<std::string>::iterator LI;
	for (LI i = f.parsed.begin(); i != f.parsed.end(); ++i) {
		if (FunctionParser::isOperator(*i)) {
			if (s.empty()) {
				derNode = 0;
				return;
			}
			DerivationNode* second = s.top();
			s.pop();
			if (s.empty()) {
				derNode = 0;
				return;
			}
			DerivationNode* first = s.top();
			s.pop();
			DerivationNode* newNode = new DerivationNode(first, second, (*i)[0]);
			s.push(newNode);
		}
		else if (FunctionParser::isFunction(*i)) {
			if (s.empty()) {
				derNode = 0;
				return;
			}
			DerivationNode* first = new DerivationNode(*i);
			DerivationNode* second = s.top();
			s.pop();
			DerivationNode* newNode = new DerivationNode(first, second);
			s.push(newNode);
		}
		else {
			s.push(new DerivationNode(*i));
		}
	}
        if (!s.empty()) {
            derNode = s.top();
            s.pop();
        }
        else {
            derNode = 0;
        }
}

Func Derivator::derive()
{
	DerivationNode* derived = _derive(derNode);
	getRidOfTavtals(derived);
	std::list<std::string> parsed;
	derived->getListRep(parsed);
        delete derived;

	return Func(parsed, f.arg);
}

Func Derivator::derive(int n)
{
	DerivationNode* derived = derNode;
	for (int i = 1; i <= n; ++i) {
		derived = _derive(derived);
		getRidOfTavtals(derived);
	}
	getRidOfTavtals(derived);
	std::list<std::string> parsed;
	derived->getListRep(parsed);
        delete derived;
	return Func(parsed, f.arg);
}

DerivationNode* Derivator::_derive(const DerivationNode* node)
{
	if (node->first == 0 && node->last == 0) {
		if (FunctionParser::isNumber(node->s)) {
			return new DerivationNode("0");
		}
		return new DerivationNode("1");
	}

	if (node->op != 0){
		switch (node->op) {
			case '+': case '-':
				{
					if (node->last->first == 0 && FunctionParser::isNumber(node->last->s)) {
						return _derive(node->first);
					}
					if (node->first->first == 0 && FunctionParser::isNumber(node->last->s)) {
						return (node->op == '+' ? _derive(node->last) :
							new DerivationNode(new DerivationNode("-1"), _derive(node->last), '*'));
					}
					return new DerivationNode(_derive(node->first), _derive(node->last), node->op);
					break;
				}
			case '*':
				{
					if (node->first->first == 0  && FunctionParser::isNumber(node->first->s)) {
						return new DerivationNode(node->first, _derive(node->last), '*');
					}
					if (node->last->first == 0  && FunctionParser::isNumber(node->last->s)) {
						return new DerivationNode(node->last, _derive(node->first), '*');
					}
					DerivationNode* f = new DerivationNode(_derive(node->first), node->last, '*');
					DerivationNode* ss = new DerivationNode(node->first, _derive(node->last), '*');
					return new DerivationNode(f, ss, '+');
					break;
				}
			case '/':
				{
					if (node->last->first == 0 && FunctionParser::isNumber(node->last->s)) {
						DerivationNode* one = new DerivationNode("1");
						DerivationNode* next = new DerivationNode(node->last->s);
						DerivationNode* div = new DerivationNode(one, next, '/');
						return new DerivationNode(div, _derive(node->first), '*');
					}
					else {
						DerivationNode* f = new DerivationNode(_derive(node->first),
														node->last, '*');
						DerivationNode* ss = new DerivationNode(node->first,
													_derive(node->last), '*');
						DerivationNode* up = new DerivationNode(f, ss, '-');
						DerivationNode* p = new DerivationNode("2");
						DerivationNode* down = new DerivationNode(node->last, p, '^');
						return new DerivationNode(up, down, '/');
					}
					break;
				}
			case '^':
				{
					double n = FunctionParser::doubleFromStr(node->last->s);
					DerivationNode* p = new DerivationNode(FunctionParser::strFromDouble(n - 1));
					DerivationNode* f = new DerivationNode(node->first, p, '^');
					DerivationNode* ss = new DerivationNode(node->last, f, '*');
					return new DerivationNode(ss, _derive(node->first), '*');
				}
		}
	}
	else {
		return new DerivationNode(getDerivative(node), _derive(node->last), '*');
	}
}

DerivationNode* Derivator::getDerivative(const DerivationNode *d)
{
	if (d->first->s == "SIN") {
		return new DerivationNode(new DerivationNode("COS"), d->last);
	}
	if (d->first->s == "COS") {
		DerivationNode* temp = new DerivationNode("-1");
		DerivationNode* t = new DerivationNode(new DerivationNode("SIN"), d->last);
		return new DerivationNode(temp, t, '*');
	}
	if (d->first->s == "SQRT") {
		DerivationNode* temp = new DerivationNode(new DerivationNode("2"),
			const_cast<DerivationNode*>(d), '*');
		return new DerivationNode(new DerivationNode("1"), temp, '/');
	}
	if (d->first->s == "TAN") {
		DerivationNode* _cos = new DerivationNode("COS");
		DerivationNode* __cos = new DerivationNode(_cos, d->last);
		DerivationNode* duo = new DerivationNode(__cos, new DerivationNode("2"), '^');
		return new DerivationNode(new DerivationNode("1"), duo, '/');
	}
	if (d->first->s == "ARCTAN") {
		DerivationNode* duo = new DerivationNode(d->last, new DerivationNode("2"), '^');
		DerivationNode* sum = new DerivationNode(new DerivationNode("1"), duo, '+');
		return new DerivationNode(new DerivationNode("1"), sum, '/');
	}
	if (d->first->s == "ARCSIN") {
		DerivationNode* duo = new DerivationNode(d->last, new DerivationNode("2"), '^');
		DerivationNode* sub = new DerivationNode(new DerivationNode("1"), duo, '-');
		DerivationNode* _sqrt = new DerivationNode(new DerivationNode("SQRT"), sub);
		return new DerivationNode(new DerivationNode("1"), _sqrt, '/');
	}
	if (d->first->s == "ARCCOS") {
		DerivationNode* duo = new DerivationNode(d->last, new DerivationNode("2"), '^');
		DerivationNode* sub = new DerivationNode(new DerivationNode("1"), duo, '-');
		DerivationNode* _sqrt = new DerivationNode(new DerivationNode("SQRT"), sub);
		return new DerivationNode(new DerivationNode("-1"), _sqrt, '/');
	}
	if (d->first->s == "LOG10") {
		double x = log(10.0);
		DerivationNode* temp = new DerivationNode(d->last,
			new DerivationNode(FunctionParser::strFromDouble(x)), '*');
		return new DerivationNode(new DerivationNode("1"), temp, '/');
	}
	if (d->first->s == "LN") {
		return new DerivationNode(new DerivationNode("1"), d->last, '/');
	}
	if (d->first->s == "EXP") {
		return const_cast<DerivationNode*>(d);
	}
}

void Derivator::getRidOfTavtals(DerivationNode*& dn)
{
	if (dn->first == 0) {
		return;
	}
	if (dn->first->first == 0) {
		if (dn->first->s == "0") {
			switch(dn->op) {
				case '+':
					dn = dn->last;
					getRidOfTavtals(dn);
					break;
				case '-':
					dn = new DerivationNode(new DerivationNode("-1"), dn->last, '*');
					break;
				case '*': case '/': case '^':
					dn = new DerivationNode("0");
					getRidOfTavtals(dn);
			}
		}
		else if (FunctionParser::doubleFromStr(dn->first->s) == 1) {
			if (dn->op == '*') {
				dn = dn->last;
				getRidOfTavtals(dn);
			}
		}
	}
	if (dn->last != 0 && dn->last->first == 0) {
		if (dn->last->s == "0") {
			switch(dn->op) {
				case '+': case '-':
					dn = dn->first;
					getRidOfTavtals(dn);
					break;
				case '*':
					dn = new DerivationNode("0");
					getRidOfTavtals(dn);
					break;
				 case '^':
					 dn = new DerivationNode("1");
					 break;
			}
		}
		else if (FunctionParser::doubleFromStr(dn->last->s) == 1) {
			if (dn->op == '*') {
				dn = dn->first;
				getRidOfTavtals(dn);
			}
		}
	}
	if (dn->first != 0) {
		getRidOfTavtals(dn->first);
		getRidOfTavtals(dn->last);
	}
}

std::string DerivationNode::getStringRep() const
{
	if (first == 0 && last == 0) {
		return s;
	}

	return (first->getStringRep() + op + last->getStringRep());
}

void DerivationNode::getListRep(std::list<std::string> &l)
{
	if (first == 0 && last == 0) {
		l.push_back(s);
		return;
	}

	if (FunctionParser::isFunction(first->s)) {
		last->getListRep(l);
		l.push_back(first->s);
		return;
	}

	first->getListRep(l);
	last->getListRep(l);
	std::string str = "";
	str += op;
	l.push_back(str);
}


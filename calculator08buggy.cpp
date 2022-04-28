/*
	calculator08buggy.cpp | From: Programming -- Principles and Practice Using C++, by Bjarne Stroustrup
	We have inserted 3 bugs that the compiler will catch and 3 that it won't.
*/

#include "std_lib_facilities.h"

struct Token {
	char kind;
	double value;
	string name;
	Token(char ch) :kind(ch), value(0) { }
	Token(char ch, double val) :kind(ch), value(val) { }
	Token(char ch, string s) :kind(ch), name(s) { }
};

class Token_stream {
	bool full;
	Token buffer;
public:
	Token_stream() :full(0), buffer(0) { }

	Token get();
	void unget(Token t) { buffer = t; full = true; }

	void ignore(char);
};

const char let = 'L';
const char quit = 'Q';
const char print = ';';
const char number = '8';
const char name = 'a';
const char constVal = 'C';

Token Token_stream::get()
{
	if (full) { full = false; return buffer; }
	char ch;
	cin >> ch;
	switch (ch) {
	case '(':
	case ')':
	case '+':
	case '-':
	case '*':
	case '/':
	case '%':
	case ';':
	case '=':
		return Token(ch);
	case '.':
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	{	cin.unget();
	double val;
	cin >> val;
	return Token(number, val);
	}
	default:
		if (isalpha(ch) || ch == '_') { // ADD || ch == '_'
			string s;
			s += ch;
			while (cin.get(ch) && (isalpha(ch) || isdigit(ch) || ch == '_')) s += ch; // s = ch -> s += ch // ADD || ch == '_'
			cin.unget();
			if (s == "let") return Token(let);
			if (s == "quit") return Token(quit); // return Token(name) -> return Token(quit)
			if (s == "const") return Token(constVal);
			return Token(name, s);
		}
		error("Bad token");
		return Token(quit); // Satisfies warning
	}
}

void Token_stream::ignore(char c)
{
	if (full && c == buffer.kind) {
		full = false;
		return;
	}
	full = false;

	char ch;
	while (cin >> ch)
		if (ch == c) return;
}

struct Variable {
	string name;
	double value;
	bool constVal; // For const values
	Variable(string n, double v, bool c) :name(n), value(v), constVal(c) { }
};

vector<Variable> names;

double get_value(string s)
{
	for (int i = 0; i < names.size(); ++i)
		if (names[i].name == s) return names[i].value;
	error("get: undefined name ", s);
	return -1; // Satisfies warning
}

void set_value(string s, double d)
{
	for (int i = 0; i <= names.size(); ++i)
		if (names[i].name == s)
		{
			//cout << names[i].constVal << endl;
			if(names[i].constVal == true)
			{
				error("set: name is a constant ", s);
			}
			names[i].value = d;
			return;
		}
	error("set: undefined name ", s);
}

bool is_declared(string s)
{
	for (int i = 0; i < names.size(); ++i)
		if (names[i].name == s) return true;
	return false;
}

double define_name(string var, double val, bool constVal) // From book pg.246
{
 if (is_declared(var))
	error(var, " declared twice");

 names.push_back(Variable(var, val, constVal)); // var_table -> names

 return val;
}

Token_stream ts;

double expression();

double primary()
{
	Token t = ts.get();
	switch (t.kind) {
	case '(':
	{	double d = expression();
	t = ts.get();
	if (t.kind != ')') error("')' expected");
	return d; // Insert return d;
	}
	case '-':
		return -primary();
	case number:
		return t.value;
	case name:
	{
		// When changine a value of a previously declared variable
		Token temp = ts.get();
		if(temp.kind == '=')
		{
			double d = expression();
			set_value(t.name, d);
			return d;
		}
		else
		{
			ts.unget(temp);
		}

		return get_value(t.name);
	}
	case constVal:
	{
		Token tempName = ts.get();

		if(is_declared(tempName.name)) 
		{
			error(tempName.name, " declared twice");
		}
		else
		{
			Token tempOp = ts.get();

			if(tempOp.kind == '=')
			{
				double d = expression();
				define_name(tempName.name, d, true);
				return d;
			}
			else
			{
				error("'=' expected");
			}
		}
	}
	default:
		error("primary expected");
		return -1; // Satisfies warning
	}
}

double term()
{
	double left = primary();
	while (true) {
		Token t = ts.get();
		switch (t.kind) {
		case '*':
			left *= primary();
			break;
		case '/':
		{	double d = primary();
		if (d == 0) error("divide by zero");
		left /= d;
		break;
		}
		case '%':
		{
			double d = primary();
			if (d == 0) error("divide by zero"); 
			left = fmod(left,d);
			//t = ts.get();
			break;
		}
		default:
			ts.unget(t);
			return left;
		}
	}
}

double expression()
{
	double left = term();
	while (true) {
		Token t = ts.get();
		switch (t.kind) {
		case '+':
			left += term();
			break;
		case '-':
			left -= term();
			break;
		default:
			ts.unget(t);
			return left;
		}
	}
}

double declaration()
{
	Token t = ts.get();
	if (t.kind != 'a') error("name expected in declaration");
	string name = t.name;
	if (is_declared(name)) error(name, " declared twice");
	Token t2 = ts.get();
	if (t2.kind != '=') error("= missing in declaration of ", name);
	double d = expression();
	names.push_back(Variable(name, d, false));
	return d;
}

double statement()
{
	Token t = ts.get();

	switch (t.kind)
	{
		case let:
			return declaration();
		default:
		{	
			ts.unget(t);
			return expression();
		}
	}
}

void clean_up_mess()
{
	ts.ignore(print);
}

const string prompt = "> ";
const string result = "= ";

void calculate()
{
	while (true) try {
		cout << prompt;
		Token t = ts.get();
		while (t.kind == print) t = ts.get();
		if (t.kind == quit) return;
		ts.unget(t);
		cout << result << statement() << endl;
	}
	catch (runtime_error& e) {
		cerr << e.what() << endl;
		clean_up_mess();
	}
}

int main()
try {
	define_name("k", 1000, true);

	calculate();
	return 0;
}
catch (exception& e) {
	cerr << "exception: " << e.what() << endl;
	char c;
	while (cin >> c && c != ';');
	return 1;
}
catch (...) {
	cerr << "exception\n";
	char c;
	while (cin >> c && c != ';');
	return 2;
}

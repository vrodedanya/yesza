#ifndef YESZA_RPN_HPP
#define YESZA_RPN_HPP

#include <stack>
#include <vector>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <functional>
#include <cmath>
#include <map>

namespace yesza
{
	inline const std::map<std::string, std::function<void(std::stack<double>&)>> operationsCallback = 
	{
		{"+", [](std::stack<double>& stack){double first = stack.top(); stack.pop(); stack.top() = first + stack.top();}},
		{"-", [](std::stack<double>& stack){double first = stack.top(); stack.pop(); stack.top() = first - stack.top();}},
		{"*", [](std::stack<double>& stack){double first = stack.top(); stack.pop(); stack.top() = first * stack.top();}},
		{"/", [](std::stack<double>& stack){double first = stack.top(); stack.pop(); 
											   if (first == 0) throw std::runtime_error("Division by zero"); stack.top() = stack.top() / first;}},
		{"^", [](std::stack<double>& stack){double first = stack.top(); stack.pop(); stack.top() = std::pow(stack.top(), first);}},
		{"~", [](std::stack<double>& stack){stack.top() = -stack.top();}},
		{"sin", [](std::stack<double>& stack){stack.top() = std::sin(stack.top());}},
		{"cos", [](std::stack<double>& stack){stack.top() = std::cos(stack.top());}},
		{"tan", [](std::stack<double>& stack){stack.top() = std::tan(stack.top());}},
		{"cotan", [](std::stack<double>& stack){stack.top() = std::tan(M_PI_2 - stack.top());}},
	};
	class equation
	{
	private:
		std::vector<std::string> arguments; 
		friend class state_machine;
	public:
		double operator()(double argument = 0)
		{
			if (arguments.empty()) throw std::runtime_error("Doesn't have arguments");
			std::stack<double> buffer;

			for (auto it = arguments.cbegin(); it != arguments.cend() ; it++)
			{
				if (operationsCallback.find(*it) != operationsCallback.end())
				{
					auto callback = operationsCallback.find(*it);
					if (callback == operationsCallback.end()) throw std::runtime_error("This operation doesn't exist");
					callback->second(buffer);
				}
				else
				{
					buffer.push((!std::isdigit((*it)[0])) ? argument : std::stod(*it));
				}
			}
			return buffer.top();
		}
	};

	class state_machine
	{
	private:
		std::vector<std::string> operations; // storage operations
		std::vector<std::string> result;
		std::vector<char> operators;
		std::vector<std::string> functions;
		std::string handlingString;

		enum state
		{
			undefined,
			number,
			oper,
			function,
		} currentState = state::undefined;
		
		enum mod
		{
			empty,
			negative,
			negativeBlock,
		};
		std::stack<mod> mods;
	
		template <typename ITERATOR, typename CALLABLE>
		std::string getElement(ITERATOR begin, ITERATOR end, CALLABLE functor)
		{
			std::string buffer;
			for (; begin != end ; begin++)
			{
				if (functor(*begin)) 
				{
					break;
				}
				buffer += *begin;
			}
			return buffer;
		}

		void process_undefined(std::string::const_iterator& it)
		{
			if (std::isdigit(static_cast<unsigned char>(*it)))
			{
				currentState = state::number;
				auto isNotNumber = [](char ch){return !(std::isdigit(ch) || ch == '.');};
				std::string string_number = getElement(it, handlingString.cend(), isNotNumber);
				it += string_number.size() - 1;
				result.emplace_back(string_number);
			}
			else if (std::find(operators.begin(), operators.end(), *it) != operators.end())
			{
				if (*it == '*' || *it == '/') throw std::runtime_error(std::string("Bad operator in the begining. Got ") + *it);
				currentState = state::oper;
				if (*it == '-') 
				{
					mods.push(mod::negative);
				}
			}
			else if (*it == '(')
			{
				currentState = state::oper;
				operations.push_back("(");
			}
			else if (*it != ' ')
			{
				currentState = state::function;
				auto isNotString = [](char ch){return !((ch >= 'a' && ch <='z') || (ch >= 'A' && ch <= 'Z'));};
				std::string string_function = getElement(it, handlingString.cend(), isNotString);
				it += string_function.size() - 1;
				if (string_function == "x")
				{
					currentState = state::number;
					result.emplace_back(string_function);
				}
				else
				{
					operations.push_back(string_function);
				}
			}
		}

		void process_number(std::string::const_iterator& it)
		{
			if (*it == ')')
			{
				while(operations.back() != "(")
				{
					result.push_back(operations.back());
					operations.pop_back();
				}
				operations.pop_back();
				if (!mods.empty() && mods.top() == mod::negativeBlock) 
				{
					result.push_back("~");
					mods.pop();
				}
				return;
			}
			if (std::find(operators.begin(), operators.end(), *it) == operators.end()) throw std::runtime_error(std::string("Expected operator. Got ") + std::to_string(*it));
			currentState = state::oper;
// TODO Add prioty to operations
			if (*it == '*' || *it == '/')
			{
				while (!operations.empty() && (operations.back() == std::string("^") || std::find(functions.begin(), functions.end(), operations.back()) != functions.end()))
				{
					result.push_back(operations.back());
					operations.pop_back();
				}
			}
			else if (*it == '+' || *it == '-')
			{
				while (!operations.empty() && (operations.back() == std::string("*") || operations.back() == "/" || operations.back() == "^"
						|| std::find(functions.begin(), functions.end(), operations.back()) != functions.end()))
				{
					result.push_back(operations.back());
					operations.pop_back();
				}
			}
			std::string string_operator;
			string_operator = *it;
			if (*it == '-')
			{
				mods.push(mod::negative);
				string_operator = "+";
			}
			operations.emplace_back(string_operator);
		}
		
		void process_oper(std::string::const_iterator& it)
		{
			if (std::isdigit(static_cast<unsigned char>(*it)))
			{
				currentState = state::number;
				auto isNotNumber = [](char ch){return !(std::isdigit(ch) || ch == '.');};
				std::string string_number = getElement(it, handlingString.cend(), isNotNumber);
				it += string_number.size() - 1;
				result.push_back(string_number);
				if (!mods.empty() && mods.top() == mod::negative)
				{
					result.push_back("~");
					mods.pop();
				}
			}
			else if (*it == '(')
			{
				currentState = state::oper;
				operations.push_back("(");
				if (!mods.empty())
				{
					if (mods.top() == mod::negative) mods.top() = mod::negativeBlock;
					else mods.push(mod::negativeBlock);
				}
			}
			else if (std::find(operators.begin(), operators.end(), *it) != operators.end())
			{
				if (*it == '*' || *it == '/') throw std::runtime_error(std::string("Bad operator in the begining. Got ") + *it);
				currentState = state::oper;
				if (*it == '-') 
				{
					mods.push(mod::negative);
				}
			}
			else if (*it != ' ')
			{
				auto isNotString = [](char ch){return !((ch >= 'a' && ch <='z') || (ch >= 'A' && ch <= 'Z'));};
				std::string string_function = getElement(it, handlingString.cend(), isNotString);
				it += string_function.size() - 1;
				if (string_function == "x")
				{
					currentState = state::number;
					result.emplace_back(string_function);
				}
				else
				{
					currentState = state::function;
					operations.push_back(string_function);
				}
			}
		}

		void process_function(std::string::const_iterator& it)
		{
			if (*it == '(')
			{
				currentState = state::oper;
				operations.push_back("(");
				if (!mods.empty())
				{
					if (mods.top() == mod::negative) mods.top() = mod::negativeBlock;
					else mods.push(mod::negativeBlock);
				}
			}
			else throw std::runtime_error("Expected ( after function name");
		}

	public:
		state_machine(const std::string& str)
			:
				handlingString{str}
		{
			for (auto it = operationsCallback.cbegin() ; it != operationsCallback.cend() ; it++)
			{
				if (it->first.size() == 1)
				{
					operators.push_back(it->first[0]);
				}
				else
				{
					functions.push_back(it->first);
				}
			}
		}
		equation process()
		{
			for (auto it = handlingString.cbegin() ; it != handlingString.cend() ; it++)
			{
				if (*it == ' ') continue;
				if (*it == '\0') break;
				if (currentState == state::undefined)
				{
					process_undefined(it);
				}
				else if (currentState == state::number)
				{
					process_number(it);
				}
				else if (currentState == state::oper)
				{
					process_oper(it);
				}
				else if (currentState == state::function)
				{
					process_function(it);
				}
			}
			for (auto it = operations.rbegin() ; it != operations.rend() ; it++)
			{
				result.push_back(std::move(*it));
			}
			equation buf;
			buf.arguments = std::move(result);
			return buf;
		}
	};
	inline equation make_equation(std::string equation)
	{
		state_machine sm(equation);
		auto buf_equation = sm.process();
		return buf_equation;
	}
	inline double count(std::string equation)
	{
		return make_equation(equation)();
	}
}

#endif //YESZA_RPN_HPP

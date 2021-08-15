#ifndef YESZA_RPN_HPP
#define YESZA_RPN_HPP

#include <iostream>
#include <stack>
#include <vector>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <functional>
#include <cmath>
#include "logger.hpp"

namespace yesza
{
	class equation // TODO add functions supporting
	{
	private:
		std::vector<std::string> arguments; // change by vector
		friend class state_machine;
	public:
		double operator()(double argument = 0)
		{
			logger::medium("equation()", "called operator()");
			if (arguments.empty()) return 0;
			std::stack<double> buffer;

			for (auto it = arguments.cbegin(); it != arguments.cend() ; it++)
			{
				logger::low("equation()", "got", *it);
				if (!std::isdigit((*it)[0]) && !std::isdigit((*it)[1]) && *it != "x") // TODO change branches to map storage
				{
					logger::low("equation()", "perfom operation", *it);
					if (*it == "+")
					{
						double first = buffer.top();
						buffer.pop();
						double second = buffer.top();
						buffer.top() = first + second;
					}
					else if (*it == "-")
					{
						double first = buffer.top();
						buffer.pop();
						double second = buffer.top();
						buffer.top() = first - second;
					}
					else if (*it == "*")
					{
						double first = buffer.top();
						buffer.pop();
						double second = buffer.top();
						buffer.top() = first * second;
					}
					else if (*it == "/")
					{
						double first = buffer.top();
						buffer.pop();
						double second = buffer.top();
						buffer.top() = second / first;
					}
					else if (*it == "sin")
					{
						double first = buffer.top();
						buffer.top() = std::sin(first);
					}
					else if (*it == "cos")
					{
						double first = buffer.top();
						buffer.top() = std::cos(first);
					}
					else if (*it == "tan")
					{
						double first = buffer.top();
						buffer.top() = std::tan(first);
					}
					else if (*it == "cotan")
					{
						double first = buffer.top();
						buffer.top() = std::tan(M_PI_2 - first);
					}
				}
				else
				{
					logger::low("equation()", "argument", *it);
					buffer.push((*it == "x") ? argument :std::stod(*it));
				}
			}
			logger::low("equation()", "result");
			return buffer.top();
		}
	};

	// TODO fix issue with negative blocks
	class state_machine
	{
	private:
		std::vector<std::string> operations; // storage operations
		std::vector<std::string> result;
		const std::vector<char> operators{'+', '-', '*', '/'}; // TODO add ^
		const std::vector<std::string> functions{"sin", "cos", "tan", "cotan"}; // TODO add more functions
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
			negative
		} currentMod = mod::empty;
	
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
			logger::medium("state_machine", "undefined branch");
			if (std::isdigit(static_cast<unsigned char>(*it)))
			{
				logger::low("state_machine", "it's a string_number");
				currentState = state::number;
				auto isNotNumber = [](char ch){return !(std::isdigit(ch) || ch == '.');};
				std::string string_number = getElement(it, handlingString.cend(), isNotNumber);
				logger::low("state_machine", "string_number is", string_number);
				it += string_number.size() - 1;
				result.emplace_back(string_number);
			}
			else if (std::find(operators.begin(), operators.end(), *it) != operators.end())
			{
				if (*it == '*' || *it == '/') throw std::runtime_error(std::string("Bad operator in the begining. Got ") + *it);
				currentState = state::oper;
				logger::low("state_machine", "it's a operator");
				if (*it == '-') currentMod = mod::negative;
			}
			else if (*it == '(')
			{
				currentState = state::oper;
				logger::low("state_machine", "it's a open bracket");
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
					logger::low("state_machine", "it's undefined variable", string_function);
					currentState = state::number;
					result.emplace_back(string_function);
				}
				else
				{
					logger::low("state_machine", "it's a string_function");
					logger::low("state_machine", "string_function is", string_function);
					operations.push_back(string_function);
				}
			}
		}

		void process_number(std::string::const_iterator& it)
		{
			logger::medium("state_machine", "number branch");
			if (*it == ')')
			{
				logger::low("state_machine", "it's a close bracket");
				while(operations.back() != "(")
				{
					result.push_back(operations.back());
					operations.pop_back();
				}
				operations.pop_back();
				return;
			}
			if (std::find(operators.begin(), operators.end(), *it) == operators.end()) throw std::runtime_error(std::string("Expected operator. Got ") + std::to_string(*it));
			logger::low("state_machine", "it's a operator", *it);
			currentState = state::oper;
			while (!operations.empty() && (operations.back() == std::string("*") || operations.back() == "/" 
					|| std::find(functions.begin(), functions.end(), operations.back()) != functions.end()))
			{
				result.push_back(operations.back());
				operations.pop_back();
			}
			std::string string_operator;
			string_operator = *it;
			if (*it == '-')
			{
				if (currentMod == mod::negative) currentMod = mod::empty;
				else currentMod = mod::negative;
				string_operator = "+";
			}
			operations.emplace_back(string_operator);
		}
		
		void process_oper(std::string::const_iterator& it)
		{
			logger::medium("state_machine", "operator branch", *it);
			if (std::isdigit(static_cast<unsigned char>(*it)))
			{
				logger::low("state_machine", "it's a number");
				currentState = state::number;
				auto isNotNumber = [](char ch){return !(std::isdigit(ch) || ch == '.');};
				std::string string_number = getElement(it, handlingString.cend(), isNotNumber);
				it += string_number.size() - 1;
				logger::low("state_machine", "number is", string_number);
				if (currentMod == mod::empty) result.push_back(string_number);
				else 
				{
					result.push_back('-' + string_number);
					currentMod = mod::empty;
				}
			}
			else if (*it == '(')
			{
				logger::low("state_machine", "it's open bracket");
				currentState = state::oper;
				operations.push_back("(");
			}
			else if (std::find(operators.begin(), operators.end(), *it) != operators.end())
			{
				if (*it == '*' || *it == '/') throw std::runtime_error(std::string("Bad operator in the begining. Got ") + *it);
				currentState = state::oper;
				logger::low("state_machine", "it's a operator");
				if (*it == '-') currentMod = mod::negative;
			}
			else if (*it != ' ')
			{
				auto isNotString = [](char ch){return !((ch >= 'a' && ch <='z') || (ch >= 'A' && ch <= 'Z'));};
				std::string string_function = getElement(it, handlingString.cend(), isNotString);
				it += string_function.size() - 1;
				if (string_function == "x")
				{
					logger::low("state_machine", "it's a undefined variable");
					currentState = state::number;
					result.emplace_back(string_function);
				}
				else
				{
					logger::low("state_machine", "it's a function");
					logger::low("state_machine", "function is", string_function);
					currentState = state::function;
					operations.push_back(string_function);
				}
			}
		}

		void process_function(std::string::const_iterator& it)
		{
			logger::medium("state_machine", "function branch");
			if (*it == '(')
			{
				logger::low("state_machine", "it's open bracket");
				currentState = state::oper;
				operations.push_back("(");
			}
			else throw std::runtime_error("Expected ( after function name");
		}

	public:
		state_machine(const std::string& str)
			:
				handlingString{str}
		{
			logger::medium("state_machine", "called state_machine constructor. Handling string:", handlingString);
		}
		equation process()
		{
			logger::medium("state_machine", "called state_machine::process");
			for (auto it = handlingString.cbegin() ; it != handlingString.cend() ; it++)
			{
				logger::low("state_machine", "Got", *it);
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
		yesza::logger::medium("make_equation", "called make_equation function");
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

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
		std::stack<std::string> arguments; // change by vector
		friend class state_machine;
	public:
		double operator()(double argument = 0)
		{
			logger::medium("equation()", "called operator()");
			if (arguments.empty()) return 0;
			auto arguments_buffer{arguments};
			std::stack<double> buffer;
			while (!arguments_buffer.empty())
			{
				logger::low("equation()", "got", arguments_buffer.top());
				if (!std::isdigit(arguments_buffer.top()[0]) && !std::isdigit(arguments_buffer.top()[1]) && arguments_buffer.top() != "x") // TODO change branches to map storage
				{
					logger::low("equation()", "perfom operation", arguments_buffer.top());
					if (arguments_buffer.top() == "+")
					{
						double first = buffer.top();
						buffer.pop();
						double second = buffer.top();
						buffer.top() = first + second;
					}
					else if (arguments_buffer.top() == "-")
					{
						double first = buffer.top();
						buffer.pop();
						double second = buffer.top();
						buffer.top() = first - second;
					}
					else if (arguments_buffer.top() == "*")
					{
						double first = buffer.top();
						buffer.pop();
						double second = buffer.top();
						buffer.top() = first * second;
					}
					else if (arguments_buffer.top() == "/")
					{
						double first = buffer.top();
						buffer.pop();
						double second = buffer.top();
						buffer.top() = second / first;
					}
					else if (arguments_buffer.top() == "sin")
					{
						double first = buffer.top();
						buffer.top() = std::sin(first);
					}
					else if (arguments_buffer.top() == "cos")
					{
						double first = buffer.top();
						buffer.top() = std::cos(first);
					}
					else if (arguments_buffer.top() == "tan")
					{
						double first = buffer.top();
						buffer.top() = std::tan(first);
					}
					else if (arguments_buffer.top() == "cotan")
					{
						double first = buffer.top();
						buffer.top() = std::tan(M_PI_2 - first);
					}
				}
				else
				{
					logger::low("equation()", "argument", arguments_buffer.top());
					buffer.push((arguments_buffer.top() == "x") ? argument :std::stod(arguments_buffer.top()));
				}
				arguments_buffer.pop();
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
		std::vector<std::string> outputString;
		std::string handlingString;
		const std::vector<char> operators{'+', '-', '*', '/'}; // TODO add ^

		const std::vector<std::string> functions{"sin", "cos", "tan", "cotan"}; // TODO add more functions

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
				logger::low("state_machine", "it's a number");
				currentState = state::number;
				auto isNotNumber = [](char ch){return !(std::isdigit(ch) || ch == '.');};
				std::string string_buffer = getElement(it, handlingString.cend(), isNotNumber);
				logger::low("state_machine", "number is", string_buffer);
				it += string_buffer.size() - 1;
				outputString.emplace_back(string_buffer);
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
				std::string buf = getElement(it, handlingString.cend(), isNotString);
				it += buf.size() - 1;
				if (buf == "x")
				{
					logger::low("state_machine", "it's undefined variable", buf);
					currentState = state::number;
					outputString.emplace_back(buf);
				}
				else
				{
					logger::low("state_machine", "it's a function");
					logger::low("state_machine", "function is", buf);
					operations.push_back(buf);
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
					outputString.push_back(operations.back());
					operations.pop_back();
				}
				operations.pop_back();
				return;
			}
			if (std::find(operators.begin(), operators.end(), *it) == operators.end()) throw std::runtime_error(std::string("Expected operator. Got ") + std::to_string(*it));
			logger::low("state_machine", "it's a operator", *it);
			currentState = state::oper;
			while (!operations.empty() && (operations.back() == std::string("*") || operations.back() == "/" 
					|| operations.back() == "sin"))
			{
				outputString.push_back(operations.back());
				operations.pop_back();
			}
			std::string string_buffer;
			string_buffer = *it;
			if (*it == '-')
			{
				if (currentMod == mod::negative) currentMod = mod::empty;
				else currentMod = mod::negative;
				string_buffer = "+";
			}
			operations.emplace_back(string_buffer);
		}
		
		void process_oper(std::string::const_iterator& it)
		{
			logger::medium("state_machine", "operator branch", *it);
			if (std::isdigit(static_cast<unsigned char>(*it)))
			{
				logger::low("state_machine", "it's a number");
				currentState = state::number;
				auto isNotNumber = [](char ch){return !(std::isdigit(ch) || ch == '.');};
				std::string string_buffer = getElement(it, handlingString.cend(), isNotNumber);
				it += string_buffer.size() - 1;
				logger::low("state_machine", "number is", string_buffer);
				if (currentMod == mod::empty) outputString.push_back(string_buffer);
				else 
				{
					outputString.push_back('-' + string_buffer);
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
				std::string string = getElement(it, handlingString.cend(), isNotString);
				it += string.size() - 1;
				if (string == "x")
				{
					logger::low("state_machine", "it's a undefined variable");
					currentState = state::number;
					outputString.emplace_back(string);
				}
				else
				{
					logger::low("state_machine", "it's a function");
					logger::low("state_machine", "function is", string);
					currentState = state::function;
					operations.push_back(string);
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
			equation buf;
			for (auto it = operations.rbegin() ; it != operations.rend() ; it++)
			{
				outputString.push_back(std::move(*it));
			}

			for (auto it = outputString.rbegin() ; it != outputString.rend() ; it++)
			{
				logger::low("state_machine", "Argument", *it);
				buf.arguments.push(std::move(*it));
			}
			
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

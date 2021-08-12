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

namespace yesza
{
	class expression // TODO add functions supporting
	{
	private:
		std::stack<std::string> arguments;

		friend expression make_equation(std::string str);
	public:
		double operator()(double argument)
		{
			if (arguments.empty()) return 0;
			std::stack<double> buffer;
			while (!arguments.empty())
			{
				std::cout << arguments.top() << std::endl;
				if (!std::isdigit(arguments.top()[0]) && !std::isdigit(arguments.top()[1])) // TODO change branches to map storage
				{
					if (arguments.top() == "+")
					{
						double first = buffer.top();
						buffer.pop();
						double second = buffer.top();
						buffer.top() = first + second;
					}
					else if (arguments.top() == "-")
					{
						double first = buffer.top();
						buffer.pop();
						double second = buffer.top();
						buffer.top() = first - second;
					}
					else if (arguments.top() == "*")
					{
						double first = buffer.top();
						buffer.pop();
						double second = buffer.top();
						buffer.top() = first * second;
					}
					else if (arguments.top() == "/")
					{
						double first = buffer.top();
						buffer.pop();
						double second = buffer.top();
						buffer.top() = second / first;
					}
					else if (arguments.top() == "sin")
					{
						double first = buffer.top();
						buffer.top() = std::sin(first);
					}
					else if (arguments.top() == "cos")
					{
						double first = buffer.top();
						buffer.top() = std::cos(first);
					}
					else if (arguments.top() == "tan")
					{
						double first = buffer.top();
						buffer.top() = std::tan(first);
					}
					else if (arguments.top() == "cotan")
					{
						double first = buffer.top();
						buffer.top() = std::tan(M_PI_2 - first);
					}
				}
				else
				{
					buffer.push(std::stod(arguments.top()));
				}
				arguments.pop();
			}
			std::cout << std::endl;
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
			if (std::isdigit(static_cast<unsigned char>(*it)))
			{
				currentState = state::number;
				auto isNotNumber = [](char ch){return !(std::isdigit(ch) || ch == '.');};
				std::string string_buffer = getElement(it, handlingString.cend(), isNotNumber);
				it += string_buffer.size() - 1;
				outputString.emplace_back(string_buffer);
			}
			else if (std::find(operators.begin(), operators.end(), *it) != operators.end())
			{
				if (*it == '*' || *it == '/') throw std::runtime_error(std::string("Bad operator in the begining. Got ") + *it);
				currentState = state::oper;
				std::cout << "Got operator in the begining: " << *it << std::endl;
				if (*it == '-') currentMod = mod::negative;
			}
			else if (*it == '(')
			{
				currentState = state::oper;
				std::cout << "Got block open " << *it << std::endl;
				operations.push_back("(");
			}
			else if (*it != ' ')
			{
				currentState = state::function;
				auto isNotString = [](char ch){return !(ch >= 'a' && ch <='z' || ch >= 'A' && ch <= 'Z');};
				std::string buff = getElement(it, handlingString.cend(), isNotString);
				it += buff.size() - 1;
				std::cout << "Got string: " << buff << std::endl;
				operations.push_back(buff);
			}
		}

		void process_number(std::string::const_iterator& it)
		{
			if (*it == ')')
			{
				std::cout << "Got block close" << std::endl;
				while(operations.back() != "(")
				{
					outputString.push_back(operations.back());
					operations.pop_back();
				}
				operations.pop_back();
				return;
			}
			if (std::find(operators.begin(), operators.end(), *it) == operators.end()) throw std::runtime_error(std::string("Expected operator. Got ") + std::to_string(*it));
			currentState = state::oper;
			std::cout << "Got operator: " << *it << std::endl;
			std::cout << operations.size() << std::endl;
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
			if (std::isdigit(static_cast<unsigned char>(*it)))
			{
				currentState = state::number;
				auto isNotNumber = [](char ch){return !(std::isdigit(ch) || ch == '.');};
				std::string string_buffer = getElement(it, handlingString.cend(), isNotNumber);
				it += string_buffer.size() - 1;
				std::cout << "Got number: " << string_buffer << std::endl;
				if (currentMod == mod::empty) outputString.push_back(string_buffer);
				else 
				{
					outputString.push_back('-' + string_buffer);
					currentMod = mod::empty;
				}
			}
			else if (*it == '(')
			{
				currentState = state::oper;
				std::cout << "Got block open " << *it << std::endl;
				operations.push_back("(");
			}

			else if (*it != ' ')
			{
				auto isNotString = [](char ch){return !(ch >= 'a' && ch <='z' || ch >= 'A' && ch <= 'Z');};
				std::string string = getElement(it, handlingString.cend(), isNotString);
				it += string.size() - 1;
				std::cout << "Got string: " << string << std::endl;
				operations.push_back(string);
				currentState = state::function;
			}
		}

		void process_function(std::string::const_iterator& it)
		{
			if (*it == '(')
			{
				currentState = state::oper;
				std::cout << "Got block open " << *it << std::endl;
				operations.push_back("(");
			}
			else throw std::runtime_error("Expected ( after function name");
		}

	public:
		state_machine(const std::string& str)
			:
				handlingString{str}
		{
		}
		std::vector<std::string> process()
		{
			for (auto it = handlingString.cbegin() ; it != handlingString.cend() ; it++)
			{
				std::cout << "Current ch: " << *it << std::endl;
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
			std::cout << "Got: ";
			while(!operations.empty())
			{
				std::string str = operations.back();
				std::cout << str << ' ';
				outputString.push_back(operations.back());
				operations.pop_back();
			}
			std::cout << std::endl;
			return outputString;
		}
	};
	inline expression make_equation(std::string equation)
	{
		state_machine sm(equation);
		auto outputString = sm.process();
		
		expression buf;
		while(!outputString.empty())
		{
			buf.arguments.push(outputString.back());
			std::cout << outputString.back() << ' ';
			outputString.pop_back();
		}
			std::cout << std::endl;
		
		return buf;
	}
	inline double count(std::string equation)
	{
		return make_equation(equation)(0);
	}

}

#endif //YESZA_RPN_HPP

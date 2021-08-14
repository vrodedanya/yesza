#ifndef YESZA_LOGGER_HPP
#define YESZA_LOGGER_HPP

#include <iostream>
#include <string>
#include <vector>

namespace yesza
{
	class logger
	{
	public:
		enum class Priority
		{
			LOW,
			MEDIUM,
			HIGH
		};
	private: // MEMBERS
		static Priority prior;
		static std::string prevSender;
		static bool on;

		template <typename T, typename... AArgs>
		static void rec_log(T&& value, AArgs&&... aargs) noexcept
		{
			std::cerr << value << ' ';
			rec_log(std::forward<AArgs>(aargs)...);
		}
		template <typename T>
		static void rec_log(T&& value) noexcept
		{
			std::cerr << value << std::endl;
		}

	private: // METHODS
	public: // METHODS
		
		template <typename... AArgs>
 		static void log(const std::string& sender, Priority priority, AArgs&&... aargs) noexcept
		{
 			if (on && priority >= prior)
			{
				if (sender != prevSender)
				{
					std::cerr << "[LOGGER]: from " << sender << std::endl;
					prevSender = sender;
				}
				std::cout << " -> ";
				rec_log(std::forward<AArgs>(aargs)...);
			// REC
			}
		}

		template <typename... AArgs>
 		static void low(const std::string& sender, AArgs&&... aargs) noexcept
		{
			log(sender, Priority::LOW, std::forward<AArgs>(aargs)...);
		}

		template <typename... AArgs>
 		static void medium(const std::string& sender, AArgs&&... aargs) noexcept
		{
			log(sender, Priority::MEDIUM, std::forward<AArgs>(aargs)...);
		}

		template <typename... AArgs>
 		static void high(const std::string& sender, AArgs&&... aargs) noexcept
		{
			log(sender, Priority::HIGH, std::forward<AArgs>(aargs)...);
		}

		static inline void turnOn() noexcept {on = true; }
	
		static inline void turnOff() noexcept {on = false; }

		static inline void setPriority(Priority newPriority) noexcept {prior = newPriority; }

		static inline Priority priority() noexcept {return prior; }

		static inline bool isOn() noexcept {return on; }
	};

	template <typename T>
	std::ostream& operator << (std::ostream& os, const std::vector<T>& container)
	{
		for (const auto& elem : container)
		{
			os << '[' << elem << "] ";
		}
		return os;
	}
}

#endif // YESZA_LOGGER_HPP

#include "logger.hpp"

yesza::logger::Priority yesza::logger::prior = yesza::logger::Priority::HIGH;
std::string yesza::logger::prevSender = "";
bool yesza::logger::on = true;


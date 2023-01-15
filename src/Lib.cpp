#include "Lib.h"
#include <string>

std::string Lib::welcome(const std::string &user) const noexcept {
  return std::string(WELCOME_MESSAGE) + " " + user;
}

#include "Lib.h"
#include <string>

std::string welcome(const std::string &user) const noexcept {
    return WELCOME_MESSAGE + "\t" + user;
}

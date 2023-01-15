#pragma once

#include <string_view>
#include <string>

class Lib
{
    static constexpr std::string_view WELCOME_MESSAGE = "Welcome to taste of HFT";
public:
 [[nodiscard]] std::string welcome(const std::string &user) const noexcept;
};

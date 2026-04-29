#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include "KeyCodes.h"

class Input;

class InputMapping {
public:
    void Bind(const std::string& action, Key key);

    bool IsActionDown(const Input& input, const std::string& action) const;
    bool IsActionPressed(const Input& input, const std::string& action) const;
    bool IsActionReleased(const Input& input, const std::string& action) const;

private:
    std::unordered_map<std::string, std::vector<Key>> m_Map;
};

#include "InputMapping.h"
#include "Input.h"

void InputMapping::Bind(const std::string& action, Key key) {
    m_Map[action].push_back(key);
}

bool InputMapping::IsActionDown(const Input& input, const std::string& action) const {
    auto it = m_Map.find(action);
    if (it == m_Map.end()) return false;

    for (Key key : it->second) {
        if (input.IsKeyDown(key))
            return true;
    }
    return false;
}

bool InputMapping::IsActionPressed(const Input& input, const std::string& action) const {
    auto it = m_Map.find(action);
    if (it == m_Map.end()) return false;

    for (Key key : it->second) {
        if (input.IsKeyPressed(key))
            return true;
    }
    return false;
}

bool InputMapping::IsActionReleased(const Input& input, const std::string& action) const {
    auto it = m_Map.find(action);
    if (it == m_Map.end()) return false;

    for (Key key : it->second) {
        if (input.IsKeyReleased(key))
            return true;
    }
    return false;
}
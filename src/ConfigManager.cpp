#include <ConfigManager.hpp>

void ConfigManager::ReplaceInString(
    const std::string& replacement, std::string& textStr,
    std::int64_t start, std::int64_t end
) noexcept {
    textStr.replace(start, end - start, replacement);
}

std::string ConfigManager::GetLineFromString(
    const std::string& textString,
    std::int64_t start, std::int64_t end
) noexcept {
    std::string foundLine;
    foundLine = std::string(textString.begin() + start, textString.begin() + end);

    return foundLine;
}

std::pair<std::int64_t, std::int64_t> ConfigManager::FindInString(
    const std::string& textStr, const std::string& searchItem
) noexcept {
    auto start = std::search(
        textStr.begin(), textStr.end(), searchItem.begin(), searchItem.end()
    );

    std::string::const_iterator end;

    if (start != textStr.end())
        end = std::find(start, textStr.end(), '\n');

    std::int64_t startPos = -1;
    std::int64_t endPos = -1;

    if (start != textStr.end() && end != textStr.end()) {
        startPos = std::distance(textStr.begin(), start);
        endPos = std::distance(textStr.begin(), end) + 1;
    }

    return { startPos, endPos };
}

std::string ConfigManager::FileToString(const char* fileName) noexcept {
    std::ifstream input(fileName, std::ios_base::in);

    std::string buffer;
    std::string str;
    while (std::getline(input, buffer))
        str.append(buffer + "\n"s);

    return str;
}

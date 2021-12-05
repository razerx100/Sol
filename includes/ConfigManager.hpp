#ifndef __CONFIG_MANAGER_HPP__
#define __CONFIG_MANAGER_HPP__
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>

using namespace std::string_literals;

class ConfigManager {
public:
    template<typename T>
    [[nodiscard]]
    static T ReadType(
        const char* fileName, const char* typeName, T defaultType,
        const std::vector<const char*>& typeNames
    ) noexcept {
        T type = defaultType;

        std::string textStr = FileToString(fileName);

        if (!textStr.empty()) {
            auto [start, end] = FindInString(textStr, typeName);
            if (start != -1 && end != -1) {
                std::string findLine = GetLineFromString(textStr, start, end);
                type = FindTypeFromString(findLine, defaultType, typeNames);
            }
        }

        return type;
    }

    template<typename T>
    static void SaveName(
        const char* fileName, const char* typeName, T type,
        const std::vector<const char*>& typeNames
    ) noexcept {
        std::string textStr = FileToString(fileName);
        std::ios_base::openmode openFLag = std::ios_base::out;
        std::string newValue =
            typeName + " = "s + typeNames[static_cast<std::uint32_t>(type)];

        if (!textStr.empty()) {
            auto [start, end] = FindInString(textStr, typeName);
            if (start != -1 && end != -1)
                ReplaceInString(newValue, textStr, start, end);
            else
                textStr.append(newValue);
        }
        else
            textStr.append(newValue);

        std::ofstream out(fileName, openFLag);
        out << textStr;
    }

private:
    template<typename T>
    [[nodiscard]]
    static T FindTypeFromString(
        const std::string& str, T defaultType, const std::vector<const char*>& searchItems
    ) noexcept {
        T type = defaultType;
        for (std::uint32_t index = 0u; index < searchItems.size(); ++index) {
            std::string temp = searchItems[index];
            auto result = std::search(str.begin(), str.end(), temp.begin(), temp.end());
            if (result != str.end()) {
                type = static_cast<T>(index);
                break;
            }
        }

        return type;
    }

private:
    static void ReplaceInString(
        const std::string& replacement, std::string& textStr,
        std::int64_t start, std::int64_t end
    ) noexcept;

    [[nodiscard]]
    static std::string GetLineFromString(
        const std::string& textString,
        std::int64_t start, std::int64_t end
    ) noexcept;

    [[nodiscard]]
    static std::pair<std::int64_t, std::int64_t> FindInString(
        const std::string& textStr, const std::string& searchItem
    ) noexcept;

    [[nodiscard]]
    static std::string FileToString(const char* fileName) noexcept;
};
#endif

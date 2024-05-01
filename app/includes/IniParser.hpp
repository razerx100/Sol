#ifndef INI_PARSER_HPP_
#define INI_PARSER_HPP_
#include <fstream>
#include <unordered_map>
#include <vector>
#include <string>

class IniParser {
    using ValueMap = std::unordered_map<std::string, std::string>;
    using Sections = std::unordered_map<std::string, ValueMap>;

public:
    IniParser(std::wstring fileName);

    void Parse() noexcept;
    void AddOrUpdateValue(
        const std::string& sectionName, std::string name, std::string value
    ) noexcept;
    void AddSection(std::string sectionName) noexcept;
    void RemoveSection(const std::string& sectionName) noexcept;
    void RemoveValue(
        const std::string& sectionName, const std::string& name
    ) noexcept;
    void WriteBack() noexcept;

    [[nodiscard]]
    bool DoesValueExist(
        const std::string& keyName, const std::string& sectionName = ""
    ) const noexcept;
    [[nodiscard]]
    std::string GetValue(
        const std::string& keyName, const std::string& sectionName = ""
    ) const noexcept;

private:
    [[nodiscard]]
    std::string Strip(const std::string& str, char input) const noexcept;
    [[nodiscard]]
    std::vector<std::string> Split(const std::string& str, char delimiter) const noexcept;
    [[nodiscard]]
    bool IsComment(const std::string& input) const noexcept;
    [[nodiscard]]
    std::string RemoveComment(const std::string& input) const noexcept;

private:
    Sections m_sections;
    std::wstring m_fileName;
};
#endif

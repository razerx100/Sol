#ifndef INI_PARSER_HPP_
#define INI_PARSER_HPP_
#include <fstream>
#include <unordered_map>
#include <vector>
#include <string>
#include <algorithm>

class IniParser
{
    using ValueMap = std::unordered_map<std::string, std::string>;
    using Sections = std::unordered_map<std::string, ValueMap>;

public:
    IniParser(std::wstring fileName);

    void Parse() noexcept;
    void AddOrUpdateValue(
        const std::string& sectionName, std::string name, std::string value
    ) noexcept;
    void AddSection(std::string sectionName) noexcept;
    void RemoveSection(const std::string& sectionName) noexcept
    {
        m_sections.erase(sectionName);
    }
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
    static std::string Strip(const std::string& str, char input) noexcept;
    [[nodiscard]]
    static std::vector<std::string> Split(const std::string& str, char delimiter) noexcept;
    [[nodiscard]]
    static bool IsComment(const std::string& input) noexcept
    {
        return input.find(';') != std::string::npos;
    }
    [[nodiscard]]
    static std::string RemoveComment(const std::string& input) noexcept
    {
        size_t commentStartPosition = input.find(';');

        return input.substr(0u, commentStartPosition);
    }

private:
    Sections     m_sections;
    std::wstring m_fileName;

public:
    IniParser(const IniParser& other) noexcept
        : m_sections{ other.m_sections }, m_fileName{ other.m_fileName }
    {}
    IniParser& operator=(const IniParser& other) noexcept
    {
        m_sections = other.m_sections;
        m_fileName = other.m_fileName;

        return *this;
    }

    IniParser(IniParser&& other) noexcept
        : m_sections{ std::move(other.m_sections) }, m_fileName{ std::move(other.m_fileName) }
    {}
    IniParser& operator=(IniParser&& other) noexcept
    {
        m_sections = std::move(other.m_sections);
        m_fileName = std::move(other.m_fileName);

        return *this;
    }
};
#endif

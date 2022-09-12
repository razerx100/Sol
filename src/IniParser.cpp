#include <IniParser.hpp>
#include <algorithm>
#include <exception>

IniParser::IniParser(std::wstring fileName)
    : m_fileName{ std::move(fileName) } {
    if (!m_fileName.ends_with(L".ini"))
        throw std::runtime_error("Extention isn't .ini");
}

void IniParser::Parse() noexcept {
    std::ifstream inputFile(m_fileName);
    std::string tempLine;
    std::string currentSection;

    while (std::getline(inputFile, tempLine)) {
        if (!std::empty(tempLine) && IsComment(tempLine))
            tempLine = RemoveComment(tempLine);

        std::string strippedLine = Strip(tempLine, ' ');

        if (std::size(strippedLine) >= 3u &&
            strippedLine.front() == '[' && strippedLine.back() == ']') {
            std::string sectionName =
                strippedLine.substr(1u, std::size(strippedLine) - 2u);

            m_sections.emplace(sectionName, ValueMap());

            currentSection = std::move(sectionName);
        }
        else {
            if (!std::empty(strippedLine)) {
                std::vector<std::string> words = Split(strippedLine, '=');

                if (!std::empty(words) && std::size(words) == 2u) {
                    ValueMap& valueMap = m_sections[currentSection];

                    valueMap.emplace(
                        std::move(words.front()), std::move(words.back())
                    );
                }
            }
        }
    }
}

void IniParser::AddOrUpdateValue(
    const std::string& sectionName, std::string name, std::string value
) noexcept {
    m_sections[sectionName].insert_or_assign(std::move(name), std::move(value));
}

void IniParser::AddSection(std::string sectionName) noexcept {
    if (!m_sections.contains(sectionName))
        m_sections.emplace(std::move(sectionName), ValueMap());
}

void IniParser::RemoveSection(const std::string& sectionName) noexcept {
    m_sections.erase(sectionName);
}

void IniParser::RemoveValue(
    const std::string& sectionName, const std::string& name
) noexcept {
    m_sections[sectionName].erase(name);
}

bool IniParser::DoesValueExist(
    const std::string& keyName, const std::string& sectionName
) const noexcept {
    auto foundSection = m_sections.find(sectionName);
    if (foundSection != std::end(m_sections)) {
        auto foundValue = foundSection->second.find(keyName);
        if (foundValue != std::end(foundSection->second))
            return true;
    }

    return false;
}

std::string IniParser::GetValue(
    const std::string& keyName, const std::string& sectionName
) const noexcept {
    std::string result;

    auto foundSection = m_sections.find(sectionName);
    if (foundSection != std::end(m_sections)) {
        auto foundValue = foundSection->second.find(keyName);
        if (foundValue != std::end(foundSection->second))
            result = foundValue->second;
    }

    return result;
}

void IniParser::WriteBack() noexcept {
    std::vector<std::string> sectionNames;

    std::transform(
        std::begin(m_sections), std::end(m_sections),
        std::back_inserter(sectionNames),
        [](const std::pair<std::string, ValueMap>& section) -> std::string {
            return section.first;
        }
    );

    std::sort(
        std::begin(sectionNames), std::end(sectionNames)
    );

    std::ofstream outputFile(m_fileName, std::ios_base::trunc);
    bool firstLine = true;
    for (const std::string& sectionName : sectionNames) {
        const ValueMap& valueMap = m_sections[sectionName];

        if (firstLine)
            firstLine = false;
        else
            outputFile << "\n";

        if (!std::empty(sectionName))
            outputFile << '[' + sectionName + ']' << "\n";

        std::transform(
            std::begin(valueMap), std::end(valueMap),
            std::ostream_iterator<std::string>(outputFile, "\n"),
            [](const std::pair<std::string, std::string>& valueMap) -> std::string {
                return valueMap.first + " = " + valueMap.second;
            }
        );
    }
}

std::string IniParser::Strip(const std::string& str, char input) const noexcept {
    if (!std::empty(str)) {
        size_t startingPointer = 0u;
        size_t endingPointer = std::size(str) - 1u;
        while (str[startingPointer] == input || str[endingPointer] == input) {
            if (str[startingPointer] == input)
                ++startingPointer;
            if (str[endingPointer] == input)
                --endingPointer;
        }

        return str.substr(startingPointer, endingPointer - startingPointer + 1u);
    }

    return str;
}

std::vector<std::string> IniParser::Split(
    const std::string& str, char delimiter
) const noexcept {
    std::vector<std::string> output;

    std::size_t startingPosition = 0u;
    for (size_t found = str.find(delimiter);
        found != std::string::npos;
        found = str.find(delimiter, found)) {

        output.emplace_back(Strip(str.substr(startingPosition, found), ' '));

        startingPosition = ++found;
    }
    if (!std::empty(str))
        output.emplace_back(Strip(str.substr(startingPosition + 1u), ' '));

    return output;
}

bool IniParser::IsComment(const std::string& input) const noexcept {
    return input.find(';') != std::string::npos;
}

std::string IniParser::RemoveComment(const std::string& input) const noexcept {
    size_t commentStartPosition = input.find(';');

    return input.substr(0u, commentStartPosition);
}

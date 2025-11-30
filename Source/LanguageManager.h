#pragma once

#include <string>
#include <unordered_map>

class LanguageManager {
public:
    LanguageManager() = default;
    ~LanguageManager() = default;

    bool Load(const std::string& langCode, const std::string& basePath);

    const std::string& Get(const std::string& key) const;
    const std::string& GetCurrentLang() const { return mCurrentLang; }

private:
    std::unordered_map<std::string, std::string> mStrings;
    std::string mCurrentLang;
    mutable std::string mFallback;

    static std::string Trim(const std::string& str);
};

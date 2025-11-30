#include "LanguageManager.h"
#include <fstream>
#include <algorithm>
#include <cctype>

namespace {
    std::string RemoveCarriageReturn(const std::string& s) {
        if (!s.empty() && s.back() == '\r') {
            return s.substr(0, s.size() - 1);
        }
        return s;
    }
}

bool LanguageManager::Load(const std::string& langCode, const std::string& basePath) {
    mStrings.clear();
    mCurrentLang = langCode;

    std::string filePath = basePath;
    if (!filePath.empty() && filePath.back() != '/' && filePath.back() != '\\') {
        filePath += "/";
    }
    filePath += langCode + ".txt";

    std::ifstream file(filePath);
    if (!file.is_open()) {
        mFallback = "";
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        line = RemoveCarriageReturn(line);
        if (line.empty() || line[0] == '#') continue;
        auto pos = line.find('=');
        if (pos == std::string::npos) continue;
        std::string key = Trim(line.substr(0, pos));
        std::string value = Trim(line.substr(pos + 1));
        if (!key.empty()) {
            mStrings[key] = value;
        }
    }
    return true;
}

const std::string& LanguageManager::Get(const std::string& key) const {
    auto it = mStrings.find(key);
    if (it != mStrings.end()) {
        return it->second;
    }
    mFallback = key;
    return mFallback;
}

std::string LanguageManager::Trim(const std::string& str) {
    size_t start = 0;
    while (start < str.size() && std::isspace(static_cast<unsigned char>(str[start]))) {
        ++start;
    }
    size_t end = str.size();
    while (end > start && std::isspace(static_cast<unsigned char>(str[end - 1]))) {
        --end;
    }
    return str.substr(start, end - start);
}

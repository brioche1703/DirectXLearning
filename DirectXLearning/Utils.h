#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <map>

std::vector<std::string> TokenizeQuoted(const std::string& input);

std::wstring ToWide(const std::string& narrow);
std::string ToNarrow(const std::wstring& wide);

template<class Iter>
void SplitStringIter(const std::string& s, const std::string& delim, Iter out) {
	if (delim.empty()) {
		*out++ = s;
	}
	else {
		size_t a = 0;
		size_t b = s.find(delim);
		for (; b != std::string::npos;
			a = b + delim.length(), b = s.find(delim, a)) {
			*out++ = std::move(s.substr(a, b - a));
		}
		*out++ = std::move(s.substr(a, s.length() - a));
	}
}

std::vector<std::string> SplitString(const std::string& s, const std::string& delim);

bool StringContains(std::string_view haystack, std::string_view needle);

std::string RemoveFileExtension(const std::string& s);

template<class T>
std::string GenerateUID(const std::string id, const std::map<std::string, T> map) {
	std::string newId = id;
	std::string idAux = id;
	auto exist = map.find(newId);
	int idN = 1;
	while (exist != map.end()) {
		idN++;
		newId = idAux + std::to_string(idN);
		exist = map.find(newId);
	}
	return newId;
}

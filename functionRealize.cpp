#include "cmulator.h"

using namespace std;
	//ai°ïÐ´µÄsplitº¯Êý
	vector<string> split(string_view str, char delimiter) {
		vector<string> result;
		size_t start = 0;
		size_t pos;
		while ((pos = str.find(delimiter, start)) != string_view::npos) {
			result.emplace_back(str.substr(start, pos - start));
			start = pos + 1;
		}
		result.emplace_back(str.substr(start));
		return result;
	}
	list<string> split_list(string_view str, char delimiter) {
		list<string> result;
		size_t start = 0;
		size_t pos;
		while ((pos = str.find(delimiter, start)) != string_view::npos) {
			result.emplace_back(str.substr(start, pos - start));
			start = pos + 1;
		}
		result.emplace_back(str.substr(start));
		return result;
	}

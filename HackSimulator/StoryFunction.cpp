/*
|	HackSimulator v0.0.5
|
|	StoryFunction.cpp
	this cpp is for some Story functions
	这个文件用于保存剧情数据和展示剧情
|
|	https://github.com/FlixKlein/HackSimulator
|	https://gitee.com/rosemarychn/HackSimulator
|	License : MIT
|	Personal Blog : https://undertopia.top
*/
#include "hacksimulator.h"
#include "UIManager.h"

namespace Story {
	std::string load_string_from_file(const std::string& name) {
		std::ifstream file(name);
		if (!file.is_open()) {
			return "";
		}
		std::stringstream buffer;
		buffer << file.rdbuf();
		std::string content = buffer.str();

		if (file.fail() && !file.eof()) {
			return "";
		}
		return content;
	}
	void init_story_from_file() {
		for (auto& [num, content] : all_story_map) {
			content = load_string_from_file("prolouge" + std::to_string(num) + ".txt");
		}
	}
	std::unordered_map<int,std::string> all_story_map = {
	{0, ""},
	{1,"ewwwwwwwdfccccccccccc" },
	{2,"rfrefrefrf" }
	};
	void show_the_background() {
		cls();
		//type_text(all_story_map[0], 1);
		show_story(0);
	}

	void read_prolouge(int num) {
		show_story(num);
	}
}
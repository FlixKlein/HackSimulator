/*
|	HackSimulator v0.0.5
|
|	TypingFunction.cpp
	this cpp is for some Typing functions
	这个文件用于一些打印文字相关函数
|
|	https://github.com/FlixKlein/HackSimulator
|	https://gitee.com/rosemarychn/HackSimulator
|	License : MIT
|	Personal Blog : https://undertopia.top
*/
#include "hacksimulator.h"
#include "UIManager.h"
using namespace std;
void show_the_logo() {
	type_text("----------------------------------------------------------------------------", 0);
	type_text("|   #          #             #                 ####        #         #     |", 0);
	type_text("|   #          #           #   #             ##            #       #       |", 0);
	type_text("|   #          #          #     #          ##              #     #         |", 0);
	type_text("|   #          #         #       #        ##               #   #           |", 0);
	type_text("|   #  ##  ##  #        #  # # #  #       ##               # #             |", 0);
	type_text("|   #          #       #           #      ##               # #             |", 0);
	type_text("|   #          #      #             #      ##              #   #           |", 0);
	type_text("|   #          #     #               #       ##            #     #         |", 0);
	type_text("|   #          #     #               #         ####        #       ####    |", 0);
	type_text("|                                                                          |", 0);
	type_text("|                 S    I    M    U    L    A    T    O    R                |", 0);
	type_text("|                                黑客模拟                                  |", 0);
	type_text("|                 https://github.com/FlixKlein/HackSimulator               |", 0);
	type_text("|                 Blog: https://www.undertopia.top                         |", 0);
	type_text("|                 LISENCE: MIT                                             |", 0);
	type_text("|                 Author: Rosemary                                         |", 0);
	type_text("|                 some functions helped by aistudio google                 |", 0);
	type_text("----------------------------------------------------------------------------", 0);
	type_text("...", 1000);
	return;
}
void type_text(const string& str, int delay) {
	for (const char c : str) {
		cout << c << flush;
		this_thread::sleep_for(chrono::milliseconds(delay));
	}
	cout << endl;
}
void show_story(const string& story) {
	// 1. 获取或创建窗口
	UIWindow* story_window = UIManager::get_instance().create_window<StoryWindow>(to_string(u8"剧情"));
	// 2. 配置窗口的内容和渲染方式
	story_window->initial_size = ImVec2(1080, 720);
	story_window->typewriter_full_text = to_utf8(story);
	story_window->typewriter_char_delay = 0.01f;
	story_window->typewriter_current_chars = 0;
	story_window->on_render = typewriter_renderer; // 使用我们之前写的打字机渲染器
	// 函数立刻返回，CMD提示符可以继续输入
	return;
}
void show_story(int chapter_id) {
	// 1. 获取或创建 StoryWindow
	StoryWindow* story_win = UIManager::get_instance().create_window<StoryWindow>(to_string(u8"剧情"));
	if (!story_win) return;

	// 2. 解锁并切换到该章节
	//    (这里的逻辑是：一旦被show，就自动解锁)
	story_win->unlock_chapter(chapter_id);
	story_win->switch_to_chapter(chapter_id,true);
}
void CenteredText(const std::string& text) {
	ImVec2 text_size = ImGui::CalcTextSize(text.c_str()); // 计算文本宽度和高度
	float window_width = ImGui::GetWindowSize().x;        // 获取当前窗口的宽度
	float text_width = text_size.x;

	// 计算起始X位置，使文本居中
	float text_start_x = (window_width - text_width) * 0.5f;

	// 确保文本不会跑到窗口左侧外面
	if (text_start_x < 0) {
		text_start_x = 0;
	}

	ImGui::SetCursorPosX(text_start_x); // 设置光标的X位置
	ImGui::Text("%s", text.c_str());     // 绘制文本
}

void typewriter_renderer(UIWindow& window) {
	ImGuiIO& io = ImGui::GetIO();
	window.typewriter_timer += io.DeltaTime;

	// 【核心修正】使用 while 循环处理
	// 如果上一帧卡了很久，一次性把所有“欠”的字符都补上
	while (window.typewriter_timer >= window.typewriter_char_delay) {
		if (window.typewriter_current_chars < window.typewriter_full_text.length()) {
			window.typewriter_current_chars++;
		}
		window.typewriter_timer -= window.typewriter_char_delay;
	}

	std::string text_to_show = window.typewriter_full_text.substr(0, window.typewriter_current_chars);;
	// 5. 将文本显示在可滚动的区域
	ImGui::BeginChild("Content", ImVec2(0, 0), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar);
	ImGui::TextWrapped("%s", text_to_show.c_str()); // TextWrapped 会自动换行
	ImGui::EndChild();
}
/*使用这个方法的指南
int cmd_show_story(Session&, const auto&) {
	// 1. 获取或创建窗口
	UIWindow* story_window = UIManager::get_instance().create_window("主线剧情");

	// 2. 设置要显示的完整文本
	story_window->typewriter_full_text = "公元2077年，夜之城。一个神秘的客户通过加密信道联系了你，要求你潜入荒坂塔，盗取一份传说中的芯片...";

	// 3. 重置打字机状态
	story_window->typewriter_current_chars = 0;
	story_window->typewriter_timer = 0.0f;

	// 4. 【核心】将“打字机画家”赋给这个窗口！
	story_window->on_render = typewriter_renderer;

	return 0;
}*/
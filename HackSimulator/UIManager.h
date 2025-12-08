/*
|	HackSimulator v0.0.5
|
|	UIManager.h
|	this is the UI-related header file of HackSimulator
|	这个是HackSimulator的UI相关头文件
|
|	https://github.com/FlixKlein/HackSimulator
|	https://gitee.com/rosemarychn/HackSimulator
|	License : MIT
|	Personal Blog : https://undertopia.top
*/
#pragma once
#include <string>
#include <vector>
#include <mutex>
#include <memory>
// 告诉SDL我们自己处理main函数，防止冲突
#define SDL_MAIN_HANDLED
#include "hacksimulator.h"
// 包含所有必要的头文件
#include "imgui.h"
#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_opengl3.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include "imgui_internal.h"
#include <iostream>
#include <cmath> // For std::abs, std::sin
#include <atomic>
#include <set> // set

//下面的UI几乎也都是aistudio写的，对ImGui比较熟悉的人可以看懂
//后面有时间再自己优化，aistudio写的比较简陋
class UIWindow {
public:
    UIWindow(const std::string& title);
    using RenderCallback = std::function<void(UIWindow&)>;//接受一个可渲染函数
    RenderCallback on_render;
    ImVec2 initial_size = ImVec2(1080, 720); // 默认初始大小
    bool size_has_been_set = false;
    // 【需求1 & 2】在主循环中被 UIManager 调用
    void render();
    // --- 通用接口，供命令调用 ---
    void clear_content();
	void add_line(const std::string& line);// 添加一行文本
    std::string typewriter_full_text;
    // 当前已经显示了多少个字符
    size_t typewriter_current_chars = 0;
    // 每个字符出现的间隔时间（单位：秒）
    float typewriter_char_delay = 0.01f; // 默认
    // 一个计时器，用于追踪距离上一个字符出现过了多久
    float typewriter_timer = 0.0f;
    std::string blinking_notification;
    // --- 数据存储 ---
    bool is_open = true; // ImGui会通过'x'按钮把它设为false
    std::string title;
    // 我们用一个通用的 vector<string> 来存储基于行的文本内容
    // 渲染函数可以决定如何使用它
    std::vector<std::string> content_lines;
    std::mutex content_mutex; // 用于保护 content_lines
    virtual ~UIWindow() = default;

private:

    bool scroll_to_bottom = false;
};

// UIManager.h

void typewriter_renderer(UIWindow& window);

class UIManager {
public:
    // 获取单例实例
    static UIManager& get_instance();

    //这里的T指的是派生窗口类型，也可能是父类
    template<typename T>
    T* create_window(const std::string& title) {
        std::lock_guard<std::mutex> lock(windows_mutex);
        auto it = windows.find(title);
        if (it != windows.end()) {
            return dynamic_cast<T*>(it->second.get());
        }
        auto new_window = std::make_unique<T>(title);
        T* ptr = new_window.get();
        windows[title] = std::move(new_window);
        return ptr;
    }

    UIWindow* find_window(const std::string& title);

    void close_window(const std::string& title);

    // 在GUI主循环中被调用
    void render_all();
    // 在GUI线程中初始化时调用
    void load_fonts();


private:
    UIManager() = default;
    ~UIManager() = default;
    UIManager(const UIManager&) = delete;
    UIManager& operator=(const UIManager&) = delete;
    ImFont* main_font = nullptr;
    std::mutex windows_mutex;
    // key是窗口标题，value是窗口对象
    std::map<std::string, std::unique_ptr<UIWindow>> windows;
};



class GUISystem {
public:
    GUISystem();
    ~GUISystem();

    // 启动GUI子系统，在新线程中运行
    void start();

    // 请求GUI子系统关闭
    void stop();

    // 检查GUI子系统是否正在运行
    bool is_running() const;

private:
    // GUI渲染循环的入口函数
    void run_gui_loop();

    std::thread gui_thread;
    std::atomic<bool> running_flag = { false };
};


// 2. 创建 StoryWindow 类，继承自 UIWindow
class StoryWindow : public UIWindow {
public:
    StoryWindow(const std::string& title);

    // --- 专门用于剧情的数据 ---

    // 存储所有已解锁的章节，key是章节ID
    std::set<int> unlocked_chapter_ids;

    // 当前正在查看的章节ID
    int current_chapter_id = -1; // -1 表示没有选中任何章节

    // --- 控制接口 ---

    /**
     * @brief 解锁一个新的章节并添加到列表中。
     * @param chapter 新的章节数据。
     */
    void unlock_chapter(int chapter_id);
    bool switch_to_chapter(int chapter_id,bool mode);
};

// 3. 我们需要一个新的、专门的渲染函数
void story_window_renderer(UIWindow& window);
/*
|	HackSimulator v0.0.6
|
|	UIManager.cpp
	this cpp implements UIManager and UIWindow
	这个文件实现UIManager和UIWindow
|
|	https://github.com/FlixKlein/HackSimulator
|	https://gitee.com/rosemarychn/HackSimulator
|	License : MIT
|	Personal Blog : https://undertopia.top
*/

#include "imgui_internal.h"
#include "UIManager.h"
#include "hacksimulator.h"



UIWindow::UIWindow(const std::string& title) : title(title) {
    // 默认的渲染函数：简单地逐行打印 content_lines
    on_render = [](UIWindow& window) {
        // 创建一个可滚动的子区域
        ImGui::BeginChild("Content", ImVec2(0, 0), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar);

        { // 加锁以安全地访问 content_lines
            std::lock_guard<std::mutex> lock(window.content_mutex);
            for (const auto& line : window.content_lines) {
                ImGui::TextUnformatted(line.c_str());
            }
        } // 锁在这里自动释放

        if (window.scroll_to_bottom) {
            ImGui::SetScrollHereY(1.0f);
            window.scroll_to_bottom = false;
        }
        ImGui::EndChild();
        };
}
void UIWindow::render() {
    if (!is_open) return;
    if (!size_has_been_set) {
        ImGui::SetNextWindowSize(initial_size, ImGuiCond_FirstUseEver);
        size_has_been_set = true;
    }
    ImGui::Begin(title.c_str(), &is_open);

    // --- 【需求2】在这里绘制闪烁提示 ---
    if (!blinking_notification.empty()) {
        // 1. 获取总时间
        float time = (float)ImGui::GetTime();

        // 2. 计算一个在 0.0 到 1.0 之间周期性变化的值
        //    std::sin(time * 5.0f) 会在 -1 和 1 之间震荡，*5.0f 是为了加快闪烁速度
        //    + 1.0f -> 0 到 2
        //    * 0.5f -> 0 到 1
        float alpha = (std::sin(time * 5.0f) + 1.0f) * 0.5f;

        // 3. 使用这个 alpha 值来设置文本的透明度
        //    颜色格式是 (R, G, B, A)，A就是透明度
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, alpha), "%s", blinking_notification.c_str());
        ImGui::Separator(); // 画一条分割线
    }

    // 调用可定制的渲染函数（这部分不变）
    if (on_render) {
        on_render(*this);
    }

    ImGui::End();
}
/*
浮动文字使用指南
void on_new_mail_received(const Mail& new_mail) {
    // 1. 找到邮件应用的窗口
    UIWindow* mail_window = UIManager::get_instance().find_window("邮件阅读器");

    if (mail_window) {
        // 2. 设置它的闪烁提示！
        mail_window->blinking_notification = ">> 您有一封新邮件 <<";

        // 也可以在这里调用 add_line 来更新邮件列表
        mail_window->add_line("发件人: " + new_mail.from);
    }
}```
当玩家点击进入这个窗口后，可以写一个逻辑来清除这个提示：`mail_window->blinking_notification.clear();`。
*/

void UIWindow::clear_content() {
    std::lock_guard<std::mutex> lock(content_mutex);
    content_lines.clear();
}

void UIWindow::add_line(const std::string& line) {
    std::lock_guard<std::mutex> lock(content_mutex);
    content_lines.push_back(line);
    scroll_to_bottom = true;
}



UIManager& UIManager::get_instance() {
    static UIManager instance;
    return instance;
}



UIWindow* UIManager::find_window(const std::string& title) {
    std::lock_guard<std::mutex> lock(windows_mutex);
    auto it = windows.find(title);
    if (it != windows.end()) {
        return it->second.get();
    }
    return nullptr;
}

void UIManager::close_window(const std::string& title) {
    std::lock_guard<std::mutex> lock(windows_mutex);
    windows.erase(title);
}

void UIManager::render_all() {
    // --- 1. 创建一个覆盖整个视口的“停靠空间”(Dockspace) ---
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace", nullptr, window_flags);
    ImGui::PopStyleVar(3);

    // 提交 Dockspace
    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.5f, 0.5f), ImGuiDockNodeFlags_None);

    // --- 2. 在 Dockspace 中渲染你所有的游戏窗口 ---
    //    这里的逻辑和之前几乎一样
    {
        std::lock_guard<std::mutex> lock(windows_mutex);

        for (auto& [title,window_ptr] : windows) {
            if (window_ptr->is_open) {
				window_ptr->render();
            }
        }
    }

    ImGui::End(); // 结束 Dockspace 窗口
}

void UIManager::load_fonts() {
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->Clear(); // 清空所有已有的字体

    const char* font_path = "NotoSansSC-Regular.ttf";
    float font_size = 30.0f;

    ImFontConfig font_config;
    font_config.PixelSnapH = true;
    const ImWchar* ranges = io.Fonts->GetGlyphRangesChineseSimplifiedCommon();
    this->main_font = io.Fonts->AddFontFromFileTTF(font_path, font_size, &font_config, ranges);
}

void GUISystem::run_gui_loop() {
    // =========================================================================
    // 1. 初始化 SDL (Simple DirectMedia Layer)
    // =========================================================================
    // 我们只需要视频和计时器子系统
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        // 如果初始化失败，在主CMD控制台打印错误，然后线程退出
        std::cerr << "[GUI THREAD ERROR] Failed to initialize SDL: " << SDL_GetError() << std::endl;
        this->running_flag = false;
        return;
    }

    // =========================================================================
    // 2. 设置 OpenGL 属性并创建窗口
    // =========================================================================

    // 告诉SDL我们想使用OpenGL 3.3 Core Profile。这是现代OpenGL的通用标准，
    // 能确保我们的程序在Windows, macOS, Linux上都有良好的兼容性。
    const char* glsl_version = "#version 330";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    // 设置窗口创建时的标志
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);

    // 创建窗口
    SDL_Window* window = SDL_CreateWindow(
        "HackSimulator - Display", // 窗口标题
        1980,                      // 初始宽度
        1060,                       // 初始高度
        window_flags
    );
    if (window == nullptr) {
        std::cerr << "[GUI THREAD ERROR] Failed to create SDL window: " << SDL_GetError() << std::endl;
        SDL_Quit();
        this->running_flag = false;
        return;
    }

    // 为窗口创建OpenGL上下文，这是我们绘图的“画布”
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    if (gl_context == NULL) {
        std::cerr << "[GUI THREAD ERROR] Failed to create OpenGL context: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        this->running_flag = false;
        return;
    }
    SDL_GL_MakeCurrent(window, gl_context);

    // 启用垂直同步 (V-Sync)，可以防止画面撕裂，让动画更平滑
    SDL_GL_SetSwapInterval(1);

    // =========================================================================
    // 3. 初始化 Dear ImGui
    // =========================================================================
    IMGUI_CHECKVERSION();
 

    // 2. 【核心】在初始化后端【之前】，调用我们新的 load_fonts
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // 启用键盘控制
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // 【可选，但强烈推荐】启用Docking功能
    // ... (ConfigFlags) ...
    ImGui::StyleColorsDark();

    // 【关键顺序】先初始化平台后端（SDL3）
    ImGui_ImplSDL3_InitForOpenGL(window, gl_context);

    // 【关键顺序】然后加载字体
    // 此时 ImGui 已经知道平台信息，但还没接触渲染器
    UIManager::get_instance().load_fonts();

    // 【关键顺序】最后初始化渲染后端（OpenGL3）
    // ImGui_ImplOpenGL3_Init 会自动处理字体纹理的上传
    ImGui_ImplOpenGL3_Init(glsl_version);

    // 初始化成功，设置运行标志
    this->running_flag = true;
    std::cout << "[GUI System] Display thread started successfully." << std::endl;

    // =========================================================================
    // 4. 主渲染循环
    // =========================================================================
    bool done = false;
    while (this->running_flag) {
        // --- 事件处理 ---
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL3_ProcessEvent(&event);
            if (event.type == SDL_EVENT_QUIT || (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(window)))
                this->running_flag = false;
        }

        // --- 开始新一帧 ---
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        // 【核心】只调用 UIManager 来渲染所有游戏相关的窗口
        UIManager::get_instance().render_all();

        // --- 渲染 ---
        ImGuiIO& io = ImGui::GetIO();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f); // 使用一个更深的背景色
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        SDL_GL_SwapWindow(window);
    }

    // =========================================================================
    // 5. 清理与关闭
    // =========================================================================
    std::cout << "[GUI System] Display thread shutting down..." << std::endl;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DestroyContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

// --- GUISystem 的公共接口实现 ---
GUISystem::GUISystem() {}

GUISystem::~GUISystem() {
    stop(); // 确保在 GUISystem 对象销毁时，线程被正确关闭
}

void GUISystem::start() {
    if (is_running()) return;

    // 【核心】创建一个新线程，让它去运行渲染循环
    // &GUISystem::run_gui_loop 是成员函数指针的语法
    // this 是传递给成员函数的对象实例
    gui_thread = std::thread(&GUISystem::run_gui_loop, this);
}

void GUISystem::stop() {
    running_flag = false; // 向GUI线程发送停止请求
    if (gui_thread.joinable()) {
        gui_thread.join(); // 等待GUI线程安全退出
    }
}

bool GUISystem::is_running() const {
    return running_flag;
}

StoryWindow::StoryWindow(const std::string& title) : UIWindow(title) {
    // 构造时，就为它指定专门的渲染器
    this->on_render = story_window_renderer;
}

void StoryWindow::unlock_chapter(int chapter_id) {
    unlocked_chapter_ids.insert(chapter_id);
}

bool StoryWindow::switch_to_chapter(int chapter_id,bool mode) {
    // 检查该章节是否已解锁
    if (unlocked_chapter_ids.count(chapter_id)) {
        current_chapter_id = chapter_id;

        // 从全局数据库中获取故事内容
        const auto& db = Story::all_story_map;
        auto it = db.find(chapter_id);
        if (it != db.end()) {
            this->typewriter_full_text = to_utf8(it->second); // 设置打字机文本
            if (mode == true)
            {
                // 重置打字机
                this->typewriter_current_chars = 0;
                this->typewriter_timer = 0.0f;
            }
            else {
                this->typewriter_current_chars = this->typewriter_full_text.length();
                this->typewriter_timer = 0.0f;
            }
        }

        return true;
    }
    return false;
}

void story_window_renderer(UIWindow& window) {
    // 我们需要将基类指针安全地转换回派生类指针，以便访问章节数据
    StoryWindow* story_window = dynamic_cast<StoryWindow*>(&window);
    if (!story_window) return; // 如果转换失败，则直接返回

    // 【核心：左右分栏布局】
    if (ImGui::BeginTable("StoryLayout", 2, ImGuiTableFlags_Resizable)) {

        // --- 第一列：左侧的章节列表 ---
        ImGui::TableSetupColumn("Chapters", ImGuiTableColumnFlags_WidthFixed, 150.0f); // 固定初始宽度
        ImGui::TableNextColumn();

        ImGui::BeginChild("ChapterList");
        ImGui::Text("Stories");
        ImGui::Separator();
        const auto& db = Story::all_story_map;
        for (int id : story_window->unlocked_chapter_ids) {
            auto it = db.find(id);
            if (it != db.end()) {
                std::string title = "prolouge" + std::to_string(id);
                if (ImGui::Selectable(title.c_str(), id == story_window->current_chapter_id)) {
                    story_window->switch_to_chapter(id,false);
                }
            }
        }
        ImGui::EndChild();

        // --- 第二列：右侧的剧情内容 ---
        ImGui::TableNextColumn();
        ImGui::BeginChild("ContentView");

        // 复用我们之前写的打字机逻辑！
        if (story_window->current_chapter_id != -1) {
            // (这里直接把 typewriter_renderer 的逻辑搬过来，并稍作修改)
            ImGuiIO& io = ImGui::GetIO();
            story_window->typewriter_timer += io.DeltaTime;

            while (story_window->typewriter_timer >= story_window->typewriter_char_delay) {
                if (story_window->typewriter_current_chars < story_window->typewriter_full_text.length()) {
                    story_window->typewriter_current_chars++;
                }
                story_window->typewriter_timer -= story_window->typewriter_char_delay;
            }

            std::string text_to_show = story_window->typewriter_full_text.substr(0, story_window->typewriter_current_chars);
            ImGui::TextWrapped("%s", text_to_show.c_str());

        }
        else {
            ImGui::Text("请从左侧选择一个章节进行阅读。");
        }

        ImGui::EndChild();

        ImGui::EndTable();
    }
}
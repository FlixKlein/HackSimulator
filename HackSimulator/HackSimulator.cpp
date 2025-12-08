/*
|	HackSimulator v0.0.4
|
|	main.cpp
|
|	https://github.com/FlixKlein/HackSimulator
|	https://gitee.com/rosemarychn/HackSimulator
|	License : MIT
|	Personal Blog : https://undertopia.top
*/
#include "hacksimulator.h"
#include "UIManager.h"
/* run this program using the console pauser or add your own getch, system("pause") or input loop */
std::vector<ComputerComponents::Computer> world_computers;
SessionManager session_manager;
CommandComponents::CommandProcessor processor(session_manager);
bool is_chinese;
int prolouge_num;
std::mutex world_mutex;
std::mutex cout_mutex;


int main() {
	GUISystem gui;
	gui.start();

	show_the_logo();
	Story::init_story_from_file();
	Init::init_game_from_json();

	int main_session_id = session_manager.create_session(&world_computers[0]);
	ComputerComponents::Session* session = session_manager.get_session(main_session_id);
	std::cout << "已连接到 " << session->current_computer->name << std::endl;
	std::string input;
	while (true) {
		std::cout << "[" << session->current_computer->name << " " << session->current_dir->show_path() << "]$ ";
		std::getline(std::cin, input);
		int status = processor.execute(*session, input);
		if (status == -1) {
			std::string filename;
			std::cout << "请输入存档文件名，不要有后缀：";
			std::getline(std::cin, filename);
			SerializeJson::save_world(world_computers, filename + ".json");
			std::cout << "请关闭图形界面窗口以退出程序" << std::endl;
			break;
		}
	}
	gui.stop();
	return 0;
}

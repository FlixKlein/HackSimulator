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
/* run this program using the console pauser or add your own getch, system("pause") or input loop */
std::vector<ComputerComponents::Computer> world_computers;
SessionManager session_manager;
CommandComponents::CommandProcessor processor(session_manager);
std::mutex world_mutex;
std::mutex cout_mutex;
int main() {
	show_the_logo();
	init_game_from_json();
	int main_session_id = session_manager.create_session(&world_computers[0]);
	ComputerComponents::Session* session = session_manager.get_session(main_session_id);
	std::cout << "已连接到 " << session->current_computer->name << std::endl;
	std::string input;
	while(true) {
		std::cout << "[" << session->current_computer->name << " " << session->current_dir->show_path() << "]$ ";
		std::getline(std::cin, input);
		        
		int status = processor.execute(*session,input);
		if (status == -1) {
			std::string filename;
			std::cout<<"请输入存档文件名，不要有后缀：";
			std::getline(std::cin,filename);
			SerializeJson::save_world(world_computers,filename + ".json");
		    break;
		}
	}
	return 0;
}

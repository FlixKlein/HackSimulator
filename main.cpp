#include <iostream>
#include "cmulator.h"
/* run this program using the console pauser or add your own getch, system("pause") or input loop */

int main() {
	std::vector<ComputerDomain::Computer> world_computers;
	world_computers.emplace_back("MainFrame","root","123456");
	world_computers.emplace_back("Webserve","root","1q2w3e4r");//这里初始化世界数据到时候我预先存在一个配置文件里
	//而且我突然发现ip不能每次都随机数生成，遂配置文件写任务需要的ip，其他的随机生成
	ComputerDomain::Session session;
	CommandDomain::CommandProcessor processor;
	session.connect_to(&world_computers[0]);
	session.change_dir(world_computers[0].get_root());
	std::cout << "欢迎来到HackSimulator！已连接到 " << session.current_computer->name << std::endl;
	std::string input;
	while(true) {
		std::cout << "[" << session.current_computer->name << " " << session.current_dir->show_path() << "]$ ";
		std::getline(std::cin, input);
		        
		int status = processor.execute(session, input);
		if (status == -1) {
		    break;
		}
	}
	return 0;
}

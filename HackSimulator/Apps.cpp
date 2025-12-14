/*
|	HackSimulator v0.0.6
|
|	Apps.cpp
	this cpp implements Apps related classes and functions
	这个文件用于实现应用相关类和函数
|
|	https://github.com/FlixKlein/HackSimulator
|	https://gitee.com/rosemarychn/HackSimulator
|	License : MIT
|	Personal Blog : https://undertopia.top
*/
#include "hacksimulator.h"
#include "UIManager.h"

std::unordered_map<std::string,std::string> app_hash_map = {
	{"help.exe" , simple_fnv_hash("help_hash_code","HackSimulator")},
	{"nano.exe" , simple_fnv_hash("nano_hash_code","HackSimulator")},
};
namespace Apps {
	using namespace ComputerComponents;
	using namespace FileSystem;
	using namespace CommandComponents;
	using namespace std;

	//Help
	Help::Help() {
		app_commands["help"] = [](Session& session, const vector<string>& args) -> int {
			int n = args.size();
			if (n == 1) {
				cout << "有关某个命令的详细信息，请键入 help 命令名" << endl;
				cout << "help\t\t显示特定指令帮助" << endl;
				cout << "ls\t\t显示当前目录下的所有文件和文件夹" << endl;
				cout << "mkdir\t\t创建空文件夹" << endl;
				cout << "cd\t\t进入特定目录" << endl;
				cout << "del\t\t删除文件或文件夹" << endl;
				cout << "pwd\t\t显示当前目录" << endl;
				cout << "cat\t\t显示可读文件内容" << endl;
				cout << "ipconfig\t\t显示当前网络信息" << endl;
				cout << "cp\t\t复制文件或者文件夹" << endl;
				cout << "read\t\t阅读剧情文本" << endl;
				cout << "user\t\t用户账户管理" << endl;
				cout << "ping\t\t测试与目标主机的连通性" << endl;
				cout << "passwd\t\t修改当前用户密码" << endl;
				cout << "login\t\t登录到账户" << endl;
				cout << "logout\t\t退出当前账户" << endl;
				cout << "touch\t\t创建空白文件" << endl;
				cout << "start\t\t启动一个后台会话" << endl;
				cout << "cl\t\t清空命令行窗口" << endl;
				cout << "exec\t\t后台会话运行命令" << endl;
				cout << "session\t\t显示线程" << endl;
				cout << "exit\t\t退出程序并显示关机选项" << endl;
			}
			else if (n > 2) {
				cout << "错误的用法！输入 help 或者 help 命令名" << endl;
			}
			else {
				search_help_from(args[1]);
			}
			return 0;
			};
	}
	int Help::execute(Session& session, const vector<string>& args) {
		if (args.empty()) {
			return 0;
		}
		const string name = args[0];
		auto it = app_commands.find(name);
		if (it != app_commands.end()) {
			return it->second(session, args);
		}
		return 1;
	}
	const string Help::get_hash() const {
		return app_hash_map["help.exe"];
	}
	//Mail
	
	//Nano
	Nano::Nano() {

	}
	int Nano::execute(Session& session, const vector<string>& args) {
		return 0;
	}
	const string Nano::get_hash() const {
		return app_hash_map["nano.exe"];
	}
}
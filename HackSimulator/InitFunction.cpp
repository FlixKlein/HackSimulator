/*
|	HackSimulator v0.0.6
|
|	InitFunction.cpp
	this cpp is for initialization functions
	这个文件用于初始化游戏的函数
|
|	https://github.com/FlixKlein/HackSimulator
|	https://gitee.com/rosemarychn/HackSimulator
|	License : MIT
|	Personal Blog : https://undertopia.top
*/
#include "hacksimulator.h"
#include "UIManager.h"

namespace Init {

	using namespace ComputerComponents;
	using namespace FileSystem;
	using namespace SerializeJson;

	void init_first_time() {
		world_computers.emplace_back("Fitzgerald", "root", "123456");//0
		world_computers.emplace_back("Otamass", "otamass", "6&82ywvYG1");//1
		world_computers.emplace_back("Webserve", "root", "1q2w3e4r");//这里初始化世界数据到时候我预先存在一个配置文件里

		//文件结构初始化
		for (auto& computer : world_computers) {
			Dir* root = computer.get_root();
			unique_ptr<Dir> bin = make_unique<Dir>("bin", root);
			unique_ptr<Dir> sys = make_unique<Dir>("sys", root);
			unique_ptr<Dir> home = make_unique<Dir>("home", root);
			root->add_dir(move(bin));
			root->add_dir(move(sys));
			root->add_dir(move(home));
			unique_ptr<File> help = make_unique<File>("help.exe", vector<string>{
				to_utf8("有关某个命令的详细信息，请键入 help 命令名"),
			});
			help->file_hash = app_hash_map["help.exe"];
			root->locate_dir_from_now("bin")->add_file(move(help));

			unique_ptr<File> nano = make_unique<File>("nano.exe");
			nano->file_hash = app_hash_map["nano.exe"];
			root->locate_dir_from_now("bin")->add_file(move(nano));
			unique_ptr<Net::NetNode> netnode = make_unique<Net::NetNode>(computer.get_ip(), computer.name);
			netnode->bind_host(&computer);
			netnodes[computer.get_ip()] = move(netnode);
		}
	}
	void init_game_from_json() {
		cls();
		string filename, tmp, lg;
		cout << "欢迎来到HackSimulator，\n输入new开始新游戏，输入load [存档名]以加载存档，不要输入文件后缀 \nWelcome to HackSimulator. \nInput 'new' to start a new game,while 'load [filename]' to load game from a saving file.No input the file extension :";
		getline(cin, tmp);
		if (tmp == "new") {
			cout << "游戏默认是中文，但是剧情提供了英文阅览，请输入你想阅览的剧情语言 'cn' | 'en'\nThe game is in Chinese by default, but the story is also available in English. Please enter the language you would like to read the story in. 'cn' | 'en' :";
			getline(cin, lg);
			if (lg == "en") is_chinese = false;
			else is_chinese = true;
			init_first_time();
			prolouge_num = 3;
			cout << "请输入存档文件名,游戏会创建新的存档 \nPlease enter the save file name; the game will create a new save file.:";
			getline(cin, filename);
			save_world(world_computers,netnodes,filename + ".json");
			cout << "\n初始账户：root 密码：123456" << endl;
			type_text("...", 1000);
			Story::show_the_background();
			return;
		}
		vector<string> args = split(tmp, ' ');
		if (args.size() > 2 || args.empty() || args[0] != "load") {
			cout << "输入错误的参数！(Error arguments !)";
			exit(0);
		}
		filename = args[1];
		world_computers = load_world(filename + ".json");
		netnodes = load_net(filename + ".json");
	}
}
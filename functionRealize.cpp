#include "cmulator.h"
using namespace std;
	//ai帮写的split函数
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
	void search_help_from(const string& n){
		static const map<string,string> messeges = {
			{"ls","ls\t显示当前目录下的所有文件和文件夹"},
			{"mkdir","mkdir [name]\t在当前目录下创建一个文件夹"},
			{"cd","cd [path]\t切换到特定目录"},
			{"del","del [path] | [name]\t在当前或特定目录删除文件或文件夹\n可带选项:\n-f | --force\t强制删除，不弹出确认提示"},
			{"pwd","pwd\t显示当前目录全称"},
			{"cat","cat [path] | [name]\t在当前或特定目录读取可读文件内容\n可带选项：\n-a | --all\t查看全部内容"},
			{"ipconfig","ipconfig\t显示当前网络信息\n可带选项：\n-p | --port\t只显示端口信息"},
			{"exit","exit\t退出命令行，运行关机程序"}
		};
		auto it = messeges.find(n);
		if(it == messeges.end()){
			cout<<"找不到指令："<<n<<endl;
			return;
		}
		cout<<it->second<<endl;
		return;
	}
	string get_time(){
		time_t now = time(0);
		tm *ltm = localtime(&now);
		string now_time = to_string(ltm->tm_hour) + ":" + to_string(ltm->tm_min) + ":" + to_string(ltm->tm_sec);
		return now_time;
	}
	void type_text(const string& str,int delay){
		for(const char c : str){
			cout<<c<<flush;
			this_thread::sleep_for(chrono::milliseconds(delay));
		}
		cout<<endl;
	}
using namespace ComputerDomain;
using namespace Filesystem;
using namespace SerializeJson;
	void init_game(){
		type_text("游戏加载......",20);
		type_text("制作者：Rosemary",20);
		type_text("个人博客：undertopia.top",20);
	}

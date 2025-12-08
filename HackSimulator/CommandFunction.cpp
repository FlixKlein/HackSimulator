/*
|	HackSimulator v0.0.5
|	
|	CommandFunction.cpp
|	this cpp if for some command functions
|	这个文件用于一些指令相关函数，尤其是帮助函数
|	
|	https://github.com/FlixKlein/HackSimulator
|	https://gitee.com/rosemarychn/HackSimulator
|	License : MIT
|	Personal Blog : https://undertopia.top
*/
#include "hacksimulator.h"
#include "UIManager.h"
using namespace std;
#ifdef WINCLS
void cls(){
	system("cls");
}
#elif LINUXCLS
void cls(){
	system("clear");
}
#endif

//aistudio帮写的split函数
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
		{"cp","cp [source] [target]\t将源复制到目标"},
		{"cl","cl\t清空命令行窗口"}, 
		{"user","user\t与用户有关的指令\n可带选项：\n-all | --all\t列出所有用户\n-add | --add [username] 添加用户无初始密码\n-del | --delete [username]\t删除用户\n	-p | --promote [username]\t向上提升用户一个等级权限"},
		//{"connect","connect [ip]\t连接到特定IP的计算机"},
		//{"disconnect","disconnect\t断开当前连接的计算机"},
		{"login","login [username] [password]\t登录到当前连接的计算机"},
		{"logout","logout\t登出当前连接的计算机"},
		{"help","help [command]\t显示帮助信息"},
		{"time","time\t显示当前系统时间"},
		{"session","session\t管理后台会话\n可带选项：\n-l | --list\t列出所有后台会话\n-k | --kill [id]\t终止后台会话[id]"},
		{"start","start\t启动一个后台会话"},
		{"exec","exec [id] [cmd]\t后台会话[id]运行命令cmd"},
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
	tm* ltm = new tm;
	localtime_s(ltm, &now);
	string now_time = to_string(ltm->tm_hour) + ":" + to_string(ltm->tm_min) + ":" + to_string(ltm->tm_sec);
	return now_time;
}

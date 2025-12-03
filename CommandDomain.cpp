#include "cmulator.h"

namespace CommandDomain{
	using namespace std;
	using namespace ComputerDomain;
	ParsedArgument parsed_argument(const vector<string>& args){
		ParsedArgument parsed;
		for(size_t i = 1;i<args.size();i++){
			const string& current_arg = args[i];
			if(current_arg.rfind("-",0) == 0){//是选项
				if(i + 1 < args.size() && args[i+1].rfind("-",0) != 0){//带值的选项不能放到最后，cmd似乎就是这样,带值的下一个必须跟值而不是其他的
					parsed.options[current_arg] = args[i+1];
					i++;
				} else {
					parsed.flags.insert(current_arg);
				}
			} else {
				parsed.arguments.emplace_back(current_arg);
			}
		}
		return parsed;
	}
	//所有指令函数的创建和注册都在这个命名空间完成，十分方便
	int cmd_ls(Session& session,[[maybe_unused]] const vector<string>& args){
		if(!session.current_dir){
			cout<<"错误，没有链接到dir"<<endl;
			return 1;
		}
		for(const auto& [key,value] : session.current_dir->get_subdir()){
			cout<<key<<endl;
		}
		for(const auto& [key,value] : session.current_dir->get_subfile()){
			cout<<key<<endl;
		}
		return 0;
		
	}
	int cmd_mkdir(Session& session,const vector<string>& args){
		if(!session.current_dir){
			cout<<"错误，没有链接到dir"<<endl;
			return 1;
		}
		if(args.size()<2 || args.size()>3){
			cout<<"错误的参数,用法 : mkdir [name]"<<endl;
			return 1;
		}
		const string n = args[1];
		if(n.find('/') != string::npos || n.find('.') != string::npos
		){
			cout<<"错误的命名方式，不能含有/或."<<endl;
			return 1;
		}
		auto new_dir = make_unique<Dir>(n);
		if(session.current_dir->add_dir(move(new_dir))){
			cout << "目录 '" << n << "' 创建成功。" <<endl;
			return 0;
		} else {
			cout<<"错误，无法创建！"<<endl;
			return 1;
		}
	}
	int cmd_cd(Session& session,const vector<string>& args){
		if(!session.current_dir){
			cout<<"错误，没有链接到dir"<<endl;
			return 1;
		}
		if(args.size()<2 || args.size()>3){
			cout<<"错误的参数，用法 : cd [target]"<<endl;
			return 1;
		}
		auto it = session.current_dir->locate_dir_from_path(args[1]);
		if(it != nullptr){
			session.change_dir(it);
			cout<<"成功切换到目录："<<args[1]<<endl;
			return 0;
		}
		cout<<"找不到目录："<<args[1]<<endl;
		return 1;
	}
	int cmd_pwd(Session& session,[[maybe_unused]]const vector<string>& args){
		if(!session.current_dir){
			cout<<"错误，没有链接到dir"<<endl;
			return 1;
		}
		cout<<session.current_dir->show_path()<<endl;
		return 0;
	}
	int cmd_del(Session& session,const vector<string>& args){
		if(!session.current_dir){
			cout<<"错误，没有链接到dir"<<endl;
			return 1;
		}
		ParsedArgument parsed = parsed_argument(args);
		if(parsed.arguments.empty()){
			cout<<"缺少必要的参数，输入help del以查看帮助"<<endl;
		}
		const string& name = parsed.arguments[0];//默认只能输入一个目录地址
		bool force_del = parsed.flags.count("-f") > 0 || parsed.flags.count("--force") > 0;
		if(!force_del){
			cout<<"确定要删除吗？输入[y/n]:";
			string s;
			getline(cin,s);
			if(s != "y" && s != "Y"){
				cout<<"操作已取消!"<<endl;
				return 0;
			}
		}
		//先判断是不是文件夹
		Dir* target_dir = session.current_dir->locate_dir_from_path(name);
		if(target_dir != nullptr){
			if(session.current_dir->delete_dir_from_path(name)){
				cout<<"文件夹:"<<name<<"已成功删除"<<endl;
				return 0;
			} else {
				cout<<"删除："<<name<<"失败"<<endl;
				return 1;
			}
		}
		File* target_file = session.current_dir->locate_file_from_path(name);
		if(target_file != nullptr){
			if(session.current_dir->delete_file_from_path(name)){
				cout<<"文件:"<<name<<"已成功删除"<<endl;
				return 0;
			} else {
				cout<<"删除："<<name<<"失败"<<endl;
				return 1;
			}
		}	
		cout<<"找不到所谓的:"<<name<<endl;
		cout<<"或者是输入错误的参数！"<<endl;
		return 0;		
	}
	int cmd_cat(Session& session,const vector<string>& args){
		ParsedArgument parsed = parsed_argument(args);
		if(parsed.arguments.empty()){
			cout<<"缺少必要的参数，输入help cat以查看帮助"<<endl;
		}
		const string& name = parsed.arguments[0];//也是就只能看一个东西
		//目前不知道有啥参数，就先写一个显示全部段落吧
		bool show_all_phrase = parsed.flags.count("-a") > 0 || parsed.flags.count("--all") > 0;
		if(show_all_phrase){
			File* target_file = session.current_dir->locate_file_from_path(name);
			if(target_file != nullptr){
				for(auto& str : target_file->returnContent()){
					cout<<str<<endl;
				}
				return 0;
			} else {
				cout<<"找不到文件:"<<name<<endl;
				return 1;
			}
		}
		cout<<"输入了错误的参数!"<<endl;
		return 1;
	}
	int cmd_ipconfig(Session& session,const vector<string>& args){
		ParsedArgument parsed = parsed_argument(args);
		bool show_port_only = parsed.flags.count("-p") > 0 || parsed.flags.count("--port") > 0;
		if(show_port_only){
			cout<<"Port list :"<<endl;
			for(const auto& [key,value] : session.current_computer->get_ip_port()){
				cout<<key<<"\t"<<value<<endl;
			}
			return 0;
		}
		vector<string> tmp = session.current_computer->get_ipconfig();
		for(auto it : tmp){
			cout<<it<<endl;
		}
		return 0;
	}
	int cmd_help(Session&,const vector<string>& args){
		int n = args.size();
		if(n == 1){
			cout<<"有关某个命令的详细信息，请键入 help 命令名"<<endl;
			cout<<"help\t\t显示特定指令帮助"<<endl;
			cout<<"ls\t\t显示当前目录下的所有文件和文件夹"<<endl;
			cout<<"mkdir\t\t创建空文件夹"<<endl;
			cout<<"cd\t\t进入特定目录"<<endl;
			cout<<"del\t\t删除文件或文件夹"<<endl;
			cout<<"pwd\t\t显示当前目录"<<endl;
			cout<<"cat\t\t显示可读文件内容"<<endl;
			cout<<"ipconfig\t\t显示当前网络信息"<<endl;
			cout<<"exit\t\t退出程序并显示关机选项"<<endl;
		} else if(n > 2){
			cout<<"错误的用法！输入 help 或者 help 命令名"<<endl;
		} else {
			search_help_from(args[1]);
		}
		return 0;
	}
	
	CommandProcessor::CommandProcessor(){
		commands["ls"] = &cmd_ls;
		commands["mkdir"] = &cmd_mkdir;
		commands["pwd"] = &cmd_pwd;
		commands["cd"] = &cmd_cd;
		commands["help"] = &cmd_help;
		commands["del"] = &cmd_del;
		commands["cat"] = &cmd_cat;
		commands["ipconfig"] = &cmd_ipconfig;
		commands["exit"] = [](Session&,const vector<string>&) -> int {return -1;};
	}
	//执行指令的函数
	int CommandProcessor::execute(Session& session,const string cmd){
		auto parts = split(cmd,' ');
		if(parts.empty()){
			return 0;//不执行
		}
		const string name = parts[0];//指令名字
		auto it = commands.find(name);
		if(it != commands.end()){
			return it->second(session,parts);//执行function
		} else {
			cout<<"找不到指令:"<<name<<endl;
			return 1;
		}
	}
}

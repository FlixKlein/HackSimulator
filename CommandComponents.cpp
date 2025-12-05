/*
|	HackSimulator v0.0.4
|	
|	CommandComponents.cpp
|	
|	https://github.com/FlixKlein/HackSimulator
|	https://gitee.com/rosemarychn/HackSimulator
|	License : MIT
|	Personal Blog : https://undertopia.top
*/
#include "hacksimulator.h"

namespace CommandComponents{
	using namespace std;
	using namespace ComputerComponents;
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
			return 1;
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
	/*int cmd_wget(Session& session,const vector<string>& args){
		
	}
	int cmd_nano(Session& session,const vector<string>& args){
		
	}*/
	int cmd_cp(Session& session,const vector<string>& args){
		ParsedArgument parsed = parsed_argument(args);
		if(parsed.arguments.size() < 2){
			cout<<"缺少必要的参数"<<endl;
			return 1;
		}
		const string source = parsed.arguments[0];
		const string target = parsed.arguments[1];
		auto dir_source = session.current_dir->locate_dir_from_path(source);
		if(dir_source != nullptr){
			if(source == "/"){
				cout<<"尝试把根目录复制！"<<endl;
				return 1;
			}
			auto dir_target = session.current_dir->locate_dir_from_path(target);
			if(dir_target == nullptr){
				cout<<"找不到目标目录！"<<endl;
				return 1;
			}
			if(dir_target->add_dir(dir_source->clone(dir_target))){
				cout<<"文件夹复制成功！"<<endl;
				return 0;
			}
			cout<<"文件夹复制失败！"<<endl;
			return 1;
		}
		auto file_source = session.current_dir->locate_file_from_path(source);
		if(file_source == nullptr){
			cout<<"找不到："<<source<<endl;
			return 1;
		}
		auto dir_target = session.current_dir->locate_dir_from_path(target);
		if(dir_target == nullptr){
			cout<<"找不到目标目录！"<<endl;
			return 1;
		}
		if(dir_target->add_file(make_unique<File>(file_source->name,file_source->returnContent()))){
			cout<<"文件复制成功！"<<endl;
			return 0;
		}
		cout<<"文件复制失败！"<<endl;
		return 1;
		
	}
	int cmd_help(Session&,const vector<string>& args){//记得修改search_help_from在FunctionImplement.cpp
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
			cout<<"cp\t\t复制文件或者文件夹"<<endl;
			cout<<"start\t\t启动一个后台会话"<<endl;
			cout<<"exec\t\t后台会话运行命令"<<endl;
			cout<<"session\t\t显示线程"<<endl;
			cout<<"exit\t\t退出程序并显示关机选项"<<endl;
		} else if(n > 2){
			cout<<"错误的用法！输入 help 或者 help 命令名"<<endl;
		} else {
			search_help_from(args[1]);
		}
		return 0;
	}
	
	CommandProcessor::CommandProcessor(SessionManager& sm){
		commands["ls"] = &cmd_ls;
		commands["mkdir"] = &cmd_mkdir;
		commands["pwd"] = &cmd_pwd;
		commands["cd"] = &cmd_cd;
		commands["help"] = &cmd_help;
		commands["del"] = &cmd_del;
		commands["cat"] = &cmd_cat;
		commands["ipconfig"] = &cmd_ipconfig;
		commands["cp"] = &cmd_cp;
		commands["start"] = [&sm](Session& session,const vector<string>&) -> int {
			sm.create_session(session.current_computer);
			cout<<"后台会话已启动"<<endl;
			return 0;
		};
		commands["exec"] = [this,&sm](Session& current,const vector<string>& args) -> int {
			if(args.size()<3){
				cout<<"参数错误！"<<endl;
				return 1;
			}
			try{
				int id = stoi(args[1]);
				Session* target = sm.get_session(id);
				stringstream command_build;
				for(size_t i = 2;i<args.size();i++){
					command_build<<args[i]<<" ";
				}
				string str = command_build.str();
				str.pop_back();
				if(target == nullptr){
					cout<<"错误，找不到会话"<<endl;
					return 1;
				} else {
					this->execute_background(*target,str,current);
				}
			} catch(const exception& e){
				cout<<"错误，无效的会话"<<endl;
				return 1;
			}
			return 0;
		};
		commands["session"] = [&sm](Session&,const vector<string>&) -> int {
			sm.list_session();
			return 0;
		};
		commands["exit"] = [](Session&,const vector<string>&) -> int {return -1;};
	}
	int CommandProcessor::execute_background(Session& session,const string cmd,Session& main_session){
		if(session.is_busy){
			cout<<"错误，会话忙碌中"<<endl;
			return 1;
		}
		thread t([this,&session,cmd,main_session_ptr = &main_session](){
			session.is_busy = true;
			{
				lock_guard<mutex> lock(cout_mutex);
				cout<<endl;
			}
			this->execute(session,cmd);
			{
				lock_guard<mutex> lock(cout_mutex);
				cout<<endl;
				session.is_busy = false;
				if (main_session_ptr && main_session_ptr->current_computer && main_session_ptr->current_dir) {
				    std::cout << "[" << main_session_ptr->current_computer->name << " " 
				        << main_session_ptr->current_dir->show_path() << "]$ " << std::flush;
				}
			}
		});
		t.detach();
		return 0;
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
			if(name == "mkdir" || name == "del" || name == "cp"){
				lock_guard<mutex> lock(world_mutex);
				return it->second(session,parts);
			} else {
				return it->second(session,parts);
			}
			
		} else {
			cout<<"找不到指令:"<<name<<endl;
			return 1;
		}
	}
}

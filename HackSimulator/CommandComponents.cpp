/*
|	HackSimulator v0.0.6
|	
|	CommandComponents.cpp
|	this cpp implements some command functions
|	这个文件实现了一些指令函数
|	
|	https://github.com/FlixKlein/HackSimulator
|	https://gitee.com/rosemarychn/HackSimulator
|	License : MIT
|	Personal Blog : https://undertopia.top
*/
#include "hacksimulator.h"
#include "UIManager.h"

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
				for(const auto& line : target_file->returnContent()){
					cout << from_utf8(line) << endl;
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
	int cmd_ping(Session& session, const vector<string>& args) {
		ParsedArgument parsed = parsed_argument(args);
		if(parsed.arguments.size() < 1){
			cout<<"缺少必要的参数，输入help ping以查看帮助"<<endl;
			return 1;
		}
		const string& target_ip = parsed.arguments[0];
		if(!netnodes.contains(target_ip)){
			cout<<"无法ping通 "<<target_ip<<"，目标主机不可达"<<endl;
			return 1;
		}
		cout << "正在 Ping " << target_ip << " 具有 32 字节的数据:" << endl;
		for(int i = 0;i<4;i++){
			Net::Packet packet(session.current_computer->get_ip(), target_ip, "ping");
			packet.set_meta("type", "ping");
			bool send = netnodes[session.current_computer->get_ip()]->send(packet);
			if (send) {
				cout << "来自 " << target_ip << " 的回复: 字节=32 时间=" << packet.ms << "ms TTL=" << packet.ttl << endl;
			}
			else {
				cout << "请求超时。" << endl;
			}
		}
		return 0;
	}
	/*int cmd_wget(Session& session,const vector<string>& args){
		
	}*/
	int cmd_nano(Session& session,const vector<string>& args){
		if (args.size() < 3 && args.size() != 1) {
			cout << "错误的参数！" << endl;
			return 1;
		}
		auto it = session.current_dir->locate_file_from_path("/bin/nano.exe");
		if (it == nullptr || it->file_hash != app_hash_map["nano.exe"]) {
			cout << "找不到应用nano.exe，或者哈希签名不匹配！" << endl;
			cout << "请前往HackSimulator官网自行下载" << endl;
			return 1;
		}
		Apps::Nano nano;
		if (args.size() == 1) {

			return 0;
		}
		else {
			return 1;
		}
	}
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
	int cmd_help(Session& session,const vector<string>& args){//记得修改search_help_from在FunctionImplement.cpp
		Apps::Help help;
		auto it = session.current_dir->locate_file_from_path("/bin/help.exe");
		if (it == nullptr) {
			cout << "找不到应用help，请访问HackSimulator官网获取help.exe！" << endl;
			return 1;
		}
		if (it->file_hash != help.get_hash()) {
			cout << "不匹配的hash签名，help.exe运行失败!" << endl;
			return 1;
		}
		return help.execute(session, args);
	}
	int cmd_user(Session& session, const vector<string>& args) {
		ParsedArgument parsed = parsed_argument(args);
		//user函数的用法：user -all | -add username | -del username | -show username
		if (args.size() < 2) {
			cout << "错误的参数数量！" << endl;
			return 1;
		}
		bool flag_all = parsed.flags.count("-all") > 0 || parsed.flags.count("--all") > 0;
		bool flag_add = parsed.options.count("-add") > 0 || parsed.options.count("--add") > 0;
		bool flag_del = parsed.options.count("-del") > 0 || parsed.options.count("--delete") > 0;
		bool flag_promote = parsed.options.count("-p") > 0 || parsed.options.count("--promote") > 0;
		if(!flag_add && !flag_del && !flag_all && !flag_promote){
			cout << "错误的参数！" << endl;
			return 1;
		}
		if (flag_all) {
			cout << "用户名\t\t权限\t\t最后登录时间" << endl;
			//这里要用反射来获取枚举名称，其实就是我懒得写switch语句
			for(const auto& [n,a] : session.current_computer->get_account()){
				cout << n << "\t\t" << magic_enum::enum_name(a.permission) << "\t\t" << a.last_login_time << endl;
			}
			return 0;
		}
		auto it = parsed.options.begin();
		if (flag_add) {
			if(session.current_computer->get_account().count(it->second) > 0){
				cout << "错误，用户已存在！" << endl;
				return 1;
			}
			if(session.current_computer->get_account()[session.current_computer->login].permission != Permission_level::Admin){
				cout << "错误，权限不足！" << endl;
				return 1;
			}
			session.current_computer->get_account()[it->second] = Account{ "" , Permission_level::Guest , get_time()};
			return 0;
		}
		else if(flag_del){
			if(session.current_computer->get_account().count(it->second) == 0){
				cout << "错误，用户不存在！" << endl;
				return 1;
			}
			if(session.current_computer->get_account()[session.current_computer->login].permission != Permission_level::Admin){
				cout << "错误，权限不足！" << endl;
				return 1;
			}
			if(it->second == session.current_computer->login){
				cout << "错误，无法删除当前登录用户！" << endl;
				return 1;
			}
			if (session.current_computer->get_account()[it->second].permission == Permission_level::Admin) {
				int cnt = 0;
				for(const auto& [n,a] : session.current_computer->get_account()){
					if(a.permission == Permission_level::Admin){
						cnt++;
					}
				}
				if(cnt <= 1){
					cout << "错误，无法删除最后一个管理员用户！" << endl;
					return 1;
				}
			}
			session.current_computer->get_account().erase(it->second);
			return 0;
		}
		else {
			if(session.current_computer->get_account().count(it->second) == 0){
				cout << "错误，用户不存在！" << endl;
				return 1;
			}
			if (session.current_computer->get_account()[session.current_computer->login].permission != Permission_level::Admin) {
				cout << "错误，权限不足！" << endl;
				return 1;
			}
			if(session.current_computer->get_account()[it->second].permission == Permission_level::Admin){
				cout << "错误，无法提升管理员权限！" << endl;
				return 1;
			}
			session.current_computer->get_account()[it->second].permission = static_cast<Permission_level>(static_cast<int>(session.current_computer->get_account()[it->second].permission) + 1);
			return 0;
		}
	}
	int cmd_passwd(Session& session, const vector<string>& args) {
		ParsedArgument parsed = parsed_argument(args);
		if(parsed.arguments.size() != 1){
			cout<<"错误的参数，输入help passwd以查看帮助"<<endl;
			return 1;
		}
		//passwd [account]
		const string& account_name = parsed.arguments[0];
		if(session.current_computer->get_account().count(account_name) == 0){
			cout<<"错误，用户不存在！"<<endl;
			return 1;
		}
		if (session.current_computer->get_account()[session.current_computer->login].permission != Permission_level::Admin) {
			cout<<"错误，权限不足！"<<endl;
			return 1;
		}
		cout << "请输入新密码：";
		string new_password;
		getline(cin, new_password);
		session.current_computer->get_account()[account_name].password = new_password;
		return 0;
	}
	int cmd_login(Session& session, const vector<string>& args) {
		ParsedArgument parsed = parsed_argument(args);
		if (parsed.arguments.size() < 2) {
			cout << "错误的参数，输入help login获取帮助" << endl;
			return 1;
		}
		const string& name = parsed.arguments[0];
		const string& pass = parsed.arguments[1];
		if (session.current_computer->login == name) {
			cout << "已在账户 " << name << " 上" << endl;
			return 1;
		}
		for (auto& [n,acc] : session.current_computer->get_account()) {
			if (n == name && acc.password == pass) {
				session.current_computer->login = name;
				cout << "已切换到账户 " << name << endl;
				return 0;
			}
		}
		cout << "不存在账户 " << name << " 或者密码错误" << endl;
		return 1;
	}
	int cmd_logout(Session& session, const vector<string>& args) {
		session.current_computer->login = "";
		cout << "已退出登录" << endl;
		return 0;
	}
	int cmd_touch(Session& session, const vector<string>& args) {
		if (args.size() < 2){
			cout << "缺少参数" << endl;
			return 1;
		}
		const string& n = args[1];
		if (n.find('/') != string::npos) {
			cout << "错误的命名方式，不能含有/" << endl;
			return 1;
		}
		unique_ptr<File> f = make_unique<File>(n);
		if (session.current_dir->add_file(move(f))) {
			cout << "成功创建" << endl;;
			return 0;
		}
		return 1;
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
		commands["ping"] = &cmd_ping;
		commands["user"] = &cmd_user; 
		commands["passwd"] = &cmd_passwd; 
		commands["login"] = &cmd_login;
		commands["logout"] = &cmd_logout;
		commands["touch"] = &cmd_touch;
		commands["cl"] = [](Session&,const vector<string>&) -> int {
			cls();
			return 0;
			};
		commands["exec"] = [this, &sm](Session& current, const vector<string>& args) -> int {
			if (args.size() < 3) {
				cout << "参数错误！" << endl;
				return 1;
			}
			try {
				int id = stoi(args[1]);
				Session* target = sm.get_session(id);
				stringstream command_build;
				for (size_t i = 2; i < args.size(); i++) {
					command_build << args[i] << " ";
				}
				string str = command_build.str();
				str.pop_back();
				if (target == nullptr) {
					cout << "错误，找不到会话" << endl;
					return 1;
				}
				else {
					this->execute_background(*target, str, current);
				}
			}
			catch (const exception& e) {
				cout << "错误，无效的会话" << endl;
				return 1;
			}
			return 0;
			};
		commands["session"] = [&sm](Session&, const vector<string>&) -> int {
			sm.list_session();
			return 0;
			};
		commands["read"] = [](Session&, const vector<string>& args) -> int {
			if (args.size() < 2) {
				cout << "错误的参数！" << endl;
				return 1;
			}
			try {
				int num = stoi(args[1]);
				if (num < 0 || num > prolouge_num) {
					cout << "错误，章节编号无效！" << endl;
					return 1;
				}
				show_story(num);
			}
			catch (const std::exception& e) {
				cout << "错误，无效的编号" << endl;
			}
			return 0;
			
		};
		commands["exit"] = [](Session&,const vector<string>&) -> int {return -1;};
	}
	//执行指令的函数
	int CommandProcessor::execute_background(Session& session, const string cmd, Session& main_session) {
		if (session.is_busy) {
			cout << "错误，会话忙碌中" << endl;
			return 1;
		}
		thread t([this, &session, cmd, main_session_ptr = &main_session]() {
			session.is_busy = true;
			{
				lock_guard<mutex> lock(cout_mutex);
				cout << endl;
			}
			this->execute(session, cmd);
			{
				lock_guard<mutex> lock(cout_mutex);
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
	int CommandProcessor::execute(Session& session,const string cmd){
		auto parts = split(cmd,' ');
		if(parts.empty()){
			return 0;//不执行
		}
		const string name = parts[0];//指令名字
		auto it = commands.find(name);
		if(it != commands.end()){
			if (session.current_computer) {
				stringstream log_ss;
				log_ss << "User=" << session.current_computer->login << " IP=" << session.current_computer->get_ip() << " CMD=" << cmd;
				session.current_computer->write_log(LogTarget::Command, log_ss.str());
				if (name == "mkdir" || name == "del" || name == "cp") {
					lock_guard<mutex> lock(world_mutex);
					return it->second(session, parts);
				}
				else {
					return it->second(session, parts);
				}
			}
			
		} else {
			cout<<"找不到指令:"<<name<<endl;
			return 1;
		}
	}
}

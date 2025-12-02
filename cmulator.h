#include <string>
#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <memory>
#include <functional>
#include <sstream>
#include <random>
#include <set>
#pragma once

std::vector<std::string> split(std::string_view str,char delimiter);
std::list<std::string> split_list(std::string_view str,char delimiter);
			
namespace Filesystem{
	using namespace std;
	class File{
		public:
			string name;
			File(const string n,const vector<string> c = {}) : name(n) , content(c) {};
			vector<string> returnContent();
			~File();
		private:
			int size;
			vector<string> content;
			
	};
	class Dir{
		public:
			string name;
			Dir(const string n) : name(n) {};//仅仅名字
			Dir(const string n,Dir* p) : name(n) , fath(p){};//一般来说初始化都是有父亲文件夹
			//即便用了智能指针，还是要写一下拷贝构造函数，学习深拷贝写法
			Dir(const Dir& other) = delete;//拷贝构造函数
			Dir& operator=(const Dir& other) = delete;//拷贝复制
			unique_ptr<Dir> clone() const {
				auto new_dir = make_unique<Dir>(name,nullptr);
				for(const auto& [key,value] : sub_file ){
					new_dir->sub_file[key] = make_unique<File>(value->name,value->returnContent());
				}
				for(const auto& [key,value] : sub_dir){
					new_dir->sub_dir[key] = value->clone();
					new_dir->sub_dir[key]->fath = value.get();
				}
				return new_dir;
			}
			
			//成员函数
			Dir* locate_dir_from_now(const string dir_name);
			File* locate_file_from_now(const string file_name);
			Dir* locate_dir_from_path(const string dir_path);
			File* locate_file_from_path(const string file_path);
			const string show_path() const;
			bool name_exist(const string name) const;
			bool is_ancestor_of(const Dir* target,const Dir* current);
			bool delete_dir(const string dir_name);
			bool delete_file(const string file_name);
			bool delete_dir_from_path(const string dir_path);
			bool delete_file_from_path(const string file_path);
			bool add_dir(unique_ptr<Dir> other);
			bool add_file(unique_ptr<File> other);
			const map<string,unique_ptr<Dir>>& get_subdir() const {
				return sub_dir;
			}
			const map<string,unique_ptr<File>>& get_subfile() const {
				return sub_file;
			}
			~Dir();
		private:
			Dir* fath;
			map<string,unique_ptr<Dir>> sub_dir;
			map<string,unique_ptr<File>> sub_file;
	};

};

namespace ComputerDomain{
	using namespace std;
	using namespace Filesystem;
	class Computer{
		public:
			string name;
			static int id;
			Computer(const string n,const string usrn = "root",const string pswd = "admin123");
			Computer(const Computer& other) = delete;//禁止拷贝构造
			Computer& operator=(const Computer& other) = delete;//禁用...
			//下面是移动语义，因为主函数用到了vector<Computer>这样的会在push_back调用拷贝函数，但是unique_ptr禁用了他
			Computer(Computer&& other) noexcept = default;
			Computer& operator=(Computer&& other) noexcept = default;
			~Computer();
			string id_to_ip(const int i);
			Dir* get_root(){
				return root.get();
			}
		private:
			string username;
			string password;
			string ip;
			map<int,bool> ip_port;
			unique_ptr<Dir> root = make_unique<Dir>("/");
			
	};
	class Session{
		public:
			Computer* current_computer;//观察指针
			Dir* current_dir;
			bool is_login_in;
			Session() : current_computer(nullptr) , current_dir(nullptr) , is_login_in(false) {}
			void connect_to(Computer* target){
				current_computer = target;
				current_dir = target->get_root();
			}
			void change_dir(Dir* target){
				current_dir = target;
			}
	};
};

namespace CommandDomain{
	using namespace std;
	using namespace ComputerDomain;
	using Command = function<int(Session&,const vector<string>&)>;
	class CommandProcessor{
		private:
			map<string,Command> commands;//存储指令函数
		public:
			CommandProcessor();
			int execute(Session& session,const string cmd);
	};
	struct ParsedArgument{
		set<string> flags;//存储-f这样的布尔标志
		map<string,string> options;//存储需要值的选项，如-o a.txt
		vector<string> arguments;//存储值，而不是选项，不需要选项的值，固定必须出现
	};
	ParsedArgument parsed_argument(const vector<string>& args);
}
//下面是一些全局函数
void search_help_from(const std::string& n);

#pragma once
#include <string> // substr stoi to_string
#include <iostream> // std
#include <vector> // vector
#include <list> // list
#include <map> // map
#include <memory> // unique_ptr
#include <functional> // function
#include <sstream> // stringstream
#include <random> // random_device mt19937
#include <set> // set
#include "json.hpp"
#include <fstream> // ofstream
#include <ctime> // time_t time tm
#include <thread> //this_thread::sleep_for
#include <chrono> //chrono::milliseconds

std::vector<std::string> split(std::string_view str,char delimiter);
std::list<std::string> split_list(std::string_view str,char delimiter);
//前置声明，防止下面的烦人问题
namespace Filesystem {
    class File;
    class Dir;
}

namespace ComputerDomain {
    class Computer;
    class Session;
    struct Account;
    enum class Permission_level;
}
namespace CommandDomain {
    class CommandProcessor;
    struct ParsedArgument;
}
using json = nlohmann::json;
namespace SerializeJson{
	json serialize_file(const Filesystem::File& file);
	json serialize_dir(const Filesystem::Dir& dir);
	json serialize_computer(const ComputerDomain::Computer& computer);
	json serialize_session(const ComputerDomain::Session& session);
	void save_world(const std::vector<ComputerDomain::Computer>& world_computers,const std::string filename);
	
	std::unique_ptr<Filesystem::File> deserialize_file(const json& j);
	std::unique_ptr<Filesystem::Dir> deserialize_dir(const json& j);
	ComputerDomain::Computer deserialize_computer(const json& j);
	std::vector<ComputerDomain::Computer> load_world(const std::string filename);
}
namespace Filesystem{
	using namespace std;
	class File;
	class Dir;
	class File{
		public:
			string name;
			File(const string n,const vector<string> c = {}) : name(n) , content(c) {};
			vector<string> returnContent();
			const vector<string> returnContent() const;
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
			unique_ptr<Dir> clone() const;
			
			//成员函数
			Dir* locate_dir_from_now(const string dir_name);
			const Dir* locate_dir_from_now(const string dir_name) const;
			File* locate_file_from_now(const string file_name);
			const File* locate_file_from_now(const string file_name) const;
			Dir* locate_dir_from_path(const string dir_path);
			const Dir* locate_dir_from_path(const string dir_path) const;
			File* locate_file_from_path(const string file_path);
			const File* locate_file_from_path(const string file_path) const;
			const string show_path() const;
			bool name_exist(const string name) const;
			bool is_ancestor_of(const Dir* target,const Dir* current) const;
			bool delete_dir(const string dir_name);
			bool delete_file(const string file_name);
			bool delete_dir_from_path(const string dir_path);
			bool delete_file_from_path(const string file_path);
			bool add_dir(unique_ptr<Dir> other);
			bool add_file(unique_ptr<File> other);
			const map<string,unique_ptr<Dir>>& get_subdir() const;
			const map<string,unique_ptr<File>>& get_subfile() const;
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
	class Computer;
	class Session;
	enum class Permission_level{
		Guest,
		User,
		Admin //存储一些权限级别
	};
	struct Account{//存储了账户数据
		string password;
		Permission_level permission = Permission_level::User;
		string last_login_time;
	};
	//account的序列化实现，由于ADL(参数依赖查找，json那个库使用的）特性，我必须把它放在Account所在的命名空间里
	NLOHMANN_JSON_SERIALIZE_ENUM(Permission_level, {
		{Permission_level::Guest , "guest"},
		{Permission_level::User , "user"},
		{Permission_level::Admin , "admin"},
	})
	void to_json(json& j,const Account& acc);
	void from_json(const json& j,Account& acc);
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
			Dir* get_root() const;
			const string get_ip() const;
			const map<int,bool> get_ip_port() const;
			const vector<string> get_ipconfig() const;
			const map<string,Account> get_account() const;
			friend Computer SerializeJson::deserialize_computer(const json& j);
		private:
			string ip;
			map<string,Account> accounts;
			map<int,bool> ip_port;
			unique_ptr<Dir> root = make_unique<Dir>("/");
			
	};
	class Session{//工具方法？还是保存一下吧
		public:
			Computer* current_computer;//观察指针
			Dir* current_dir;
			bool is_login_in;
			Session() : current_computer(nullptr) , current_dir(nullptr) , is_login_in(false) {}
			void connect_to(Computer* target);
			void change_dir(Dir* target);
	};
};

namespace CommandDomain{//工具方法，不需要保存
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
std::string get_time();
void init_game();//初始化游戏，而不是一开始就给你个cmd，这里导入初始剧情
void type_text(const std::string& str,int delay);//aistudio实现的一个延时输出函数

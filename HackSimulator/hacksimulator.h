/*
|	HackSimulator v0.0.5
|
|	hacksimulator.h
|	this is the main header file of HackSimulator
|	这个是HackSimulator的主头文件
|
|	https://github.com/FlixKlein/HackSimulator
|	https://gitee.com/rosemarychn/HackSimulator
|	License : MIT
|	Personal Blog : https://undertopia.top
*/
#pragma once
#include <string> // substr stoi to_string
#include <iostream> // std
#include <vector> // vector
#include <cstddef> // size_t
#include <list> // list
#include <map> // map
#include <unordered_map> // unordered_map
#include <memory> // unique_ptr
#include <functional> // function
#include <sstream> // stringstream
#include <random> // random_device mt19937
#include <set> // set
#include "json.hpp"
#include <fstream> // ofstream
#include <ctime> // time_t time tm
#include <thread> // this_thread::sleep_for
#include <chrono> // chrono::milliseconds
#include <mutex> // mutex
#include <cstdlib> // system
#include <atomic> // atomic
#include <string_view> // string_view
#include <locale>
#include <codecvt>
#include "UIManager.h"
#include "magic_enum.hpp" // magic_enum::enum_name magic_enum::enum_cast<>

#if defined(_WIN32) || defined(_WIN64)
#define WINCLS
#elif defined(__linux__) || defined(__APPLE__) || defined(__unix__)
#define LINUXCLS
#endif
#ifdef WINCLS
void cls();
#elif LINUXCLS
void cls();
#endif


//前置声明，防止下面的烦人问题
namespace FileSystem {
	class File;
	class Dir;
}
namespace ComputerComponents {
	class Computer;
	class Session;
	struct Account;
	enum class Permission_level;
}
namespace CommandComponents {
	class CommandProcessor;
	struct ParsedArgument;
}
namespace Net {

}
namespace Apps {
	class IApplication;//一个抽象基类，描述应用
	class Mail;
	class Nano;
	class Help;
}
class SessionManager;
using json = nlohmann::json;
namespace SerializeJson {
	json serialize_file(const FileSystem::File& file);
	json serialize_dir(const FileSystem::Dir& dir);
	json serialize_computer(const ComputerComponents::Computer& computer);
	void save_world(const std::vector<ComputerComponents::Computer>& world_computers, const std::string filename);

	std::unique_ptr<FileSystem::File> deserialize_file(const json& j);
	std::unique_ptr<FileSystem::Dir> deserialize_dir(const json& j, FileSystem::Dir* fath);
	ComputerComponents::Computer deserialize_computer(const json& j);
	std::vector<ComputerComponents::Computer> load_world(const std::string filename);
}
namespace FileSystem {
	using namespace std;
    // 修复 File 构造函数的成员初始化列表错误
    class File {
    public:
        string name;
        string file_hash;
		File(const string n, const vector<string> c = {});
        vector<string> returnContent();
        const vector<string> returnContent() const;
        ~File();
    private:
        int size;
        vector<string> content;
    };
	class Dir {
	public:
		string name;
		Dir(const string n) : name(n) {};//仅仅名字
		Dir(const string n, Dir* p) : name(n), fath(p) {};//一般来说初始化都是有父亲文件夹
		//即便用了智能指针，还是要写一下拷贝构造函数，学习深拷贝写法
		Dir(const Dir& other) = delete;//拷贝构造函数
		Dir& operator=(const Dir& other) = delete;//拷贝复制
		unique_ptr<Dir> clone(Dir* fath) const;//重载两个版本
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
		bool is_ancestor_of(const Dir* target, const Dir* current) const;
		bool delete_dir(const string dir_name);
		bool delete_file(const string file_name);
		bool delete_dir_from_path(const string dir_path);
		bool delete_file_from_path(const string file_path);
		bool add_dir(unique_ptr<Dir> other);
		bool add_file(unique_ptr<File> other);
		const unordered_map<string, unique_ptr<Dir>>& get_subdir() const;
		const unordered_map<string, unique_ptr<File>>& get_subfile() const;
		~Dir();
	private:
		Dir* fath;
		unordered_map<string, unique_ptr<Dir>> sub_dir;
		unordered_map<string, unique_ptr<File>> sub_file;
	};

};
namespace ComputerComponents {
	using namespace std;
	using namespace FileSystem;
	enum class Permission_level {
		Guest,
		User,
		Admin //存储一些权限级别
	};
	struct Account {//存储了账户数据
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
	void to_json(json& j, const Account& acc);
	void from_json(const json& j, Account& acc);
	class Computer {
	public:
		string name;
		static int id;
		string login;
		Computer(const string n, const string usrn = "root", const string pswd = "admin123");
		Computer(const Computer& other) = delete;//禁止拷贝构造
		Computer& operator=(const Computer& other) = delete;//禁用...
		void change_ip(const string& ip);
		void change_port(int port, bool state);
		//下面是移动语义，因为主函数用到了vector<Computer>这样的会在push_back调用拷贝函数，但是unique_ptr禁用了他
		Computer(Computer&& other) noexcept = default;
		Computer& operator=(Computer&& other) noexcept = default;
		~Computer();

		string id_to_ip(const int i);
		Dir* get_root() const;
		const string get_ip() const;
		const map<int, bool> get_ip_port() const;
		const vector<string> get_ipconfig() const;
		unordered_map<string, Account> get_account();
		const unordered_map<string, Account> get_account() const;//不知道此处何意味，不如直接public...
		friend Computer SerializeJson::deserialize_computer(const json& j);
	private:
		string ip;
		unordered_map<string, Account> accounts;
		map<int, bool> ip_port;
		unique_ptr<Dir> root = make_unique<Dir>("/");

	};
	class Session {//工具方法？还是保存一下吧
	public:
		Computer* current_computer;//观察指针
		Dir* current_dir;
		bool is_login_in;
		std::atomic<bool> is_busy = { false };
		Session() : current_computer(nullptr), current_dir(nullptr), is_login_in(false) {}
		~Session();
		void connect_to(Computer* target);
		void change_dir(Dir* target);
	};
};

namespace CommandComponents {//工具方法，不需要保存
	using namespace std;
	using namespace ComputerComponents;
	using Command = function<int(Session&, const vector<string>&)>;
	class CommandProcessor {
	private:
		unordered_map<string, Command> commands;//存储指令函数
	public:
		CommandProcessor(SessionManager& sm);
		int execute_background(Session& session, const string cmd, Session& main_session);//启动后台线程
		int execute(Session& session, const string cmd);
		//这里设计的时候想到一点，就是无论怎么改变，前台的会话不能改变，新建的会话只会后台执行，不然就会混乱
	};
	struct ParsedArgument {
		set<string> flags;//存储-f这样的布尔标志
		unordered_map<string, string> options;//存储需要值的选项，如-o a.txt
		vector<string> arguments;//存储值，而不是选项，不需要选项的值，固定必须出现
	};
	ParsedArgument parsed_argument(const vector<string>& args);
}

//全局性质的类，管理所有会话，于是我就不用命名空间了
class SessionManager {
private:
	std::map<int, std::unique_ptr<ComputerComponents::Session>> sessions;//全局会话存储
	int new_session_id = 0;
	std::mutex sessions_mutex;
public:
	//新建会话
	int create_session(ComputerComponents::Computer* comp);
	ComputerComponents::Session* get_session(int id);
	bool delete_session(int id);
	void list_session();
};
namespace Net {

}

namespace Apps {
	//特此声明：目前应用执行是在bin找可执行文件名，随后验证hash，如果存在就进入应用（开一个窗口显示信息），然后用应用的execute
	using namespace std;
	using Command = function<int(ComputerComponents::Session&, const vector<string>&)>;
	class IApplication {
	public:
		virtual ~IApplication() = default;//虚析构方法，至于为啥要虚的还要我多说吗
		virtual int execute(ComputerComponents::Session& session, const vector<string>& args) = 0;//纯虚函数，执行应用
		virtual const string get_hash() const = 0;//获取应用的哈希值，用于验证完整性
	};
	class Mail : public IApplication {
	public:
		int execute(ComputerComponents::Session& session, const vector<string>& args) override;
		const string get_hash() const override;
	private:
		unordered_map<string, Command> app_commands;//应用内的命令存储
	};
	class Help : public IApplication {
	public:
		int execute(ComputerComponents::Session& session, const vector<string>& args) override;
		const string get_hash() const override;
		Help();
	private:
		unordered_map<string, Command> app_commands;//应用内的命令存储
	};
	class Nano : public IApplication {
	public:
		int execute(ComputerComponents::Session& session, const vector<string>& args) override;
		const string get_hash() const override;
		Nano();
	};
}
//下面是一些全局函数
// 
//CommandFunction.cpp里的函数
void search_help_from(const std::string& n);//搜索帮助方法
std::string get_time();//获取系统时间
std::vector<std::string> split(std::string_view str, char delimiter);
std::list<std::string> split_list(std::string_view str, char delimiter);

namespace Story {
	//StoryFunction.cpp里的函数
	void init_story_from_file();
	void show_the_background();//这里导入初始剧情
	void read_prolouge(int num);
	extern std::unordered_map<int, std::string> all_story_map;
}

namespace Init {
	//InitFunction.cpp里的函数
	void init_first_time();//第一次初始化
	void init_game_from_json();//初始化游戏，有json就从json里导入
}

//TypingFunction.cpp里的函数
void show_the_logo();//展示一下游戏信息，打广告的
void type_text(const std::string& str, int delay);//aistudio实现的一个延时输出函数
void show_story(const std::string& story);//展示剧情文本文件
void show_story(int chapter_id);

//CodingFunction.cpp里的函数
std::string to_utf8(const std::string& input);
std::string from_utf8(const std::string& input);
extern inline std::string to_string(const char8_t* u8_str);
std::string simple_fnv_hash(const std::string& key, const std::string& message);


//需要设置全局变量来存储
extern std::vector<ComputerComponents::Computer> world_computers;
extern SessionManager session_manager;
extern CommandComponents::CommandProcessor processor;
extern std::mutex cout_mutex;
extern std::mutex world_mutex;//全局互斥锁
extern bool is_chinese;
extern int prolouge_num;

extern std::unordered_map<std::string, std::string> app_hash_map;//全局应用哈希签名
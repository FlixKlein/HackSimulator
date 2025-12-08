/*
|	HackSimulator v0.0.5
|
|	ComputerComponents.cpp
|	this cpp implements Computer and Session classes
|   这个文件实现Computer和Session类
|
|	https://github.com/FlixKlein/HackSimulator
|	https://gitee.com/rosemarychn/HackSimulator
|	License : MIT
|	Personal Blog : https://undertopia.top
*/
#include "hacksimulator.h"

namespace ComputerComponents {
	using namespace std;
	using namespace FileSystem;
	int Computer::id = 1;
	string Computer::id_to_ip(const int i) {
		random_device rd;
		mt19937 gen(rd());
		uniform_int_distribution<int> dist_int1(1, 255);
		uniform_int_distribution<int> dist_int2(1, 10000);
		string res = to_string(dist_int1(gen)) + "." + \
			to_string((dist_int2(gen) + i) % 255) + "." + \
			to_string(dist_int1(gen)) + "." + \
			to_string((dist_int2(gen) + i) % 255);
		return res;
	}
	Computer::Computer(const string n, const string usrn, const string pswd) {
		name = n;
		Account account = {
			pswd,
			Permission_level::Admin,
			get_time()
		};
		accounts[usrn] = account;
		id += 1;
		ip = id_to_ip(id);
		login = usrn;
		ip_port[21] = false;
		ip_port[22] = false;
		ip_port[80] = false;
		ip_port[88] = false;
		ip_port[443] = false;
	}
	//下面这几个都是权限极高的函数，不得不说完全是丢掉了类的封闭性，不过目前没有更好的处理办法暂且如此吧
	const string Computer::get_ip() const {
		return ip;
	}
	void Computer::change_ip(const string& ip) {
		this->ip = ip;
		return;
	}
	void Computer::change_port(int port, bool state) {
		ip_port[port] = state;
		return;
	}
	const map<int, bool> Computer::get_ip_port() const {
		return ip_port;
	}
	const vector<string> Computer::get_ipconfig() const {
		vector<string> res;
		res.emplace_back("ip address : " + this->ip);
		res.emplace_back("Port list :");
		for (auto& [key, value] : ip_port) {
			res.emplace_back(to_string(key) + "\t" + to_string(value));
		}
		return res;
	}
	const unordered_map<string, Account> Computer::get_account() const {
		unordered_map<string, Account> res;
		for (auto& [key, value] : accounts) {
			res[key] = value;
		}
		return res;
	}
	unordered_map<string, Account> Computer::get_account() {
		unordered_map<string, Account> res;
		for (auto& [key, value] : accounts) {
			res[key] = value;
		}
		return res;
	}
	//account的序列化实现，由于ADL(参数依赖查找，json那个库使用的）特性，我必须把它放在Account所在的命名空间里
	void to_json(json& j, const Account& acc) {
		j = json{
			{"password",acc.password},
			{"permission",acc.permission},
			{"last_login_time",acc.last_login_time}
		};
	}
	void from_json(const json& j, Account& acc) {
		j.at("password").get_to(acc.password);
		j.at("permission").get_to(acc.permission);
		j.at("last_login_time").get_to(acc.last_login_time);
	}
	//顺带提一嘴，这个是“重载”了json.hpp里的某个方法
	Dir* Computer::get_root() const {
		return root.get();
	}
	Computer::~Computer() {

	}

	void Session::connect_to(Computer* target) {
		current_computer = target;
		current_dir = target->get_root();
	}
	void Session::change_dir(Dir* target) {
		current_dir = target;
	}
	Session::~Session() {
		current_computer = nullptr;
		current_dir = nullptr;
	}
}

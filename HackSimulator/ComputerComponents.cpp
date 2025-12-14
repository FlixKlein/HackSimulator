/*
|	HackSimulator v0.0.6
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
		login = usrn;
		ip = id_to_ip(id);
		ip_port[21] = false;
		ip_port[22] = false;
		ip_port[80] = false;
		ip_port[88] = false;
		ip_port[443] = false;
	}
	const string Computer::get_ip() const {
		return ip;
	}
	string& Computer::get_ip() {
		return ip;
	}
	const map<int, bool> Computer::get_ip_port() const {
		return ip_port;
	}
	map<int, bool>& Computer::get_ip_port() {
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
	unordered_map<string, Account>& Computer::get_account() {
		return accounts;
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
	void Computer::write_log(const LogTarget l,const string& s){
		string log_ss;
		log_ss = "[" + get_time() + "] " + s;
		string filename;
		switch(l){
			case LogTarget::System:
				filename = "System.log";
				break;
			case LogTarget::Command:
				filename = "Command.log";
				break;
			case LogTarget::Application:
				filename = "Application.log";
				break;
			case LogTarget::Network:
				filename = "Network.log";
				break;
		}
		auto it = root->locate_dir_from_now("log");
		if (!it) {
			root->add_dir(make_unique<Dir>("log", root.get()));
			it = root->locate_dir_from_now("log");
		}
		auto log_file = it->locate_file_from_now(filename);
		if(!log_file){
			it->add_file(make_unique<File>(filename, vector<string>{}));
			log_file = it->locate_file_from_now(filename);
		}
		log_file->content.push_back(to_utf8(log_ss));
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

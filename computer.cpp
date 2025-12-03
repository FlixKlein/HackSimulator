#include "cmulator.h"

namespace ComputerDomain{
	using namespace std;
	using namespace Filesystem;
	int Computer::id = 1;
	string Computer::id_to_ip(const int i){
		random_device rd;
		mt19937 gen(rd());
		uniform_int_distribution<int> dist_int1(1,255);
		uniform_int_distribution<int> dist_int2(1,10000);
		string res = to_string(dist_int1(gen)) + "." + \
						to_string((dist_int2(gen) + i) % 255) + "." + \
							to_string(dist_int1(gen)) + "." + \
								to_string((dist_int2(gen) + i) % 255);
		return res;
	}
	Computer::Computer(const string n,const string usrn,const string pswd){
		name = n;
		Account account = {
			pswd,
			Permission_level::Admin,
			get_time()
		};
		accounts[usrn] = account;
		id+=1;
		ip = id_to_ip(id);
	}
	const string Computer::get_ip() const {
		return ip;
	}
	const map<int,bool> Computer::get_ip_port() const {
		return ip_port;
	}
	const vector<string> Computer::get_ipconfig() const {
		vector<string> res;
		res.emplace_back("ip address : " + this->ip);
		res.emplace_back("Port list :");
		for(auto& [key,value] : ip_port){
			res.emplace_back(to_string(key) + "\t" + to_string(value));
		}
		return res;
	}
	const map<string,Account> Computer::get_account() const {
		map<string,Account> res;
		for(auto& [key,value] : accounts){
			res[key] = value;
		}
		return res;
	}
	//account的序列化实现，由于ADL(参数依赖查找，json那个库使用的）特性，我必须把它放在Account所在的命名空间里
	void to_json(json& j,const Account& acc){
		j = json{
			{"password",acc.password},
			{"permission",acc.permission},
			{"last_login_time",acc.last_login_time}
		};
	}
	void from_json(const json& j,Account& acc){
		j.at("password").get_to(acc.password);
		j.at("permission").get_to(acc.permission);
		j.at("last_login_time").get_to(acc.last_login_time);
	}
	
	Dir* Computer::get_root() const {
		return root.get();
	}
	Computer::~Computer(){
		
	}
	
	void Session::connect_to(Computer* target){
		current_computer = target;
		current_dir = target->get_root();
	}
	void Session::change_dir(Dir* target){
		current_dir = target;
	}
}

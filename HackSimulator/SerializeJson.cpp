/*
|	HackSimulator v0.0.6
|	
|	SerializeJson.cpp
|	this cpp implements JSON serialization and deserialization functions
|	这个文件实现JSON序列化和反序列化函数
|	
|	https://github.com/FlixKlein/HackSimulator
|	https://gitee.com/rosemarychn/HackSimulator
|	License : MIT
|	Personal Blog : https://undertopia.top
*/
#include "hacksimulator.h"

namespace SerializeJson{
	using json = nlohmann::json;
	using namespace std;
	using namespace FileSystem;
	using namespace ComputerComponents;
	using namespace Net;
	//游戏全局序列化
	json serialize_file(const File& file){
		return json{
			{"name",file.name},
			{"content",file.returnContent()},
			{"file_hash",file.file_hash}
		};
	}
	json serialize_dir(const Dir& dir){
		json j;
		j["name"] = dir.name;
		for(const auto& [name,file_ptr] : dir.get_subfile()){
			j["files"][name] = serialize_file(*file_ptr);
		}
		for(const auto& [name,dir_ptr] : dir.get_subdir()){
			j["directories"][name] = serialize_dir(*dir_ptr);
		}
		return j;
	}
	json serialize_packet(const Packet& packet){
		return json{
			{"source_ip",packet.source_ip},
			{"dest_ip",packet.dest_ip},
			{"payload",packet.payload},
			{ "ttl",packet.ttl },
			{"metadata",packet.metadata }
		};
	}
	json serialize_netnode(const NetNode& netnode) {
		json j;
		j["ip"] = netnode.ip;
		j["name"] = netnode.name;
		j["online"] = netnode.online;
		j["resource"] = netnode.resource; 
		j["ports"] = netnode.ports;
		for(auto& packet : netnode.packets){
			j["packets"].push_back(serialize_packet(packet));
		}
		return j;
	}
	json serialize_computer(const Computer& computer){
		return json{
			{"name",computer.name},
			{"accounts",computer.get_account()},
			{"login",computer.login},
			{"ip",computer.get_ip()},
			{"ip_port",computer.get_ip_port()},
			{"dirtree",serialize_dir(*computer.get_root())}
		};
	}
	void save_world(const vector<Computer>& world_computer,const unordered_map<string,unique_ptr<Net::NetNode>>& netnodes,const string filename){
		json world_json;
		for(const auto& world_cpt : world_computer){
			world_json["world_computer"].push_back(serialize_computer(world_cpt));
		}
		for (const auto& [ip, netnode] : netnodes) {
			world_json["netnodes"][ip] = serialize_netnode(*netnode.get());
		}
		world_json["prolouge_num"] = prolouge_num;
		world_json["is_chinese"] = is_chinese;
		ofstream file(filename);
		file << world_json.dump(4);
	}
	unique_ptr<File> deserialize_file(const json& j){
		string name = j.at("name").get<string>();
		vector<string> content;
		if(j.contains("content")){
			content = j.at("content").get<vector<string>>();
		}
		string file_hash = j.at("file_hash").get<string>();
		unique_ptr<File> file = make_unique<File>(name, content);
		file->file_hash = file_hash;
		return file;
	}
	unique_ptr<Dir> deserialize_dir(const json& j,Dir* fath){
		string name = j.at("name").get<string>();
		auto new_dir = make_unique<Dir>(name,fath);
		if(j.contains("files")){
			for(auto& [key,file_json] : j["files"].items()){
				new_dir->add_file(deserialize_file(file_json));
			}
		}
		if(j.contains("directories")){
			for(auto& [key,dir_json] : j["directories"].items()){
				new_dir->add_dir(deserialize_dir(dir_json,new_dir.get()));
			}
		}
		return new_dir;
	}
	Packet deserialize_packet(const json& j){
		string source_ip = j.at("source_ip").get<string>();
		string dest_ip = j.at("dest_ip").get<string>();
		string payload = j.at("payload").get<string>();
		int ttl = j.at("ttl").get<int>();
		unordered_map<string, string> metadata;
		if(j.contains("metadata")){
			metadata = j.at("metadata").get<unordered_map<string, string>>();
		}
		Packet packet(source_ip, dest_ip, payload);
		packet.ttl = ttl;
		packet.metadata = metadata;
		return packet;
	}
	unique_ptr<NetNode> deserialize_netnode(const json& j,Computer* comp) {
		string ip = j.at("ip").get<string>();
		string name = j.at("name").get<string>();
		bool online = j.at("online").get<bool>();
		int resource = j.at("resource").get<int>();
		map<int, bool> ports = j.at("ports").get<map<int, bool>>();
		unique_ptr<NetNode> netnode = make_unique<NetNode>(ip, name);
		netnode->online = online;
		netnode->resource = resource;
		netnode->ports = ports;
		netnode->host = comp;
		if (j.contains("packets")) {
			for (const auto& packet_json : j["packets"]) {
				netnode->packets.push_back(deserialize_packet(packet_json));
			}
		}
		return netnode;
	}
	Computer deserialize_computer(const json& j){
		string name = j["name"].get<string>();
		string ip = j["ip"].get<string>();
		string login = j["login"].get<string>();
		map<int,bool> ip_port = j["ip_port"].get<map<int,bool>>();
		Computer new_comp(name);
		new_comp.ip = ip;
		new_comp.ip_port = ip_port;
		new_comp.login = login;
		if(j.contains("accounts")){
			new_comp.accounts = j.at("accounts").get<unordered_map<string,Account>>();//注意这里返回类型
		}
		if(j.contains("dirtree")){
			new_comp.root = deserialize_dir(j.at("dirtree"),nullptr);
		}
		return new_comp;
	}
	vector<Computer> load_world(const string filename){
		ifstream file(filename);
		if(!file.is_open()){
			cout<<"配置文件加载错误！"<<endl;
			return {};//还给默认构造方法
		}
		json world_json = json::parse(file);
		vector<Computer> world;
		if(world_json.contains("world_computer")){
			//这里循环加载comp
			for(const auto& comp_json : world_json["world_computer"]){
				world.push_back(deserialize_computer(comp_json));
			}
		}
		is_chinese = world_json["is_chinese"].get<bool>();
		prolouge_num = world_json["prolouge_num"].get<int>();
		return world;
	}
	unordered_map<string, unique_ptr<NetNode>> load_net(const string filename) {
		ifstream file(filename);
		if (!file.is_open()) {
			cout << "配置文件加载错误！" << endl;
			return {};
		}
		json net_json = json::parse(file);
		unordered_map<string, unique_ptr<NetNode>> net;
		if (net_json.contains("netnodes")) {
			for (auto& [ip, netnode_json] : net_json["netnodes"].items()) {
				if (netnodes.count(ip) > 0) {
					continue;//已经绑定过了
				}
				unique_ptr<NetNode> netnode = deserialize_netnode(netnode_json, nullptr);
				net[ip] = move(netnode);
			}
			for (auto& computer : world_computers) {
				net[computer.get_ip()]->bind_host(&computer);
			}
		}
		return net;
	}
}

/*
|	HackSimulator v0.0.5
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
	json serialize_computer(const Computer& computer){
		return json{
			{"name",computer.name},
			{"accounts",computer.get_account()},
			{"ip",computer.get_ip()},
			{"ip_port",computer.get_ip_port()},
			{"dirtree",serialize_dir(*computer.get_root())}
		};
	}
	void save_world(const vector<Computer>& world_computer,const string filename){
		json world_json;
		for(const auto& world_cpt : world_computer){
			world_json["world_computer"].push_back(serialize_computer(world_cpt));
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
	Computer deserialize_computer(const json& j){
		string name = j["name"].get<string>();
		string ip = j["ip"].get<string>();
		map<int,bool> ip_port = j["ip_port"].get<map<int,bool>>();
		Computer new_comp(name);
		new_comp.ip = ip;
		new_comp.ip_port = ip_port;
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
}

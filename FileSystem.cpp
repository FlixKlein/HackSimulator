/*
|	HackSimulator v0.0.4
|	
|	FileSystem.cpp
|	
|	https://github.com/FlixKlein/HackSimulator
|	https://gitee.com/rosemarychn/HackSimulator
|	License : MIT
|	Personal Blog : https://undertopia.top
*/
#include "hacksimulator.h"

namespace FileSystem{
	//File的函数实现
	vector<string> File::returnContent() {
		return this->content;
	}
	const vector<string> File::returnContent() const {
		return this->content;
	}
	File::~File(){
		
	}
	
	//Dir的函数实现
	/*Dir::Dir(const Dir& other) : name(other.name),fath(nullptr){
		for(const auto& [key,value] : other.sub_file){
			this->sub_file[key] = make_unique<File>(value->name,value->returnContent());
		}
	}
	Dir& Dir::operator=(const Dir& other){
		if(this == &other){
			return *this;
		}
		for(auto& [key,value] : sub_file) value.reset();//这里做示范智能指针auto前面不写const，指针要写
		for(auto& [key,value] : sub_dir) value.reset();
		sub_file.clear();
		sub_dir.clear();
		this->name = other.name;
		this->fath = nullptr;
		for(const auto& [key,value] : other.sub_file){
			this->sub_file[key] = make_unique<File>(value->name,value->returnContent());
		}
		for(const auto& [key,value] : other.sub_dir){
			this->sub_dir[key] = make_unique<Dir>(value->name);
			this->sub_dir[key]->fath = this;
		}
		return *this;
	}*/
	//需要访问private成员，作为成员函数
	const unordered_map<string,unique_ptr<Dir>>& Dir::get_subdir() const {
		return sub_dir;
	}
	const unordered_map<string,unique_ptr<File>>& Dir::get_subfile() const {
		return sub_file;
	}
	Dir* Dir::locate_dir_from_now(const string dir_name){
		auto it = sub_dir.find(dir_name);
		if(it == sub_dir.end()) return nullptr;
		return it->second.get();
	}
	File* Dir::locate_file_from_now(const string file_name){
		auto it = sub_file.find(file_name);
		if(it == sub_file.end()) return nullptr;
		return it->second.get();
	}
	const Dir* Dir::locate_dir_from_now(const string dir_name) const {
		auto it = sub_dir.find(dir_name);
		if(it == sub_dir.end()) return nullptr;
		return it->second.get();
	}
	const File* Dir::locate_file_from_now(const string file_name) const {
		auto it = sub_file.find(file_name);
		if(it == sub_file.end()) return nullptr;
		return it->second.get();
	}
	unique_ptr<Dir> Dir::clone(Dir* fath) const {
		auto new_dir = make_unique<Dir>(name,fath);
		for(const auto& [key,value] : sub_file ){
			new_dir->sub_file[key] = make_unique<File>(value->name,value->returnContent());
		}
		for(const auto& [key,value] : sub_dir){
			new_dir->sub_dir[key] = value->clone(new_dir.get());
		}
		return new_dir;
	}
	unique_ptr<Dir> Dir::clone() const {
		return this->clone(nullptr);
	}
	Dir* Dir::locate_dir_from_path(const string dir_path){
		if(dir_path.empty()) return this;
		Dir* current_node = this;
		string path_to_parsed = dir_path;
		if(dir_path[0] == '/'){
			while(current_node->fath != nullptr){
				current_node = current_node->fath;
			}
			path_to_parsed = dir_path.substr(1);
		}
		if(path_to_parsed.empty()) return current_node;
		vector<string> sub_path = split(path_to_parsed,'/');
		for(auto it : sub_path){
			if(it.empty()||it=="."){
				continue;
			}
			if(it == ".."){
				if(current_node->fath != nullptr) current_node = current_node->fath;
			} else {
				Dir* next_node = current_node->locate_dir_from_now(it);
				if(next_node == nullptr){
					return nullptr;
				}
				current_node = next_node;
			}
		}
		return current_node;
	}
	const Dir* Dir::locate_dir_from_path(const string dir_path) const {
		if(dir_path.empty()) return this;
		Dir* current_node = const_cast<Dir*>(this);
		string path_to_parsed = dir_path;
		if(dir_path[0] == '/'){
			while(current_node->fath != nullptr){
				current_node = current_node->fath;
			}
			path_to_parsed = dir_path.substr(1);
		}
		if(path_to_parsed.empty()) return current_node;
		vector<string> sub_path = split(path_to_parsed,'/');
		for(auto it : sub_path){
			if(it.empty()||it=="."){
				continue;
			}
			if(it == ".."){
				if(current_node->fath != nullptr) current_node = current_node->fath;
			} else {
				Dir* next_node = current_node->locate_dir_from_now(it);
				if(next_node == nullptr){
					return nullptr;
				}
				current_node = next_node;
			}
		}
		return current_node;
	}
	File* Dir::locate_file_from_path(const string file_path){
		if(file_path.empty()||file_path.back()=='/'){
			return nullptr;
		}
		size_t last_slash_pos = file_path.find_last_of('/');
		string dir_path;
		string name;
		if(last_slash_pos == string::npos){
			dir_path = ".";
			name = file_path;
		} else {
			dir_path = file_path.substr(0,last_slash_pos);
			name = file_path.substr(last_slash_pos+1);
			if(dir_path.empty()){
				dir_path = "/";
			}
		}
		Dir* current_node = locate_dir_from_path(dir_path);
		if(current_node != nullptr){
			return current_node->locate_file_from_now(name);
		}
		return nullptr;
	}
	const File* Dir::locate_file_from_path(const string file_path) const {
		if(file_path.empty()||file_path.back()=='/'){
			return nullptr;
		}
		size_t last_slash_pos = file_path.find_last_of('/');
		string dir_path;
		string name;
		if(last_slash_pos == string::npos){
			dir_path = ".";
			name = file_path;
		} else {
			dir_path = file_path.substr(0,last_slash_pos);
			name = file_path.substr(last_slash_pos+1);
			if(dir_path.empty()){
				dir_path = "/";
			}
		}
		Dir* current_node = const_cast<Dir*>(locate_dir_from_path(dir_path));
		if(current_node != nullptr){
			return current_node->locate_file_from_now(name);
		}
		return nullptr;
	}
	bool Dir::delete_dir(const string dir_name){
		size_t erase_count = sub_dir.erase(dir_name);
		if(erase_count > 0){
			return true;
		}
		return false;
	}
	bool Dir::delete_file(const string file_name){
		size_t erase_count = sub_file.erase(file_name);
		if(erase_count > 0){
			return true;
		}
		return false;
	}
	const string Dir::show_path() const{
		if(this->fath == nullptr) return "/";
		stringstream res;
		list<string> components;
		const Dir* current = this;
		while(current->fath != nullptr){
			components.emplace_front("/" + current->name);
			current = current->fath;
		}
		for(auto it : components) res<<it;
		return res.str();
	}
	bool Dir::is_ancestor_of(const Dir* target,const Dir* current) const {
		if(target == nullptr || current == nullptr) return false;
		const Dir* it = current;
		while(it != nullptr){
			if(it == target){
				return true;
			}
			it = it->fath;
		}
		return false;
	}
	bool Dir::name_exist(const string name) const {
		if(sub_dir.count(name) || sub_file.count(name)) {
			return true;
		}
		return false;
	}
	/*写一下用户可能出现的目录输入
	/aaa/bbb/ccc
	aaa/bbb/ccc
	/aaa/
	./aaa
	../aaa
	/aaa/.
	/aaa/..
	/aaa
	刚发现用户会输入危险的行为，也就是当前/aaa/bbb/ccc用户删除/aaa/bbb这种，遂加入一个检查祖先的函数
	*/
	bool Dir::delete_dir_from_path(const string dir_path){
		Dir* target_node = this->locate_dir_from_path(dir_path);
		if(target_node != nullptr){//有这个目录才进行接下来的操作
			if(dir_path.empty() || dir_path == "." || dir_path == ".." || dir_path == "/" ) return false;
			size_t last_slash = dir_path.find_last_of('/');
			if(last_slash == string::npos) return this->delete_dir(dir_path);//在当前
			string fath_path = dir_path.substr(0,last_slash);
			string target = dir_path.substr(last_slash+1);
			if(fath_path.empty()) fath_path = "/";
			if(target == "." || target == ".." || target.empty()) return false;
			if(is_ancestor_of(target_node,this)){
				cout<<"试图删除父目录或者当前目录！！"<<endl;
				return false;
			}
			target_node = this->locate_dir_from_path(fath_path);
			return target_node->delete_dir(target);
		}
		return false;//根本没有这个路径
	}
	bool Dir::delete_file_from_path(const string file_path){
		File* target_node = this->locate_file_from_path(file_path);
		if(target_node != nullptr){
			if(file_path.empty() || file_path =="." || file_path ==".." || file_path == "/") return false;
			size_t last_slash = file_path.find_last_of('/');
			if(last_slash == string::npos) return this->delete_file(file_path);
			string fath_path = file_path.substr(0,last_slash);
			string target = file_path.substr(last_slash+1);
			if(fath_path.empty()) fath_path = "/";
			if(target == "." || target == ".." || target.empty()) return false;
			return this->locate_dir_from_path(fath_path)->delete_file(target);
		}
		return false;
	}
	bool Dir::add_dir(unique_ptr<Dir> other){
		if(name_exist(other->name)){
			cout<<"禁止创建同名文件或文件夹！！"<<endl;
			return false;
		}
		if(!other) return false;
		const string& name = other->name;
		if(sub_dir.count(name)){
			return false;
		}
		sub_dir[name] = move(other);
		sub_dir[name]->fath = this;
		return true;
	}
	bool Dir::add_file(unique_ptr<File> other){
		if(name_exist(other->name)){
			cout<<"禁止创建同名文件或文件夹！！"<<endl;
			return false;
		}
		if(!other) return false;
		const string& name = other->name;
		if(sub_file.count(name)){
			return false;
		}
		sub_file[name] = move(other);
		return true;
	}
	Dir::~Dir(){
		fath = nullptr;
	}
}

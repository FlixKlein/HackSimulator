/*
|	HackSimulator v0.0.4
|	
|	SessionManager.cpp
|	
|	https://github.com/FlixKlein/HackSimulator
|	https://gitee.com/rosemarychn/HackSimulator
|	License : MIT
|	Personal Blog : https://undertopia.top
*/
#include "hacksimulator.h"

using namespace ComputerComponents;
using namespace std;
int SessionManager::create_session(Computer* comp){
	if(comp == nullptr){
		cout<<"尝试将会话连接到无效的电脑"<<endl;
		return -1;
	}
	lock_guard<mutex> lock(sessions_mutex);
	int new_id = new_session_id++;
	auto new_session = make_unique<Session>();
	new_session->connect_to(comp);
	sessions[new_id] = move(new_session);
	return new_id;
}
Session* SessionManager::get_session(int id){
	lock_guard<mutex> lock(sessions_mutex);
	auto it = sessions.find(id);
	if(it != sessions.end()){
		return it->second.get();
	}
	return nullptr;
}
void SessionManager::list_session() {
	lock_guard<mutex> lock(sessions_mutex);
	cout<<"ID\tStatus\tComputer\tPath"<<endl;
	for(const auto& [id,sess] : sessions){
		cout<<id<<"\t";
		cout<<(sess->is_busy ? "Busy" : "Idle")<<"\t";
		if(sess->current_computer){
			cout<<sess->current_computer->name<<"\t\t"<<sess->current_dir->show_path();
		} else {
			cout<<"[N/A]\t\tN/A";
		}
		cout<<endl;
	}
}

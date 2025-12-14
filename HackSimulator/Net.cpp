#include "hacksimulator.h"
#include "UIManager.h"

namespace Net {
	using namespace std;
	using namespace ComputerComponents;
	
	void NetNode::clear_packets() {
		packets.clear();
	}
	bool NetNode::receive(Packet& packet) {
		if(resource >= packet.cosumed_resource){
			resource -= packet.cosumed_resource;
		} else {
			//资源不足，丢包
			cout << "NetNode " << ip << " 资源不足，丢弃数据包" << endl;
			return false;
		}
		packet.ms += 10; //基础延迟
		string log_ss;
		log_ss = "接收数据包来自 " + packet.dest_ip + " , 延迟 " + to_string(packet.ms) + " ms" \
			+ ", TTL " + to_string(packet.ttl) + " payload: " + packet.payload;
		host->write_log(LogTarget::Network, log_ss);
		packets.push_back(packet);
		return true;
	}
	bool NetNode::send(Packet& packet) {
		if (netnodes.count(packet.dest_ip) > 0) {
			DelayMs delay(uniform_int_distribution<int>(10, 500)(rng));
			Sleep(delay.count());
			packet.ms += delay.count();
			packet.ttl -= 1;
			if(packet.ttl <= 0){
				//丢包
				return false;
			}
			string log_ss;
			log_ss = "发送数据包到 " + packet.dest_ip + " , 延迟 " + to_string(packet.ms) + " ms" \
					+ ", TTL " + to_string(packet.ttl) + " payload: " + packet.payload;
			host->write_log(LogTarget::Network, log_ss);
			return netnodes[packet.dest_ip]->receive(packet);
		}
		return false;
	}
	bool NetNode::bind_host(Computer* computer) {
		host = computer;
		return true;
	}
	bool NetNode::unbind_host() {
		host = nullptr;
		return true;
	}
	bool NetNode::open_port(int port) {
		ports[port] = true;
		return true;
	}
	bool NetNode::close_port(int port) {
		ports[port] = false;
		return true;
	}
}
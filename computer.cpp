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
		username = usrn;
		password = pswd;
		id+=1;
		ip = id_to_ip(id);
	}
	Computer::~Computer(){
		
	}
}

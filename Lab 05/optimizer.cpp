#include <iostream>
#include <vector>
#include <string>
#include <fstream>

using namespace std;

class Block{
	vector<string> code;
	Block* F;
	Block* T;
	Block(){*F = NULL; *T = NULL;};
	~Block(){delete F; delete T;};
};

int main(){
	string line;
	vector<string> all_code;

	fstream myfile;
	myfile.open("all_test.c.ic");

	if(myfile.is_open()){
		while(getline(myfile, line))
			all_code.push_back(line);
		myfile.close();
	}

	count = 0;
	
	Block B1 = new Block;
	for(int i=0; i<all_code.size(); i++){
		// all_code[i].substring
		size_t found = str.find(str2);
  		if (found!=string::npos){
  			// New	Block()
  		}
  		else{
  			.push_back(all_code[i]);
  		}
	}

	// for(int i=0; i<all_code.size(); i++){
	// 	cout << all_code[i] << endl;
	// }
}
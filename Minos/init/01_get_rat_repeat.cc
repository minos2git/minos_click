#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <string>
#include <map>
#include <algorithm>
#include <vector>

using namespace std;



int main(int argc, char * argv[])
{

	char line[1500]={0};
	map<string, int> count_act;
	map<string, int>::iterator it_act;
	
	map<int, int> comp_repeat; //<cid, repeat> the max repeat time
	map<int, int>::iterator it_repeat;

	char filename[15] = "legal_AIDs";
	FILE *fp1 = freopen(filename, "r", stdin);
	if (fp1 == NULL)
		cout<< "file open fail.\n";

	while (fgets(line, 1500, fp1))
	{
		string str_line(line);
		it_act = count_act.find(str_line);
		if (it_act != count_act.end()) continue; //exist act
		else count_act[str_line] =1;
		
		//cout << "tcp line :"<<line<< "\n ";
		map<int, int> tmp_repeat; //<id, repeat> in an action
		map<int, int>::iterator  tmp_it; 
		tmp_repeat.clear();

		char id[30] ={0};
		int i=0, j=0;
		while (line[i] != '\n' ){
			if (line[i] != ' ') {id[j++] = line[i++]; }
			else {
				id[j++] = '\0';
				j=0; i++;
				if (strlen(id)==1 && id[0] == '0') break;

				char *endp;
				int tmp_id = strtol(id, &endp, 10);
				if (tmp_id == 0) continue;
				
				tmp_it = tmp_repeat.find(tmp_id);
				if (tmp_it != tmp_repeat.end()) //exist
					tmp_repeat[tmp_id]++;
				else tmp_repeat[tmp_id] = 1;

			}
		}
		for (tmp_it = tmp_repeat.begin(); tmp_it != tmp_repeat.end(); tmp_it++){
			int comp_id = tmp_it->first;
			int repeat = tmp_it->second;
			it_repeat = comp_repeat.find( comp_id );
			if (it_repeat != comp_repeat.end()){ //exist
				if (repeat > comp_repeat[comp_id])
					comp_repeat[comp_id] = repeat;
			}
			else 
				comp_repeat[comp_id] = repeat;
		}

	}
	fclose(fp1);



	//for (it_repeat = comp_repeat.begin(); it_repeat != comp_repeat.end(); it_repeat++)
		//cout << it_repeat->first <<"  "<< it_repeat->second << "\n";

	freopen("init_repeat.h", "w", stdout);
	cout<<"HashTable<int, int> Router::init_repeat_map(){\n";
	cout<<"\tHashTable<int, int> tmp_m;//<cid, repeat times> \n//including the first time; the cid is added by repeat times\n";
	cout<<"//Using this file needs to set check_repeat in Attach_CTAG().\n";
	int accelerate_cid = 0;
	for (it_repeat = comp_repeat.begin(); it_repeat != comp_repeat.end(); it_repeat++){
		if (it_repeat->second > 1) {
			cout<<"\ttmp_m["<<  it_repeat->first + accelerate_cid  <<"]= "<< it_repeat->second <<";\n";
			accelerate_cid += it_repeat->second - 1; //normal cid has alread added 1, so minus 1;
		}
	}
	cout<<"\treturn tmp_m;\n}";
	fclose (stdout);
	
	return 1;
}

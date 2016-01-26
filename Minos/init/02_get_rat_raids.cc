#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <string>
#include <map>
#include <algorithm>
#include <vector>

using namespace std;


#define HashTable std::map 

class Router {
	public:
		static map<int, int>init_repeat_map(); //<cid, repeat times>
};
#include "init_repeat.h"


class Get_RAID {
public:
	map<string, int>  act_count; //<action, times>
	map<string, int>::iterator  it3; 
	map<int, int> original_new_cid; //<original cid, new cid with considering repeated cids>
	map<int, int>::iterator it_cid; 

	int comp_ids_in_line[100][40]; //the [0] element of act stores the length of the act, not Action Weight.
	int act_n;
		
	map<int, int>rep_map; //<cid, repeat times>

	void get_acts();
	void print_acts();
	void print_acts2();
	void consider_repeated_cids();
	void print_RAIDs();
	void run();
};

void Get_RAID::get_acts()
{
	freopen("legal_AIDs", "r", stdin);


	act_n=0;
	char line[1500]={0};
	int comp_n_of_act=0;
	while (fgets(line, 1500, stdin))
	{
		//cout << "line: "<<line<< ", len="<< strlen(line) <<"\n";
		//cout << "last char of line: "<< line[strlen(line-1)] << "\n";
		char id[30] ={0};
		char pre_id[60] = {0};
		string str_line(line);
		it3 = act_count.find(str_line);
		if (it3 != act_count.end()) continue; //exist act
		else act_count[str_line] =1;
		
		comp_n_of_act = 1;
		int i=0, j=0;
		while (line[i] != '\n' ){
			if (line[i] != ' ') {id[j++] = line[i++]; }
			else {
				id[j++] = '\0';
				j=0; i++;
				if (strlen(id)==1 && id[0] == '0') break;

				char *endp;
				int id_2 = strtol(id, &endp, 10);
				comp_ids_in_line[act_n][comp_n_of_act ]=id_2;
				comp_n_of_act ++;
				original_new_cid [id_2] = id_2;
			}
		}
		comp_ids_in_line[act_n][0]=comp_n_of_act -1;


		//cout << "line: "<<line<< ", len="<< strlen(line) <<"\n";
		//for (int j=1; j<=comp_ids_in_line[act_n][0]; j++)
			//printf("%d ", comp_ids_in_line[act_n][j]);
		//printf("\n");

		act_n++;
		memset(line, 1500, 0);
	}
	fclose(stdin);

}


void Get_RAID::print_acts()
{
	map<int, int>::iterator rep_it;

	printf("repeat times: \n");
	for (rep_it = rep_map.begin(); rep_it != rep_map.end(); rep_it++)
		printf("cid %d, rep time %d\n", rep_it->first, rep_it->second);
	
	printf("cids number: %ld\n", original_new_cid.size());
	for (it_cid = original_new_cid.begin(); it_cid!= original_new_cid.end(); it_cid++)
		printf("%d ", it_cid->first);
	printf("\n");

	printf("different act number: %d\n", act_n);
	for (int i=0; i<act_n; i++){
		printf("\nact %d: ", i);
		for (int j=1; j<=comp_ids_in_line[i][0]; j++)
			printf("%d ", comp_ids_in_line[i][j]);
	}
	printf("\n");
}

void Get_RAID::print_acts2()
{
	printf("after considering repeated cids, different act number: %d\n", act_n);
	for (int i=0; i<act_n; i++){
		printf("\nact %d: ", i);
		for (int j=1; j<=comp_ids_in_line[i][0]; j++)
			printf("%d ", comp_ids_in_line[i][j]);
	}
	printf("\n");
}

void Get_RAID::print_RAIDs()
{
	freopen("legal_RAIDs", "w", stdout);
	for (int i=0; i<act_n; i++){
		for (int j=1; j<=comp_ids_in_line[i][0]; j++)
			printf("%d ", comp_ids_in_line[i][j]);
		printf("0 \n");
	}
	fclose(stdout);
}

void Get_RAID::consider_repeated_cids()
{
	for (int i=0; i<act_n; i++){
		for (int j=1; j<=comp_ids_in_line[i][0]; j++)
		{
			for (map<int, int>::iterator rep_it = rep_map.begin(); rep_it != rep_map.end(); rep_it++)
			{
				if (comp_ids_in_line[i][j] > rep_it->first)
					comp_ids_in_line[i][j] += rep_it->second - 1;
			}
			int cur_cid = comp_ids_in_line[i][j];
			for (int k=1; k<j; k++)
				if (comp_ids_in_line[i][k] == cur_cid) cur_cid++;
			comp_ids_in_line[i][j] = cur_cid;
		}
	}
}

void Get_RAID::run()
{
	rep_map = Router::init_repeat_map();
	get_acts();
	//printf("before considering repeat cids.\n");
	//print_acts();
	consider_repeated_cids();
	//print_acts2();
	print_RAIDs();
}

int main(int argc, char * argv[])
{
	Get_RAID g;
	g.run();
	

	return 1;
}

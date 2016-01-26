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


class Get_Update_AID {
public:
	map<string, int>  act_count; //<action, times>
	map<string, int>::iterator  it3; 
	map<int, int> original_new_cid; //<original cid, new cid with considering repeated cids>
	map<int, int>::iterator it_cid; 

	int comp_ids_in_line[100][40]; //the [0] element of act stores the length of the act, not Action Weight.
	int act_n;
		
	map<int, int>rep_map; //<cid, repeat times>
	map<int, int>aid_map; //<raid_cid, aid_cid>
	int max_raid_cid;

	void get_acts();
	void print_acts();
	void print_acts2();
	void consider_repeated_cids();
	void print_RAIDs();
	void run(map<int, string> comp_c);
	void get_aid_cid();
	void print_update_aid_cid();
	void print_update(map<int, string> comp_c);
};

void Get_Update_AID ::get_acts()
{
	freopen("00_update_RAIDs", "r", stdin);
	max_raid_cid = -1;


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
				if (id_2 > max_raid_cid ) max_raid_cid  = id_2;
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


void Get_Update_AID ::print_acts()
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

void Get_Update_AID ::print_acts2()
{
	printf("after considering repeated cids, different act number: %d\n", act_n);
	for (int i=0; i<act_n; i++){
		printf("\nact %d: ", i);
		for (int j=1; j<=comp_ids_in_line[i][0]; j++)
			printf("%d ", comp_ids_in_line[i][j]);
	}
	printf("\n");
}

void Get_Update_AID ::print_RAIDs()
{
	freopen("legal_RAIDs", "w", stdout);
	for (int i=0; i<act_n; i++){
		for (int j=1; j<=comp_ids_in_line[i][0]; j++)
			printf("%d ", comp_ids_in_line[i][j]);
		printf("0 \n");
	}
	fclose(stdout);
}

void Get_Update_AID ::consider_repeated_cids()
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

void Get_Update_AID ::get_aid_cid()
{

	int increment = 0;
	int cur_aid_cid = 1;
	int cur_raid_cid = 1;
	for (map<int, int>::iterator rep_it = rep_map.begin(); rep_it != rep_map.end(); rep_it++)
	{
		int cur_rep_raid_cid = rep_it->first;
		int cur_rep_time = rep_it->second;
		int temp_aid_cid;

		for (;cur_raid_cid <= cur_rep_raid_cid ; cur_raid_cid++)
		{
			temp_aid_cid = cur_raid_cid - increment ;
			aid_map[cur_raid_cid] = temp_aid_cid ; 
		}
		for (int i=1; i<cur_rep_time; i++, cur_raid_cid++ )
		{
			aid_map[cur_raid_cid] = temp_aid_cid ; 
		}
		increment += cur_rep_time - 1;
	}
	for (;cur_raid_cid <= max_raid_cid ; cur_raid_cid++)
		aid_map[cur_raid_cid]= cur_raid_cid - increment ;

}

void Get_Update_AID ::print_update_aid_cid()
{
	for (map<int, int>::iterator raid_it = aid_map.begin(); raid_it != aid_map.end(); raid_it ++)
	{
		cout << "raid " <<raid_it->first <<" -> " << raid_it->second << "\n";
	}
}

void Get_Update_AID ::print_update(map<int, string> comp_cids)
{
	for (map<int, string>::iterator it = comp_cids.begin(); it != comp_cids.end(); it ++)
	{
		//comp_cids[1] = "packet_make1";
		cout << "comp_cids[" << aid_map[it->first] << "] = \"" << it->second <<"\";\n";
	}

	printf("different act number: %d\n", act_n);
	for (int i=0; i<act_n; i++){
		printf("\n %d act :", i);
		for (int j=1; j<=comp_ids_in_line[i][0]; j++)
			printf( "%d ", aid_map[comp_ids_in_line[i][j]] );
	}
	printf("\n");
}

void Get_Update_AID ::run(map<int, string> comp_cids)
{
	rep_map = Router::init_repeat_map();
	get_acts();
	//printf("before considering repeat cids.\n");
	//print_acts();
	//consider_repeated_cids();
	//print_acts2();
	if (comp_cids.size() > max_raid_cid ) max_raid_cid  = comp_cids.size();
	get_aid_cid();
	print_update_aid_cid();
	print_update(comp_cids);
}


int main(int argc, char * argv[])
{
	Get_Update_AID g;

map<int, string> comp_cids; //<cid, name>
#include "00_update_elements.h"

	g.run(comp_cids);
	

	return 1;
}

// get_count_files.cpp : Defines the entry point for the console application.
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



typedef struct edge{
	int act_id;
	int act_head; //simple id //all the ids this program uses are simple ids
	int start_id; //simple id
	int end_id;   //simple id
	int act_len;
	bool visited_tag;
	struct edge* share_next;
	struct edge* share_pre;
	struct edge* edge_of_act_next;
	struct edge* edge_of_act_head;


}edge_t;




#define MAX_COMP_N  300
class solution{
public:	

#define MAX_N_ENTRY  16777216 //2^24 = 16777216 bits: 8 aw, 8 cid1, 8 cid2
	//bool rat_weight_2cids[ 1048576 ]; //2^20 bits
	static bool rat_weight_2cids[MAX_N_ENTRY  ];  


	map<string, int>  count; //<action, times>
	map<string, int>::iterator  it3; 

	map<int, int>  comp_simple_id; //<comp id, simple id of component>
	map<int, int>::iterator it2;
	int  max_actual_id;
	map<int, string> cid_name; //<cid, name>
	map<int, int>repeat_map ; //<cid, repeat times>
	
	int comp_ids_in_line[100][40]; //the [0] element of act stores the length of the act
	edge_t* edge_heads[100] ;
	int act_n;
	int max_comp_n_of_act;
	
	int comp_weight[MAX_COMP_N  ]; //use simple id as index
	int comp_id[MAX_COMP_N  ]; //use simple id as index
	int comp_n;

	//used to find unshare sid
	int in_degree[ MAX_COMP_N ];
	int out_degree[ MAX_COMP_N ];


	//used to reduce rat size
	int rat_count;
	int rat_count_array[500];
	int final_bits_or;
	unsigned int final_bits_and;


	edge_t* relation_topology[MAX_COMP_N  ][MAX_COMP_N  ]; //use simple id as index


	solution(map<int, string> cid_name_map, map<int, int>rep_m);

	void get_act_line();
	
	void get_relation_topoloy();
	void get_relation_topoloy2(); //save comp id in the order of colomn of acts
	
	void get_degrees();
	
	void print_info(int tofile);
	void print_initial_file();
	void print_acts();
	void print_act(int act_i);
	void print_topology(int simple_comp_id);

	void run();

	void seperate_conflicting();

	void get_conflict_acts(int &actid1, int &actid2);
	void seperate_conflicting_using_while();
	int find_unshared_comp(int act_id1, int act_id2);
	void incre_weight(int node_j);

	void get_rat();

	void print_bit(int n_bit, int n);

	int get_even_bit(int n_bit, int n);
	int get_odd_bit(int n_bit, int n);

};

bool solution::rat_weight_2cids[MAX_N_ENTRY  ] = {0};  

solution::solution(map<int, string> cid_name_map, map<int, int>rep_m)
{
	max_actual_id = 0;
	memset(comp_ids_in_line, sizeof(comp_ids_in_line), 0);
	memset(comp_weight, sizeof(comp_weight), 0);
	memset(relation_topology, sizeof(relation_topology), 0);
	memset(edge_heads, sizeof(edge_heads), 0);
	cid_name = cid_name_map;
	repeat_map = rep_m;
}

void solution::get_act_line()
{
	freopen("legal_RAIDs", "r", stdin);

	act_n=0;
	max_comp_n_of_act = 0;
	char line[1500]={0};
	int comp_n_of_act=0;
	while (fgets(line, 1500, stdin))
	{
		//cout << "line: "<<line<< ", len="<< strlen(line) <<"\n";
		//cout << "last char of line: "<< line[strlen(line-1)] << "\n";
		char id[30] ={0};
		char pre_id[60] = {0};
		string str_line(line);
		it3 = count.find(str_line);
		if (it3 != count.end()) continue; //exist act
		else count[str_line] =1;
		
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
			}
		}
		comp_ids_in_line[act_n][0]=comp_n_of_act -1;
		if (comp_n_of_act -1 > max_comp_n_of_act) max_comp_n_of_act = comp_n_of_act -1;
		act_n++;
		memset(line, 1500, 0);
	}
	fclose(stdin);

}
	


void solution::get_relation_topoloy()
{
	comp_n=0;
	int pre_id=0;
	int comp_n_of_act=0;
	for (int i=0; i<act_n; i++)
	{
		comp_n_of_act = comp_ids_in_line[i][0];
		pre_id=0;
		edge_t *pre_edge = NULL;
		edge_t *edge_head = NULL;
		for (int j=1; j<=comp_n_of_act ; j++)
		{
			int id = comp_ids_in_line[i][j];
			if (id > max_actual_id ) max_actual_id = id;
			it2 = comp_simple_id.find(id);
			if (it2==comp_simple_id.end()){ //not exist
				comp_simple_id[id] = comp_n;
				comp_weight[comp_n] = 1;
				comp_id[comp_n] = id;
				comp_n++;
			}
			if (pre_id!=0)
			{
				edge_t *p = new edge_t;
				p->act_id = i;
				p->act_head = comp_simple_id[comp_ids_in_line[i][1]];
				p->start_id = comp_simple_id[pre_id];
				p->end_id   = comp_simple_id[id];
				p->act_len = comp_n_of_act ;
				p->share_next = NULL;
				p->share_pre = NULL;
				p->edge_of_act_next = NULL; 

				if (edge_head == NULL ) { edge_head  = p; edge_heads[i] = p;}
				if ( pre_edge )  pre_edge->edge_of_act_next = p; 
				p->edge_of_act_head = edge_head ; 
				pre_edge = p;

				edge_t *pp = relation_topology[comp_simple_id[pre_id]][comp_simple_id[id]];
				edge_t *pre = NULL;
				while (pp && p->act_len >= pp->act_len)
				{
					pre=pp;
					pp=pp->share_next;
				}
				if (pre == NULL){
					if (pp == NULL)
						relation_topology[comp_simple_id[pre_id]][comp_simple_id[id]] = p;
					else{
						relation_topology[comp_simple_id[pre_id]][comp_simple_id[id]] = p;
						p->share_next = pp; 
						pp->share_pre = p;
					}
				}
				else{
					pre->share_next = p; p->share_pre = pre;
					p->share_next = pp; if(pp) pp->share_pre = p;
				}
			}
			pre_id=id;

		}
	}
	
}

void solution::get_relation_topoloy2()
{/*save comp id in the order of colomn of acts
   for instance, act 0: 1234 3456 5678
                 act 1: 9876 7654 5432 3210
	iterate the nodes in the order as follows: 1234 9876 3456 7654 ...
   */
	comp_n=0;
	for (int i=1; i<=max_comp_n_of_act; i++)
		for (int j=0; j<act_n; j++)
		{
			if (i > comp_ids_in_line[j][0]) continue;
			int id = comp_ids_in_line[j][i];
			it2 = comp_simple_id.find( id );
			if (it2==comp_simple_id.end()){ //not exist
				comp_simple_id[ id ] = comp_n;
				comp_weight[comp_n] = 1;
				comp_id[comp_n] = id;
				comp_n++;
			}
		}

	int pre_id=0;
	int comp_n_of_act=0;
	for (int i=0; i<act_n; i++)
	{
		comp_n_of_act = comp_ids_in_line[i][0];
		pre_id=0;
		edge_t *pre_edge = NULL;
		edge_t *edge_head = NULL;
		for (int j=1; j<=comp_n_of_act ; j++)
		{
			int id = comp_ids_in_line[i][j];
			if (id > max_actual_id ) max_actual_id = id;
			if (pre_id!=0)
			{
				edge_t *p = new edge_t;
				p->act_id = i;
				p->act_head = comp_simple_id[comp_ids_in_line[i][1]];
				p->start_id = comp_simple_id[pre_id];
				p->end_id   = comp_simple_id[id];
				p->act_len = comp_n_of_act ;
				p->share_next = NULL;
				p->share_pre = NULL;
				p->edge_of_act_next = NULL; 

				if (edge_head == NULL ) { edge_head  = p; edge_heads[i] = p;}
				if ( pre_edge ) pre_edge->edge_of_act_next = p;
				p->edge_of_act_head = edge_head ; 
				pre_edge = p;

				edge_t *pp = relation_topology[comp_simple_id[pre_id]][comp_simple_id[id]];
				edge_t *pre = NULL;
				while (pp && p->act_len >= pp->act_len)
				{
					pre=pp;
					pp=pp->share_next;
				}
				if (pre == NULL){
					if (pp == NULL)
						relation_topology[comp_simple_id[pre_id]][comp_simple_id[id]] = p;
					else{
						relation_topology[comp_simple_id[pre_id]][comp_simple_id[id]] = p;
						p->share_next = pp; 
						pp->share_pre = p;
					}
				}
				else{
					pre->share_next = p; p->share_pre = pre;
					p->share_next = pp; if (pp) pp->share_pre = p;
				}
			}
			pre_id=id;

		}
	}
	
}

void solution::get_degrees()
{
	memset(in_degree, 0, sizeof(int)*MAX_COMP_N );
	memset(out_degree, 0, sizeof(int)*MAX_COMP_N );
	
	int comp_n_of_act=0;
	for (int i=0; i<act_n; i++)
	{
		comp_n_of_act = comp_ids_in_line[i][0];
		for (int j=1; j<=comp_n_of_act ; j++)
		{
			int id = comp_ids_in_line[i][j];
			if (j > 1) 
				in_degree [ comp_simple_id[ id ] ] ++;
			if (j < comp_n_of_act ) 
				out_degree [ comp_simple_id[ id ] ] ++;
		}
	}
}


void solution::print_acts()
{
	cout<<"act array, act link list:\n";
	for (int i=0; i<act_n; i++)
	{
		int comp_n_of_act= comp_ids_in_line[i][0];
		printf("i=%02d compn=%02d : ", i, comp_n_of_act);
		int w=0;
		for (int j=1; j<=comp_n_of_act; j++){
			printf("%d ", comp_ids_in_line[i][j]);
			w +=  comp_weight [  comp_simple_id[comp_ids_in_line[i][j]] ];
		}
		cout<<"\nsimple id     : ";
		for (int j=1; j<=comp_n_of_act; j++)
			printf("%d ",   comp_simple_id[comp_ids_in_line[i][j]] );
		cout<<"\n";
		cout<<"w = "<< w <<"        : ";
		for (int j=1; j<=comp_n_of_act; j++)
			printf("%d ", comp_weight [  comp_simple_id[comp_ids_in_line[i][j]] ]);
		cout<<"\n";
		
		edge_t *p = edge_heads[ i ];
		if (p && p->act_len != w) cout<< "not the same len!\n";
		cout<< "i "<< i <<", p "<< p  <<" : ";
		while (p)
		{
			cout << "->"<< p->act_id <<","<< p->act_head <<","<< p->act_len <<","<<p->edge_of_act_head ;
			p = p->edge_of_act_next; 
		}
		cout<<"\n\n";

	}

}

void solution::print_topology(int simple_comp_id)
{

	int c=0;
	cout<<"comp_relation=[\n";
	for (int i=0; i<comp_n; i++){
		printf("%02d :", i);
		for (int j=0; j<comp_n; j++){
			edge_t *p = relation_topology[i][j];
			if (p == NULL)
				cout <<", ";
			else{
				cout <<j<<":";
				while (p){
					if (p->act_id == simple_comp_id) c++;
					cout << "-("<<p->act_head <<","<<p->act_id <<")"<<p->act_len ;
					p=p->share_next;
				}
				cout<<", ";
			}
		}
		cout<< ";\n";
	}
	cout << "];\n";
	cout<<"simple id "<< simple_comp_id <<": appears "<<c <<" times. \n";


}

void solution::print_info(int tofile)
{
	if (tofile==1)
		freopen("get_rat_results", "w", stdout);
	else if (tofile==2)
		freopen("get_rat_results", "a", stdout);
	
	cout<<"comp_simple_id: comp_id  simple_id\n bit16\n bit8\n";
	for (it2 = comp_simple_id.begin(); it2 != comp_simple_id.end(); it2++)
		cout<<it2->first<<" " <<it2->second<<"\n";

	print_acts();			
	/*cout<<"act array, act link list:\n";
	for (int i=0; i<act_n; i++)
	{
		int comp_n_of_act= comp_ids_in_line[i][0];
		cout<< i <<" : "<<comp_n_of_act<<" : ";
		int w=0;
		for (int j=1; j<=comp_n_of_act; j++){
			cout<<comp_ids_in_line[i][j]<<" ";
			w +=  comp_weight [  comp_simple_id[comp_ids_in_line[i][j]] ];
		}
		cout<<"\nw "<< w <<": ";
		for (int j=1; j<=comp_n_of_act; j++)
			cout<< comp_weight [  comp_simple_id[comp_ids_in_line[i][j]] ] <<" ";
		cout<<"\n";
		
		edge_t *p = edge_heads[ i ];
		cout<< i <<","<< p  <<" : ";
		if (p && p->act_len != w) cout<< "not same len!!!";
		while (p)
		{
			cout << "->"<< p->act_id <<","<< p->act_head <<","<< p->act_len <<","<<p->edge_of_act_head ;
			p = p->edge_of_act_next; 
		}
		cout<<"\n\n";

	}*/

	print_topology(0);
	/*
	cout<<"comp_relation=[\n";
	for (int i=0; i<comp_n; i++){
		for (int j=0; j<comp_n; j++){
			if (relation_topology[i][j] == NULL)
				cout <<", ";
			else{
				edge_t *p = relation_topology[i][j];
				while (p){
					cout << "-("<<p->act_head <<","<<p->act_id <<")"<<p->act_len ;
					p=p->share_next;
				}
				cout<<", ";
			}
		}
		cout<< ";\n";
	}
	cout << "];\n";
	*/

	
	cout<<"confused group in edge node (1 to many paths with same length.):\n";
	int total_repeat=0;
	for (int i=0; i<comp_n; i++){
		int row_repeat=0;
		cout<< i << ": ";
		int visit_len[200] = {0};
		int visit_idx = 0;
		for (int j=0; j<comp_n; j++){
			edge_t *p = relation_topology[i][ j ];
			while (p){
				int cur_len = p->act_len;
				int cur_actid = p->act_id;
				int k;
				for (k=0; k<visit_idx; k++)
					if (cur_len == visit_len[k] )break;
				if (k == visit_idx) {//the k len is not visited before
					visit_len[visit_idx++] = cur_len;
					int print_tag=0;
					for (k=0; k<comp_n; k++)
					{
						if (k==j)continue; //do not search itself.
						edge_t *q = relation_topology[i][ k ];
						while (q && q->act_len < cur_len)
							q=q->share_next;
						if (q && q->act_len == cur_len){
							row_repeat++;
							if (print_tag==0){
								print_tag = 1;
								cout << "\ncur i "<<i <<",j "<<j <<",actid "<<cur_actid <<"\n   len "<< cur_len <<"(i "<< i <<", k "<< k << ", actid " << q->act_id <<"),";
							}
							else
								cout << "len "<< cur_len <<"(i "<< i <<", k "<< k << ", actid " << q->act_id <<"),";
						}
					}
				}
				
				//do{
				p=p->share_next;
				//}while (p && p->act_len == cur_len);
			}
		}
		cout<<"\n row repeat="<<row_repeat<<"\n";
		total_repeat += row_repeat;
	}
	cout<<"total repeat="<<total_repeat <<"\n";

	cout<<"comp id weight:\n";
	for (int i=0; i<comp_n; i++){
		//cout<<"comp id:"<< comp_id[i]  <<", weight:"<<  comp_weight[i] <<"\n";
		cout<<"sid:"<<i<<", cid:"<< comp_id[i]  <<", weight:"<<  comp_weight[i] <<", name: "<< cid_name[comp_id[i] ] <<"\n";
		//int even_id = get_even_bit(16, comp_id[i]) & 0xf0; //high 8 digit
		//int odd_id = get_odd_bit(8, comp_id[i]) ; //low 8 digit
		
		//print_bit(16, comp_id[i]);
		//print_bit(16, even_id | odd_id );
	}

	cout<<"len sets:\n";
	map<int, edge_t *> len_sets;
	map<int, edge_t *>::iterator len_it;
	for (int i=0; i<act_n; i++)
	{
		int len = edge_heads[ i ]->act_len;
		len_it = len_sets.find (len);
		edge_t *p = new edge_t;
		p->act_id = i;
		p->act_len = len;
		p->share_next = NULL;
		if (len_it == len_sets.end()) // no exist
		{
			len_sets[len] = p;
		}
		else// exist
		{
			p->share_next = len_sets[len]; 
			len_sets[len] = p;
		}
	}
	for (len_it = len_sets.begin(); len_it != len_sets.end(); len_it++)
	{
		edge_t *p = len_it->second;
		cout << "len " << p->act_len << ":  ";
		//cout << "len " << p->act_len << ": even_len: ";
		//int even_len = get_even_bit(16, p->act_len );
		//print_bit(16, even_len );
		while (p)
		{
			cout << "  act id "<< p->act_id << ", ";
			p=p->share_next;
		}
		cout << "\n";

	}


	cout << "the number of 1 entries of rat " <<rat_count << ", maximum number of entries 2^24 = 16777216 \n";
	cout << "final_bits_or : \n" ;
	print_bit(24, final_bits_or );
	cout << "final_bits_and : \n" ;
	print_bit(24, final_bits_and );
	cout << "\n" ;
	for (int i=0; i<rat_count; i++ ) 
		print_bit(24, rat_count_array[i]);


	if (tofile)
		fclose(stdout);

}

void solution::print_initial_file()
{
	freopen("init_rat_cw.h", "w", stdout);
	freopen("032_size_global_cids", "r", stdin);
	int comps_n;
	scanf("%d", &comps_n);
	fclose(stdin);
	//comps_n +=10;

	//max_actual_id is the max comp_id.
	cout<<"//comp id weight, copms_n="<< comps_n <<" :\n";
	cout<<"int Router::CWs[]={1, \n";
	for (int i=1; i<=max_actual_id ; i++){
		int id, weight ;
		
		id = i;
		it2 = comp_simple_id.find( id );   
		if (it2 != comp_simple_id.end()) //exist
			weight = comp_weight [it2->second];
		else{
			weight = 1;
		}
		cout<<weight<<", ";

		//cout<<"comp_weights["<< id  <<"] = "<<  weight <<";\n";
	}
	cout<<"};\n";


	cout << "//sizeof RAT = "<< sizeof(rat_weight_2cids) << " bits\n";
	cout << "//#define MAX_N_ENTRY  16777216 //2^24 bits: 8 AW, 8 CID1, 8 CID2\n";
	cout << "//i.e., MAX_N_ENTRY/8 = 16777216/8 = 2097152 bytes .\n";
	cout << "unsigned char Router::RAT["<< MAX_N_ENTRY/8  <<"]={\n";
	for (int i=0; i<MAX_N_ENTRY ;i+=8 ){
		unsigned char r = 0;
		for (int j=0; j<8; j++)
		{
			r |= rat_weight_2cids[j+i]<<j;   //the 8 bits are in the order from low to high
		}
		if (i %1000 ==0 ) printf("%d, \n", r);
		else 		printf("%d, ", r);
	}
	cout << "};\n";

	fclose(stdout);
}

void solution::print_act(int act_i)
{
	int comp_n_of_act= comp_ids_in_line[act_i][0];
	cout<< "act_i "<< act_i <<" , w " << edge_heads[act_i]->act_len  <<" , comp_n "<<comp_n_of_act<<" : ";
	for (int j=1; j<=comp_n_of_act; j++)
		cout<< comp_simple_id [ comp_ids_in_line[act_i][j] ]<<" ";
	cout << endl;

}


void solution::incre_weight(int node_j)
{

	std::vector <edge_t *> col_list;
	int col_list_n=0;
	for (int i=0; i<comp_n; i++)
	{
		//i -> j
		edge_t *tmp_p = relation_topology[i][ node_j ];
		while (tmp_p){
			tmp_p->visited_tag = 0;
			col_list.push_back(tmp_p);
			col_list_n ++;
			tmp_p=tmp_p->share_next;
		}

	}
	for (int i=0; i<comp_n; i++)
	{
		//j -> i when j is the head of act
		edge_t *tmp_p = relation_topology[ node_j ][ i ];
		while (tmp_p){
			if (tmp_p == tmp_p->edge_of_act_head) {
				tmp_p->visited_tag = 0;
				col_list.push_back(tmp_p);
				col_list_n ++;
			}
			tmp_p=tmp_p->share_next;
		}

	}
	

	comp_weight[node_j] ++;
	for (int i = 0; i<col_list_n ; i++)
	{
		edge_t *p = col_list[i];
	
		edge_t *edge_p = p->edge_of_act_head ; //from the beginning of the act of p
		//there are two noticeable problems:
		//1. a edge_t may be a p of node_j;
		//2. when order the share_p, it may be the share_ps of node_j
		int act_i = edge_p->act_id;
		int comp_n_of_act= comp_ids_in_line[act_i][0];
		int w=0;
		for (int j=1; j<=comp_n_of_act; j++)
			w +=  comp_weight [  comp_simple_id[comp_ids_in_line[act_i][j]] ];
		while (edge_p ){
			edge_p->act_len = w;
			edge_p->visited_tag = 1;

			//adjust edge_p in the oder of act_len
			edge_t *topology_start = relation_topology[ edge_p->start_id ][  edge_p->end_id ];
			bool start_id_tag = (topology_start == edge_p);
			edge_t *share_p = edge_p->share_next ;
			while (share_p && edge_p->act_len > share_p->act_len )
			{//share list is in incre oder 
				if (start_id_tag && edge_p->share_pre == NULL)
					relation_topology[ edge_p->start_id ][  edge_p->end_id ] = share_p; 
				edge_p->share_next = share_p->share_next;
				if (edge_p->share_next) 
					edge_p->share_next->share_pre = edge_p;
				share_p->share_next = edge_p;
				share_p->share_pre = edge_p->share_pre;
				if (share_p->share_pre) 
					share_p->share_pre->share_next = share_p;
				edge_p->share_pre = share_p;
				
				share_p = edge_p->share_next ;
			}

			edge_p = edge_p->edge_of_act_next ;
		}
	}

	/*tmp_p = relation_topology[i][ node_j ];
	if (tmp_p) cout << "incre after\n";
	while (tmp_p){
		print_act(tmp_p->act_id);
		tmp_p=tmp_p->share_next;
	}*/
		
}

int solution::find_unshared_comp(int act_id1, int act_id2)
{//search on act id 2, return unshared simple id.
	int	n1 = comp_ids_in_line[act_id1][0];
	int	n2 = comp_ids_in_line[act_id2][0];
	int id1, id2;
	
	int *act1 = new int[n1];
	int *act2 = new int[n2];
		
	//remove repeat cids
	int real_n1=0, real_n2=0;
	bool find_it = 0;
	for (int i=1; i<=n1 ; i++)
	{
		id1 = comp_ids_in_line [act_id1][i];
		find_it  = 0;
		for (map<int, int>::iterator it_rep_map = repeat_map.begin(); it_rep_map != repeat_map.end() && !find_it ; it_rep_map++ )
			for (int k=0; k<it_rep_map->second; k++)
				if (id1 == k + it_rep_map->first) 
				{
					find_it = 1;
					break;
				}
		if ( !find_it )
			act1[real_n1++] = id1;
	}
	
	for (int i=1; i<=n2 ; i++)
	{
		id2 = comp_ids_in_line [act_id2][i];
		find_it  = 0;
		for (map<int, int>::iterator it_rep_map = repeat_map.begin(); it_rep_map != repeat_map.end() && !find_it ; it_rep_map++ )
			for (int k=0; k<it_rep_map->second; k++)
				if (id2 == k + it_rep_map->first) 
				{
					find_it = 1;
					break;
				}
		if ( !find_it )
			act2[real_n2++] = id2;
	}
	std::vector<int> act_v1(act1, act1 + real_n1);
	std::vector<int> act_v2(act2, act2 + real_n2);
	delete[] act1;
	delete[] act2;
	
	int *unshare_sids = new int[real_n2];
	int unshare_n = 0;
	
	std::sort(act_v1.begin(), act_v1.end());
	std::sort(act_v2.begin(), act_v2.end());

	int i=0, j=0;
	while (i<real_n1 && j<real_n2)
	{
		if (act_v1[i] == act_v2[j]) {i++; j++;}
		else if (act_v1[i] > act_v2[j]) 
		{
			unshare_sids [unshare_n++] = comp_simple_id [act_v2[j]];
			j++;
		}
		else //if (act_v1[i] < act_v2[j]) 
		{
			while (i<real_n1 && act_v1[i] < act_v2[j])
				i++; 
			if (act_v1[i] > act_v2[j]){
				unshare_sids [unshare_n++] = comp_simple_id [act_v2[j]];
				j++;
			}
			else if (act_v1[i] == act_v2[j]) {
				i++; j++;
			}
		}

	}
	while (j<real_n2)
		unshare_sids [unshare_n++] = comp_simple_id [act_v2[j++]];

	if (unshare_n == 0){
		cout << "no available share cid for act " << act_id1 << " and "<< act_id2 << ".\n";
		delete[] unshare_sids ;
		return -1;
	}

	int min_degree_sid = unshare_sids [0];
	int min_degree = in_degree[ min_degree_sid ] + out_degree[ min_degree_sid ];
	for (int i=1; i<unshare_n; i++)
	{
		int tmp_sid = unshare_sids [i];
		int degree = in_degree[tmp_sid] + out_degree[tmp_sid];
		if (degree < min_degree ) {
			min_degree_sid = tmp_sid;
			min_degree = degree;
		}
		
	}
		
	delete[] unshare_sids ;
	return min_degree_sid;
	
}

void solution::seperate_conflicting()
{
	//cout << "\nseperate_relation||||||||||||||||||||||||||||||||||||||||||\n";
	for (int i=0; i<comp_n; i++){
		for (int j=0; j<comp_n-1; j++){
			edge_t *cur_p = relation_topology[i][ j ];
			if (!cur_p ) continue;

			int share_n=0;
			std::vector <edge_t *> share_list;
			while (cur_p ){
				share_n++;
				share_list.push_back(cur_p);
				cur_p = cur_p->share_next;
			}

			
			for (int share_i=0; share_i<share_n; share_i++)
			//while (cur_p )
			{ 
				cur_p = share_list[share_i];
				//don't use while or for, because the share link list may be changed after incre_weight(k).
				//e.g., cur_p act_id 1, w 8 :  1 0 1 15 1 28 1 5 
				//                             - -
				//      p act_id 0, w 8, k 6 : incre_weight(6)
				//      w 8, 1-6, actid 0   :  0 1 6 .....   ok
				//                               - -
				//      w 16, 43-6, actid 22 : 1 0 ..... 43 6 .....  not ok
				//                             - -       -- -
				//      because 1-0 of actid 22 is also in share list of 1-0, incresing 43-6 will change 1-0 list.
				int cur_len = cur_p->act_len;
				for (int k=j+1; k<comp_n; k++){ //do not start from 0 to prevent from deadlock or unlimit recurrence
					edge_t *p = relation_topology[i][ k ];
					while (p && p->act_len < cur_len)
						p = p->share_next;
					if (p && p->act_id != cur_p->act_id && p->act_len == cur_len)
					{
						// when p->act_id == cur_p->act_id, i.e., both are in the same act, 
						// this algorithm cannot seperate relation.
						// E.g., in act: 1 0 1 15 1 28 1 5,  
						// 1 0 and 1 5 are confused relation which cannot be seperated by this function.
						
						cout << "\n=*=*=*=*=*=*==*=*=*=*=*=*=*=*=*=*=*=*==*=*=*=*=*=*=\n";
						cout << "before incre i="<<i<<", j="<<j
							<<", p->act_len="<<p->act_len
							<<", k="<<k 
							<<", cur_p->act_id="<< cur_p->act_id 
							<<", p->act_id="<< p->act_id 
							<<", cur_len="<<cur_len
							<<", p->act_len="<<p->act_len
							<<", p->act_head(is sid)="<<p->act_head << "\n";
						
						int unshare_sid = find_unshared_comp (cur_p->act_id, p->act_id);
							
						if (unshare_sid != -1) {
							incre_weight( unshare_sid );
							cout << "after  incre i="<<i<<", j="<<j
							<<", p->act_len="<<p->act_len
							<<", k="<<k << ", unshare sid=" << unshare_sid 
							<<", cur_p->act_id="<< cur_p->act_id 
							<<", p->act_id="<< p->act_id 
							<<", cur_len="<<cur_len
							<<", p->act_len="<<p->act_len
							<<", comp_weight[unshare_sid ]="<< comp_weight[unshare_sid ]
							<<", p->act_head(is sid)="<<p->act_head << "\n";
						}
						else{
							cout << "after  incre i="<<i<<", j="<<j
							<<", p->act_len="<<p->act_len
							<<", k="<<k << ", faile unshare sid=" << unshare_sid 
							<<", cur_p->act_id="<< cur_p->act_id 
							<<", p->act_id="<< p->act_id 
							<<", cur_len="<<cur_len
							<<", p->act_len="<<p->act_len
							<<", p->act_head(is sid)="<<p->act_head << "\n";
						}
							
							
						//print_acts();
						//print_topology(22);
						k--; //have to start checking from start
					}
					
				}
			
				//do{
					//cur_p = cur_p->share_next;
				//} while (cur_p && cur_p->act_len == cur_len );
			}

		}

	}
}

void solution::get_conflict_acts(int &actid1, int &actid2)
{
	actid1=-1; actid2=-1;
	for (int i=0; i<comp_n; i++){
		for (int j=0; j<comp_n; j++){
			edge_t *p = relation_topology[i][ j ];
			while (p){
				int cur_len = p->act_len;
				int cur_actid = p->act_id;
				int k;
				for (k=0; k<comp_n; k++)
				{
					if (k==j)continue; //do not search itself.
					edge_t *q = relation_topology[i][ k ];
					while (q && q->act_len < cur_len)
						q=q->share_next;
					if (q && q->act_len == cur_len){
						actid1=cur_actid; 
						actid2=q->act_id;
						return;
					}
				}// i,k q
				p=p->share_next;
			}// i,j p
		}// j
	}// i

	return;
	
}

void solution::seperate_conflicting_using_while()
{
	int a,b;
	get_conflict_acts (a, b);
	if (a==-1 && b==-1) return;

	do {
		/*cout << "\n=*=*=*=*=*=*==*=*=*=*=*=*=*=*=*=*=*=*==*=*=*=*=*=*=\n";
		cout << "before act1 ="<<a<<", act2="<<b
			<<", act1 len="<<edge_heads[a]->act_len
			<<", act2 len="<<edge_heads[b]->act_len
			<< "\n";
			*/
						

		int unshare_sid = find_unshared_comp (a, b);
		if (unshare_sid != -1) {
			incre_weight( unshare_sid );
			/*cout << "after act1 ="<<a<<", act2="<<b
			<<", act1 len="<<edge_heads[a]->act_len
			<<", act2 len="<<edge_heads[b]->act_len
			<< "\n";*/
		}
		else{
			cout << "cannot find unshare sid between acts:" << a <<" and " << b << "\n";
			break;
		}
		get_conflict_acts (a, b);
	}while (a!=-1 && b!=-1);
}

void solution::print_bit(int n_bit, int n)
{
	if (n_bit > 32 || n_bit < 0) return ;
	//cout << n << " : ";
	printf("%10d: ", n);
	for (int i = n_bit-1 ; i >= 0; i--)
	{
		if (n & (1 << i)) cout << "1";
		else			  cout << "0";
		if (i % 2 == 0)   cout << " ";

	}
	cout << "\n";
}


int solution::get_even_bit(int n_bit, int n)
{
	if (n_bit > 32 || n_bit < 0) return n;
	int res = 0;
	n_bit >>= 1;
	for (int k=0; k < n_bit; k++)
	{
		int distance = (k<<1) + 1; // 2 * k + 1
		int digit_v = n & (1 << distance );
		//cout << "k=" <<k << ", distance="<< distance  <<"\n";
		if (digit_v)
			res = res | (1 << k);
	}

	return res;

}

int solution::get_odd_bit(int n_bit, int n)
{
	if (n_bit > 32 || n_bit < 0) return n;
	int res = 0;
	n_bit >>= 1;
	for (int k=0; k < n_bit; k++)
	{
		int distance = k<<1 ; // 2 * k
		int digit_v = n & (1 << distance );
		//cout << "k=" <<k << ", distance="<< distance  <<"\n";
		if (digit_v)
			res = res | (1 << k);
	}
	return res;

}

void solution::get_rat()
{
/* comp id is 8 bits
 * two ids have 16 bits
 * len is 8 bits
 * len + two ids have 24 bits
 * in total use 2^24 bits, i.e., 2^21 bytes.
 * 2^21 bytes = 2 MB
 * |  8  |  8  |  8  |
 *   len   id1   id2
 *
 * */

	rat_count = 0;
	final_bits_or = 0;
	final_bits_and = 0x00ffffff;

	for (int i=0; i<act_n; i++){
		int len = comp_ids_in_line[i][0];
		edge_t *p = edge_heads[ i ];
		int embed_len = p->act_len; 

		for (int j=2; j<=len; j++){
			int id1 = comp_ids_in_line[i][ j-1 ];
			int id2 = comp_ids_in_line[i][ j ];
			int rat_id = 0;

			rat_id = (embed_len << 16) | (id1 << 8) | (id2) ;
		    //print_bit (8, embed_len );
		    //print_bit (8, id1);
		    //print_bit (8, id2);
		    //print_bit (24, rat_id);
			rat_weight_2cids [rat_id] = true;
			rat_count_array [rat_count ++] = rat_id;
			final_bits_or  |= rat_id;
			final_bits_and  &= rat_id;
			
		}
	}
}

void solution::run()
{
	get_act_line();
	//get_relation_topoloy();
	get_relation_topoloy2();
	get_degrees ();
	//print_info(0);

	//seperate_conflicting();
	seperate_conflicting_using_while();
	get_rat ();
	//print_info(1);
	print_initial_file();

}

int main(int argc, char * argv[])
{

	map<int, string> comp_cids; //<cid, name>
	//map<int, string>::iterator it1; //<cid, name>
#include "elements.h"

	map<int, int>rep_map = Router::init_repeat_map();

	solution s(comp_cids, rep_map);
	s.run();
	
	return 1;
}

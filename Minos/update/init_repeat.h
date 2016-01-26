HashTable<int, int> Router::init_repeat_map(){
	HashTable<int, int> tmp_m;//<cid, repeat times> 
//including the first time; the cid is added by repeat times
//Using this file needs to set check_repeat in Attach_CTAG().
	tmp_m[6]= 2;
	tmp_m[9]= 6;
	return tmp_m;
}
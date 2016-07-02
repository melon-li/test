#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <deque>
#include <time.h>
#include <string>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <stdint.h>
#include <set>

using namespace std;
 
#define PKT_NUM 8
#define FILE_NUM 4
typedef map<uint32_t, char *> Pkts;
class Slot_Object{
private:
    typedef map<string, uint8_t> Name2index;
    typedef vector<Pkts> Files;
    
    //map filename  to index in files variable,because file is vector
    Name2index name2index;
    Files files;
    uint32_t pkt_num = PKT_NUM;
    uint32_t file_num = FILE_NUM;
    uint32_t cur_index = 0;

public:
    //uint64_t write_for_evictions = 0;
    Slot_Object(){}

    
    /*
     *@func: insert a packet of filename
     *@param: make_pair(filename-ID, data_pointer)
     *@return: False,failed to insert. 
     *         True, succeeded to insert it or it has already been stored in DRAM
     *         if slot is full, instead the most front filename.
     *         int, stored_packet number at this time
     */
     pair<bool, int> insert_packet(const string& key, uint32_t _ID, char *payload);

    /*
     *@func: insert PKT_NUM or total(if it is less than PKT_NUM) packets of filename
     *@param: filename with begin-ID
     *@param: last_id, last_id subtracted  begin_id is less than PKT_NUM
     *@return: False,failed to insert; 
     *         True, succeeded to insert it or it has already been stored in DRAM
     *         if slot is full, instead the most front filename.
     *         int, stored_packet number at this time
     */
    pair<bool, int> insert_packets(string key, uint32_t last_id, Pkts payloads);

    /*
     *@param: filename without file ID(begin-id)
     *@return: bool,true or false
     *         Pkts
     */
    pair<bool, Pkts> find(const string & key);

    //File_Obj files[FILE_NUM];
     
};

pair<bool, int> Slot_Object::insert_packet(const string& key, uint32_t _ID, char *payload){
    Name2index::iterator it;
    uint32_t pos = 0;
    string _filename;
    int cnt = 0;

    it = name2index.find(key);

    //The file named key is not cached!
    if(it == name2index.end()){
        //this slot is full
        if(files.size() >= file_num){
            /* Delete the oldest file, or in options, we can use LRU algorithm to delete file
               Note: it's worthy to discuss if we should delete the corresponding elements in bloom filter, 
            */
            cnt -= files[cur_index].size();
            files[cur_index].clear();
            files[cur_index].insert(Pkts::value_type(_ID, payload)); 
            cnt++;
            //Find old corresponding element in name2index, and erase it
            for(Name2index::iterator it_t=name2index.begin(); it_t!=name2index.end(); it_t++){
                if(it_t->second == cur_index) {
                    name2index.erase(it_t);
                    break;
                }
            }
            //insert new element into name2index
            name2index.insert(Name2index::value_type(key, cur_index));

            // increment cur_index to can delete  the oldest file
            cur_index++;
            if(cur_index > file_num) cur_index = 0;
        }else{
            Pkts p;

            // get the pos after insert one element into files
            pos = files.size();
            p.insert(Pkts::value_type(_ID, payload));
            files.push_back(p);
            cnt++;
            name2index.insert(Name2index::value_type(key, pos));
        }
    }else{
        pos = it->second;
        Pkts::iterator vt = files[pos].begin();

        files[pos].insert(Pkts::value_type(_ID, payload));
        cnt++;
    }
    return std::make_pair(true, cnt);
}

pair<bool, int> Slot_Object::insert_packets(string key, uint32_t last_id, Pkts payloads){
    uint32_t pos = 0, _ID = 0, len=0;
    string _filename, filename;
    int cnt = 0;

    pos = key.find("-");
    if(!pos) return std::make_pair(false, 0);
    _ID = atoi(key.substr(pos+1).c_str());
    _filename = key.substr(0, pos);
    _filename = _filename + "-";

    len = last_id - _ID + 1;
    //if( len > PKT_NUM ) return std::make_pair(false, 0);
    
    //insert packets to dram
    for(Pkts::iterator it =payloads.begin(); it != payloads.end(); it++){
        //filename = _filename;
        //filename.append(std::to_string(it->first));
        pair<bool, int> pr = insert_packet(key, it->first, it->second);
        //if(!pr.first)return std::make_pair(false, 0);
        cnt += pr.second;
    }
 
    return std::make_pair(true, cnt);
}

pair<bool, Pkts> Slot_Object::find(const string& key){
    Pkts p;
    Name2index::iterator it = name2index.find(key);
    if(it == name2index.end()) return std::make_pair(false, p);
    return std::make_pair(true, files[it->second]);
}

 map <uint32_t, Slot_Object> data_table;
int main(){
    Pkts payloads;
    payloads.insert(Pkts::value_type(0, NULL));
    payloads.insert(Pkts::value_type(1, NULL));
    payloads.insert(Pkts::value_type(2, NULL));
    payloads.insert(Pkts::value_type(3, NULL));
    Slot_Object so;
    string key0 = "/domain0/sex-0";
    string key1 = "/domain0/sex-8";
    so.insert_packets(key0, 3, payloads);
    payloads.erase(3);
    so.insert_packets(key1, 3, payloads);
    data_table.insert(map<uint32_t, Slot_Object>::value_type(1, so));
    map<uint32_t, Slot_Object>::iterator it = data_table.find(1);
    pair<bool, Pkts> pr = it->second.find(key1);
    if(!pr.first) return 1;
    pr.second.erase(pr.second.begin());
    
    std::cout<<pr.second.size()<<std::endl;
    
    //map<uint32_t, Slot_Object>::iterator it2 = data_table.find(1);
   // pair<bool, Pkts> pr2 = it2->second.find(key0);
    //pair<bool, Pkts> pr2 = it2->second.find(key0);
    //if(!pr2->first) return 1;
    it = data_table.find(1);
    pr = it->second.find(key1);
    if(!pr.first) return 1;
    std::cout<<pr.second.size()<<std::endl;
    
}

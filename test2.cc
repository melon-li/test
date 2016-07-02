#include <iostream>
#include <set>
#include <sstream>
using namespace std;
int main(){
    set<int> asked;
    stringstream ss;
    ss<<-1
      <<"hh"<<endl;
    cout<<ss.str()<<endl;
    asked = set<int>();
    asked.insert(1);
    asked.insert(2);
    std::cout<<asked.size()<<std::endl;
}

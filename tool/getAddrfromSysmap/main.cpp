#include <iostream>
#include<fstream>
#include<string>
using namespace std;

int main()
{

    ifstream f2("syscallnum.txt");
    ofstream f3("syscallres.txt");
    string f1name,f1b,f2name,f1ad;
    int f2num;
    while(f2>>f2name){
        f2>>f2num;
       int a = 0;
       ifstream f1("syscalladd.txt");
        while(f1>>f1ad>>f1b>>f1name){
            if(f1name == f2name){
                f3<<"{.num = "<<f2num<<", .addr = 0x"<<f1ad.c_str()<<", .name = \""<<f2name.c_str()<<"\", .pre = 0x00},\n";
                a = 1;
                break;
            }
        }
        if(a == 0){
            f3<<"{.num = "<<f2num<<", .addr = 0x0000000000000000, .name = \""<<f2name.c_str()<<"\", .pre = 0x00},\n";
        }
    }
    
    return 0;
}

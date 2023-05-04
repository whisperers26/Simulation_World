#include"simulation.h"
#include<string>
#include<sstream>
#include<fstream>
using namespace std;

int main(int argc, char* argv[]){
    //check arguments
    if(argc<4){
        cerr<<"Error: Missing arguments!"<<endl;
        cerr<<"Usage: ./p3 <species-summary> <world-file> <rounds> [v|verbose]"<<endl;
        return 0;
    }
    if(atoi(argv[3])<0){
        cerr<<"Error: Number of simulation rounds is negative!"<<endl;
        return 0;
    }
    try{
        bool ifverbose;
        if(argc>=5){
            string ver=argv[4];
            if(ver=="v"||ver=="verbose")
                ifverbose=true;
        }
        else
            ifverbose=false;
        string a=r_sumFile(argv[1]);
        species_t *s=r_speProgram(a);
        world_t world;
        r_worldtype(argv[2], s, world);
        int round=atoi(argv[3]);
        allturn(world, ifverbose, round);
    }
    catch(string s){
        cout<<s<<endl;
        return 0;
    }
    return 0;
}

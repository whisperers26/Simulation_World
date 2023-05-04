#include<iostream>
#include<fstream>
#include<sstream>
#include<iomanip>
#include<string>
#include<cstdlib>
#include<cassert>
#include"world_type.h"
#include"simulation.h"
using namespace std;

//get size of an array
template<class T>
int length(T& arr){
    return sizeof(arr)/sizeof(arr[0]);
}

//test function
void printWorld(const world_t &world){
    int h=world.grid.height;
    int w=world.grid.width;
    int i,j;
    for(i=0;i<h;i++){
        for(j=0;j<w;j++){
            if(world.grid.squares[i][j]==NULL){
                cout<<"____ ";
            }
            else{
                cout<<world.grid.squares[i][j]->species->name.substr(0,2)<<"_";
                cout<<directShortName[world.grid.squares[i][j]->direction]<<" ";
            }
        }
        cout<<endl;
    }
}

//effect:read from species summary file and return dir and species of species program
//require:valid addr
string r_sumFile(const string &addr){
    ifstream iFile;
    string str, tmp;
    str="";
    iFile.open(addr);
    if(!iFile){
        throw ("Error: Cannot open file "+addr+"!");
    }
    while(getline(iFile, tmp)){
        str+=(" "+tmp);
    }
    iFile.close();
    return str;
}

//effect:read from species program and store instructions to species, return a pointer to an array
//require:valid inf:dir+species
species_t*  r_speProgram(const string &inf){
    istringstream iString,testi;
    iString.str(inf);
    string s_name[MAXSPECIES]={""};
    string dir,test;
    iString>>dir;
    int n=0;//number of species
    testi.str(inf);
    int testint=-1;
    while(testi>>test){
        testint++;
    }
    if(testint>MAXSPECIES){
            ostringstream oStr;
            oStr<<"Error: Too many species!"<<"\n"<<"Maximal number of species is "<<MAXSPECIES<<".";
            string a=oStr.str();
            throw (a);
    }
    while(iString>>s_name[n]){
        n++; 
    }
    static species_t species[MAXSPECIES];
    int i=0;
    for(i=0;i<n;i++){//for every sepies
        (species+i)->name=s_name[i];
        instruction_t program[MAXPROGRAM];
        ifstream iFile;
        string file="";
        ostringstream oString;
        oString<<dir<<"/"<<s_name[i];
        file=oString.str();
        iFile.open(file);
        if(!iFile){
            throw ("Error: Cannot open file "+file+"!");
        }
        string line="";
        int n_inst=0;//size of program
        // for(m=0;m<n_inst;m++){
        //     program[m].op=OP_SIZE;
        // }
        //get instructions from the species program
        
        
        while(getline(iFile, line)){
            if(n_inst>MAXPROGRAM){
                ostringstream oStr;
                oStr<<"Error: Too many instructions for species "<<(species+i)->name<<"!\n"
                    <<"Maximal number of instructions is "<<MAXPROGRAM<<".";
                throw (oStr.str());
            }
            if(line.length()==0)break;
            string op_name;
            unsigned int b=0;
            istringstream iString2;
            iString2.str(line);
            iString2>>op_name>>b;
            int k;
            int n=length(opName);
            bool ifexist=false;
            for(k=0;k<n;k++){
                if(op_name.compare(opName[k])==0){
                    program[n_inst].op=opcode_t(k);
                    program[n_inst].address=b-1;
                    ifexist=true;
                    break;
                }
            }
            if(!ifexist){
                string a="Error: Instruction "+op_name+" is not recognized!";
                throw a;
            }
            n_inst++;
        }
        //initialize species
        (species+i)->programSize=n_inst;
        unsigned int j;
        for(j=0;j<n_inst;j++){
            (species+i)->program[j]=program[j];
        }
        iFile.close();
    }
    if(n==MAXSPECIES);
    else (species+n)->name="";
    return species;
}

//effect:read from world file and initialize the world
//require:valid addr
bool r_worldtype(const string &addr,const species_t *s, world_t &world){
    //First initialize species
    unsigned int n=0;
    while(s[n].name!=""){
        n++;
    }
    world.numSpecies=n;
    unsigned int i;
    for(i=0;i<MAXSPECIES;i++){
        world.species[i]=*(s+i);
    }

    //Second initialize creatures (read from world file)
    ifstream iFile;
    iFile.open(addr);
    if(!iFile){
        throw ("Error: Cannot open file "+addr+"!");
    }
    string line;
    istringstream iStr, iStr2, iStr3;
    int h,w;//height and width
    getline(iFile, line);
    iStr.str(line);
    iStr>>h;
    getline(iFile, line);
    iStr2.str(line);
    iStr2>>w;
    //check h,w
    if(h<1||h>MAXHEIGHT){
        string a="Error: The grid height is illegal!";
        throw a;
    }
    if(w<1||w>MAXWIDTH){
        string a="Error: The grid width is illegal!";
        throw a;
    }
    int j,k;
    terrain_t ter=TERRAIN_SIZE;
    char tmp;
    string tmp1;
    for(i=0;i<h;i++){//initialize terrain[][]
        getline(iFile, line);
        iStr3.str(line);
        for(j=0;j<w;j++){
            iStr3>>tmp;
            tmp1=tmp;
            bool ifvalid=false;//check the terrain
            //convert the short name of terrain to terrain
            for(k=0;k<length(terrainShortName);k++){
                if(tmp1.compare(terrainShortName[k])==0){
                    ter=terrain_t(k);
                    ifvalid=true;
                    break;
                }
            }
            if(!ifvalid){
                ostringstream oStr;
                oStr<<"Error: Terrain square ("<<tmp1<<" "<<i<<" "<<j<<") is invalid!";
                throw (oStr.str());
            }
            world.grid.terrain[i][j]=ter;
        }
    }
    //build creatures
    int numCreature=0;
    while(getline(iFile, line)){
        istringstream iStr4;
        iStr4.str(line);
        string spe;
        iStr4>>spe;
        //species
        bool ifspecies=false;
        for(i=0;i<world.numSpecies;i++){
            if(spe.compare(world.species[i].name)==0){
                world.creatures[numCreature].species=&world.species[i];
                ifspecies=true;
                break;
            }
        }
        if(!ifspecies){
            ostringstream oStr;
            oStr<<"Error: Species "<<spe<<" not found!";
            throw oStr.str();
        }
        //direction
        string dir;
        direction_t dt;
        iStr4>>dir;
        bool ifdirect=false;
        for(i=0;i<length(directName);i++){
            if(dir.compare(directName[i])==0){
                dt=direction_t(i);
                ifdirect=true;
                break;
            }
        }
        if(!ifdirect){
            ostringstream oStr;
            oStr<<"Error: Direction "<<dir<<" is not recognized!";
            throw oStr.str();
        }
        world.creatures[numCreature].direction=dt;
        //cout<<dt<<endl;
        //location
        int lh=-1,lw=-1;
        iStr4>>lh>>lw;
        if(lh<0||lh>=h){
            ostringstream errorstr;
            errorstr<<"Error: Creature ("<<spe<<" "<<dir<<" "<<lh<<" "<<lw<<") is out of bound!"
                    <<"\nThe grid size is "<<h<<"-by-"<<w<<".";
            throw errorstr.str();
        }
        if(lw<0||lw>=w){
            ostringstream errorstr;
            errorstr<<"Error: Creature ("<<spe<<" "<<dir<<" "<<lh<<" "<<lw<<") is out of bound!"
                    <<"\nThe grid size is "<<h<<"-by-"<<w<<".";
            throw errorstr.str();
        }
        world.creatures[numCreature].location.r=lh;
        world.creatures[numCreature].location.c=lw;
        //program-id
        world.creatures[numCreature].programID=0;
        //ability
        string a="";
        for(i=0;i<ABILITY_SIZE;i++){
            world.creatures[numCreature].ability[i]=false;
        }
        while(iStr4>>a){
            bool isability=false;
            for(i=0;i<ABILITY_SIZE;i++){
                if(a.compare(abilityShortName[i])==0){
                    world.creatures[numCreature].ability[i]=true;
                    isability=true;
                    break;
                }
            }
            if(!isability){
                ostringstream oStr;
                oStr<<"Error: Creature ("<<spe<<" "<<dir<<" "<<lh<<" "<<lw<<")has an invalid ability "<<a<<"!";
                throw oStr.str();
            }
        }
        //hillActive
        world.creatures[numCreature].hillActive=false;
        numCreature++;
        //check num of creatures
        if(numCreature>MAXCREATURES){
            ostringstream oStr;
            oStr <<"Error: Too many creatures!\nMaximal number of creatures is "<<MAXCREATURES<<".";
            throw oStr.str();
        }
    }
    world.numCreatures=numCreature;

    //initialize grid
    //squares
    world.grid.height=h;
    world.grid.width=w;
    for(i=0;i<h;i++){
        for(j=0;j<w;j++){
            world.grid.squares[i][j]=NULL;
        }
    }
    int r,c;
    for(i=0;i<world.numCreatures;i++){
        r=world.creatures[i].location.r;
        c=world.creatures[i].location.c;
        if(world.grid.squares[r][c]!=NULL){
            ostringstream oStr;
            oStr<<"Error: Creature ("<<world.creatures[i].species->name
                <<" "<<directName[world.creatures[i].direction]<<" "<<r<<" "<<c<<") overlaps with creature ("
                <<world.grid.squares[r][c]->species->name<<" "<<directName[world.grid.squares[r][c]->direction]<<
                " "<<r<<" "<<c<<")!";
            throw oStr.str();
        }
        world.grid.squares[r][c]=&world.creatures[i];
        if(world.grid.terrain[r][c]==LAKE&&world.creatures[i].ability[FLY]==false){
            ostringstream oStr;
            oStr<<"Error: Creature ("<<world.creatures[i].species->name
                <<" "<<directName[world.creatures[i].direction]<<" "<<r<<" "<<c<<") is in a lake square!"
                <<"\nThe creature cannot fly!";
            throw oStr.str();
        }
    }
    //terrain has been initialized    
    return 1;
}


//part 3: simulation of the world
//effect:show the point the creature is facing
point_t showface(const creature_t &creature){
    point_t newP;
    newP=creature.location;
    switch(creature.direction){
    case EAST:
        newP.c++;
        break;
    case SOUTH:
        newP.r++;
        break;
    case WEST:
        newP.c--;
        break;
    case NORTH:
        newP.r--;
        break;
    }
    return newP;
}

//effect: show if the point is inside the grid
bool ifinside(const point_t &point, const grid_t &grid){
    if(point.r>=grid.height||point.r<0||point.c>=grid.height||point.c<0){
        return false;
    }
    else
        return true;
}

//effect: The creature moves forward. If moving forward would put
//the creature outside the boundaries of the grid, would cause the
//creature that cannot fly to land in a “lake” square, or would cause
//the creature to land on top of another creature, the hop instruction does nothing.
//If success, hop will return true, or return false
bool hop(creature_t &creature, grid_t &grid){
    creature.programID++;
    point_t newP=showface(creature);
    //if out bound, return false
    if(!ifinside(newP, grid)){
        return false;
    }
    //if occupied, return false
    if(grid.squares[newP.r][newP.c]!=NULL){
        return false;
    }
    //if creature cannot fly land in lake, return false
    if(creature.ability[FLY]==false&&grid.terrain[newP.r][newP.c]==LAKE){
        return false;
    }
    //change grid pointer
    grid.squares[creature.location.r][creature.location.c]=NULL;
    grid.squares[newP.r][newP.c]=&creature;
    //change creature position and hillActive, programID
    creature.location=newP;
    creature.hillActive=false;
    return true;
}

//effect: turn left
void turnleft(creature_t &creature){
    creature.programID++;
    switch(creature.direction){
    case EAST:
        creature.direction=NORTH;
        break;
    case SOUTH:
        creature.direction=EAST;
        break;
    case WEST:
        creature.direction=SOUTH;
        break;
    case NORTH:
        creature.direction=WEST;
        break;
    }
}

//effect: turn right
void turnright(creature_t &creature){
    creature.programID++;
    switch(creature.direction){
    case EAST:
        creature.direction=SOUTH;
        break;
    case SOUTH:
        creature.direction=WEST;
        break;
    case WEST:
        creature.direction=NORTH;
        break;
    case NORTH:
        creature.direction=EAST;
        break;
    }
}

//effect: infect the creature in front of it
bool infect(creature_t &creature, grid_t &grid){
    creature.programID++;
    point_t newP=showface(creature);
    //if front out bound, return false
    if(!ifinside(newP, grid)){
        return false;
    }
    //for creature dont have A ability
    if(creature.ability[ARCH]==false){
        //empty
        if(grid.squares[newP.r][newP.c]==NULL){
            return false;
        }
        //forest square
        if(grid.terrain[newP.r][newP.c]==FOREST){
            return false;
        }
        //same species
        if(grid.squares[newP.r][newP.c]->species==creature.species){
            return false;
        }
        //infect
        grid.squares[newP.r][newP.c]->species=creature.species;
        grid.squares[newP.r][newP.c]->programID=0;
        return true;
    }
    //for creature has A ability
    else{
        bool ifenemy=false;
        int dh=newP.r-creature.location.r;
        int dw=newP.c-creature.location.c;
        point_t finalP;
        finalP.r=creature.location.r;
        finalP.c=creature.location.c;
        while(ifinside(finalP, grid)){
            finalP.r+=dh;
            finalP.c+=dw;
            creature_t *tmpc=grid.squares[finalP.r][finalP.c];
            if(tmpc!=NULL&&tmpc->species!=creature.species){
                tmpc->species=creature.species;
                tmpc->programID=0;
                ifenemy=true;
                return ifenemy;
            }
        }
        return ifenemy;
    }
}

//execute go operation if true
bool gotoid(creature_t &creature, bool ifgo, int id){
    if(ifgo){
        creature.programID=id;
        return true;
    }
    else{
        creature.programID++;
        return false;
    }
}

//effect: If the square in front of the creature
//is inside the grid boundary and unoccupied, jump to
//step n of the program; otherwise, go on with the next instruction in sequence.
bool emptygo(creature_t &creature,const grid_t &grid, int id){
    point_t newP=showface(creature);
    bool ifempty=false;
    if(ifinside(newP, grid)&&grid.squares[newP.r][newP.c]==NULL){
        ifempty=true;
    }
    return gotoid(creature, ifempty, id);
}

//If the square the creature is facing is not a “forest” square
//and is occupied by a creature of an enemy species, jump to
//step n; otherwise, go on with the next instruction.
bool enemygo(creature_t &creature, const grid_t &grid, int id){
    point_t newP=showface(creature);
    bool ifenemy=false;
    if(ifinside(newP, grid)&&grid.squares[newP.r][newP.c]!=NULL){
        if(grid.squares[newP.r][newP.c]->species!=creature.species&&
            grid.terrain[newP.r][newP.c]!=FOREST){
            ifenemy=true;
        }
    }
    return gotoid(creature, ifenemy, id);
}

//If the square the creature is facing is not a “forest” square
//and is occupied by a creature of the same species, jump to 
//step n; otherwise, go on with the next instruction.
bool samego(creature_t &creature, const grid_t &grid, int id){
    point_t newP=showface(creature);
    bool ifsame=false;
    if(ifinside(newP, grid)&&grid.squares[newP.r][newP.c]!=NULL){
        if(grid.squares[newP.r][newP.c]->species==creature.species&&
            grid.terrain[newP.r][newP.c]!=FOREST){
            ifsame=true;
        }
    }
    return gotoid(creature, ifsame, id);
}

//If the creature is facing the border of the grid (which we
//imagine as consisting of a huge wall), or the creature is
//facing a lake square and it cannot fly, jump to step n of
//the program; otherwise, go on with the next instruction
bool wallgo(creature_t &creature, const grid_t &grid, int id){
    point_t newP=showface(creature);
    bool ifwall=false;
    if(ifinside(newP, grid)){
        if(creature.ability[FLY]==false&&
            grid.terrain[newP.r][newP.c]==LAKE){
            ifwall=true;    
        }
    }
    if(!ifinside(newP, grid)){
        ifwall=true;
    }
     return gotoid(creature, ifwall, id);
}

//This instruction always jumps to step n, independent of any condition.
void go(creature_t &creature, int id){
     gotoid(creature, true, id);
}

//simulate one turn of a creature and print all actions
bool creturn(creature_t &creature, grid_t &grid, bool ifverbose){
    bool ifstop=false;
    int lh=creature.location.r;
    int lw=creature.location.c;
    //if in hill and hillactive is 0, do nothing
    if(grid.terrain[lh][lw]==HILL&&!creature.ability[FLY]&&!creature.hillActive){
        creature.hillActive=true;
        return false;
    }
    //simulate one turn
    ostringstream oStr;
    oStr<<"Creature ("<<creature.species->name<<" "<<directName[creature.direction]
        <<" "<<lh<<" "<<lw<<") takes action:";
    cout<<oStr.str();
    if(ifverbose){
        cout<<endl;
    }
    oStr.clear();
    oStr.str("");
    string s;
    while(!ifstop){
        if(ifverbose){
            if(creature.species->program[creature.programID].op<=3){
                oStr<<"Instruction "<<(creature.programID+1)<<": "
                    <<opName[creature.species->program[creature.programID].op];
            }
            else{
                oStr<<"Instruction "<<(creature.programID+1)<<": "
                    <<opName[creature.species->program[creature.programID].op]
                    <<" "<<(creature.species->program[creature.programID].address+1);
            }
            s=oStr.str();
            cout<<s<<endl; 
        }
        else{
            if(creature.species->program[creature.programID].op<=3){
                oStr<<" "<<opName[creature.species->program[creature.programID].op];
                s=oStr.str();
                cout<<s<<endl; 
            }
        }
        oStr.clear();
        oStr.str("");
        switch(creature.species->program[creature.programID].op){
        case HOP:
            hop(creature, grid);
            ifstop=true;
            break;
        case LEFT:
            turnleft(creature);
            ifstop=true;
            break;
        case RIGHT:
            turnright(creature);
            ifstop=true;
            break;
        case INFECT:
            infect(creature, grid);
            ifstop=true;
            break;
        case IFEMPTY:
            emptygo(creature, grid, creature.species->
                    program[creature.programID].address);
            break;
        case IFENEMY:
            enemygo(creature, grid, creature.species->
                    program[creature.programID].address);
            break;
        case IFSAME:
            samego(creature, grid, creature.species->
                    program[creature.programID].address);
            break;
        case IFWALL:
            wallgo(creature, grid, creature.species->
                    program[creature.programID].address);
            break;
        case GO:
            go(creature,  creature.species->
                    program[creature.programID].address);
            break;
        }
    }
    creature.hillActive=false;
    return true;
}

//simulate all turns of creatures and print result
void allturn(world_t &world, bool ifverbose, int round){
    cout<<"Initial state"<<endl;
    printWorld(world);
    int i,j;
    bool ifmove;
    for(i=0;i<round;i++){
        cout<<"Round "<<(i+1)<<endl;
        for(j=0;j<world.numCreatures;j++){
            ifmove=creturn(world.creatures[j], world.grid, ifverbose);
            if(ifverbose&&ifmove){
                printWorld(world);
            }
        }
        if(!ifverbose){
            printWorld(world);
        }
    }
}

#include<iostream>
#include<fstream>
#include<sstream>
#include<iomanip>
#include<string>
#include<cstdlib>
#include<cassert>
#include"world_type.h"


#ifndef SIMULATION_H
#define SIMULATION_H
//test function
void printSpecies(const species_t *s);

void printWorld(const world_t &world);

/*
First part of functions: read data from files and initialize
the world
*/

//effect:read from species summary file and return dir and species of species program
//require:valid addr
string r_sumFile(const string &addr);

//effect:read from species program and store instructions to species, return a pointer to an array
//require:valid inf:dir+species
species_t* r_speProgram(const string &inf);

//effect:read from world file and initialize the world
//require:valid addr
bool r_worldtype(const string &addr,const species_t *s, world_t &world);

/*
Second part: simulate the development of the world
*/

//effect:show the point the creature is facing
point_t showface(const creature_t &creature);

//effect: show if the point is inside the grid
bool ifinside(const point_t &point, const grid_t &grid);


//effect: The creature moves forward. If moving forward would put
//the creature outside the boundaries of the grid, would cause the
//creature that cannot fly to land in a “lake” square, or would cause
//the creature to land on top of another creature, the hop instruction does nothing.
//If success, hop will return true, or return false
bool hop(creature_t &creature, grid_t &grid);

//effect: turn left
void turnleft(creature_t &creature);

//effect: turn right
void turnright(creature_t &creature);

//effect: infect the creature in front of it
bool infect(creature_t &creature, grid_t &grid);

//execute go operation if true
bool gotoid(creature_t &creature, bool ifgo, int id);

//effect: If the square in front of the creature
//is inside the grid boundary and unoccupied, jump to
//step n of the program; otherwise, go on with the next instruction in sequence.
bool emptygo(creature_t &creature, const grid_t &grid, int id);

//If the square the creature is facing is not a “forest” square
//and is occupied by a creature of an enemy species, jump to
//step n; otherwise, go on with the next instruction.
bool enemygo(creature_t &creature, const grid_t &grid, int id);

//If the square the creature is facing is not a “forest” square
//and is occupied by a creature of the same species, jump to 
//step n; otherwise, go on with the next instruction.
bool samego(creature_t &creature, const grid_t &grid, int id);

//If the creature is facing the border of the grid (which we
//imagine as consisting of a huge wall), or the creature is
//facing a lake square and it cannot fly, jump to step n of
//the program; otherwise, go on with the next instruction
bool wallgo(creature_t &creature, const grid_t &grid, int id);

//This instruction always jumps to step n, independent of any condition.
void go(creature_t &creature, int id);

//simulate one turn of a creature
bool creturn(creature_t &creature, grid_t &grid, bool ifverbose);

//simulate all turns of creatures and print result
void allturn(world_t &world, bool ifverbose, int round);

#endif

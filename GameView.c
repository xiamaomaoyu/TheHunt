// GameView.c ... GameView ADT implementation
// Implementation written by Jonathan Charles, z5115933 & Nam Tran z5090191
// Finished dd/mm/16
// GameView for Fury of Dracula Game (CS1927 16s2 Project)
//nam

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "Globals.h"
#include "Game.h"
#include "GameView.h"
#include "Map.h"
#include "Places.h"

#define CHAR_PER_TURN 7
#define TURNS_PER_ROUND 5
#define NEW_GAME 0
#define DEAD 0

typedef struct _playerDetail {
   LocationID locationNumber;
   int playerHP;
   LocationID trail[TRAIL_SIZE];

}playerDetail;

struct gameView {
    Round round;
    int turnNumber;
    int score;
    Map map;
    PlayerID currentPlayer;
    playerDetail playerID[NUM_PLAYERS];
    int dracTraps[NUM_MAP_LOCATIONS];
    int dracVamps[NUM_MAP_LOCATIONS];
};

LocationID drac_trail[TRAIL_SIZE] = {UNKNOWN_LOCATION};

void toString(char *, char , char );
void recordTrail(GameView g, LocationID input,LocationID* trail);
LocationID getCityID (char , char);

// Creates a new GameView to summarise the current state of the game
GameView newGameView(char *pastPlays, PlayerMessage messages[])
{

    GameView g = malloc(sizeof(struct gameView));
    g->map = newMap();
    int i;  //temp-counter
    
    //initializing
    g->round = 0;
    g->turnNumber = 0;
    g->score = GAME_START_SCORE;
    g->currentPlayer = PLAYER_LORD_GODALMING;
    int player_counter = 0;
    for(player_counter=0;player_counter < NUM_PLAYERS;player_counter++){
        if(player_counter == PLAYER_DRACULA){
            g->playerID[player_counter].playerHP = GAME_START_BLOOD_POINTS;
        }else{
            g->playerID[player_counter].playerHP = GAME_START_HUNTER_LIFE_POINTS;
        }
        g->playerID[player_counter].locationNumber = UNKNOWN_LOCATION;
        
        for(i=0;i<TRAIL_SIZE;i++)
        g->playerID[player_counter].trail[i] = UNKNOWN_LOCATION;
    }
    for(i=0;i<NUM_MAP_LOCATIONS;i++){
        g->dracTraps[i] = 0;
        g->dracVamps[i] = 0;
    }
    player_counter = 0;
    // end of initializing
    
    //get round ,turn and currentplayer (direct information from string length)
    g->turnNumber = (strlen(pastPlays)+1)/8;
    g->round = g->turnNumber/NUM_PLAYERS;
    g->currentPlayer = g->turnNumber%NUM_PLAYERS;
    if(g->currentPlayer == PLAYER_DRACULA)
        for(i=0;i<TRAIL_SIZE;i++){
            drac_trail[i] = UNKNOWN_LOCATION;
        }
    
    
    //reading the pastplays info
    int counter_8 = 0;
    int counter_1 = 0;
    for(counter_8 = 0; counter_8 < strlen(pastPlays);counter_8+=8){     //for each turn
        counter_1 = 0;
        //counter_1 == 0                                      *************[0]****************
        switch (pastPlays[counter_1+counter_8]) {             //for the player char
            case 'G':
                player_counter = PLAYER_LORD_GODALMING; break;
            case 'S':
                player_counter = PLAYER_DR_SEWARD; break;
            case 'H':
                player_counter = PLAYER_VAN_HELSING; break;
            case 'M':
                player_counter = PLAYER_MINA_HARKER; break;
            case 'D':
                player_counter = PLAYER_DRACULA; break;
            default:
                break;
        }
        //each time drac finish a turn, score -1
        if(player_counter == PLAYER_DRACULA) g->score --;
        counter_1 ++;                                         //end of player char reading
        
        //counter_1 == 1,2                                   *************[1],[2]***************
                                                              //for the location char
                                                              // get location ..so messy
        LocationID location = 0;
        location = getCityID(pastPlays[counter_1+counter_8],pastPlays[counter_1+1+counter_8]);
        g->playerID[player_counter].locationNumber = location;              // update player.location
        recordTrail(g,location,g->playerID[player_counter].trail);          //update trail
        if(g->currentPlayer == PLAYER_DRACULA && player_counter == PLAYER_DRACULA){
            recordTrail(g,location,drac_trail);                             //update global drac_trail
        }                                                                   //todo HP things$$$$$
        
        //if dracula is at the sea, hp-2; if it's in castle hp+10
        if(player_counter == PLAYER_DRACULA ){
            if(location == CASTLE_DRACULA)
                g->playerID[PLAYER_DRACULA].playerHP += 10;
            else if (location == SEA_UNKNOWN || (validPlace(location) && isSea(location)))
                g->playerID[PLAYER_DRACULA].playerHP -= 2;
            if(location >= DOUBLE_BACK_1 && location <= DOUBLE_BACK_5){
                if(g->playerID[PLAYER_DRACULA].trail[location-DOUBLE_BACK_1+1] == SEA_UNKNOWN ||
                   isSea(g->playerID[PLAYER_DRACULA].trail[location-DOUBLE_BACK_1+1]))
                     g->playerID[PLAYER_DRACULA].playerHP -= 2;
            }
        }
        //if hunter rest in a city he gains 3 hp
        if(player_counter != PLAYER_DRACULA){
            if(g->playerID[player_counter].trail[0]==
               g->playerID[player_counter].trail[1]){
                g->playerID[player_counter].playerHP += 3;
            }
            if( g->playerID[player_counter].playerHP > 9)
                g->playerID[player_counter].playerHP = 9;
        }
        //if hunter is in the hospital he gets full hp and score -6
        if(location == ST_JOSEPH_AND_ST_MARYS){
            g->playerID[player_counter].playerHP = GAME_START_HUNTER_LIFE_POINTS;
            g->score -= 6;
        }
        counter_1+=2;
                                                              //end of location char reading
    
        //counter_1 == 3                                    **************[3]*******************
                                                              //for the trap char
        if(pastPlays[counter_1+counter_8] == 'T'){
            if(player_counter == PLAYER_DRACULA){               //dracula place a trap
                if(g->dracTraps[location] < 3) g->dracTraps[location]++;
            }else{                                              //hunters encounter a trap and loose HP
                g->dracTraps[location]--;
                g->playerID[player_counter].playerHP -= 2;
            }
            counter_1++;
        }else{
            if(player_counter == PLAYER_DRACULA) counter_1++;
        }
      
        
                                                              // end of trap char
        //counter_1 == 4                                    **************[4]*******************
                                                              // for vamp char
        if(pastPlays[counter_1+counter_8] == 'V'){
            if(player_counter == PLAYER_DRACULA){               //dracula place a vamp
                g->dracVamps[location]++;
            }else{
                g->dracVamps[location] = 0;                     //hunter kills the vamp
            }
            counter_1++;
        }else{
            if(player_counter == PLAYER_DRACULA) counter_1++;
        }
                
        //counter_1 == 5                                   **************[5]*******************
        if(pastPlays[counter_1+counter_8] == 'D'){              //if a dracula is confronted
            g->playerID[player_counter].playerHP -= 4;
            g->playerID[PLAYER_DRACULA].playerHP -= 10;
        }
        
    }
        

    
    return g;

}

// Frees all memory previously allocated for the GameView toBeDeleted
void disposeGameView(GameView toBeDeleted)
{
    disposeMap(toBeDeleted->map);
    free(toBeDeleted);
    toBeDeleted = NULL;
}
void recordTrail(GameView g,LocationID input,LocationID* trail){
    //TODO trap and vamp leave queue
    if(g->currentPlayer == PLAYER_DRACULA){         //if that's a drac
        if(g->dracTraps[trail[TRAIL_SIZE-1]] != 0){        //if there's a trap in the oldest location
            g->dracTraps[trail[TRAIL_SIZE-1]] = 0;           // delete it
        }
        if(g->dracVamps[trail[TRAIL_SIZE-1]] != 0){        //if there's a vamp
            g->dracVamps[trail[TRAIL_SIZE-1]] = 0;             //it matures
            g->score -= 13;
        }
    }
    for(int i=TRAIL_SIZE-1;i>0;i--){
        trail[i] = trail[i-1];
    }
    trail[0] = input;
}

//// Functions to return simple information about the current state of the game

// Get the current round
Round getRound(GameView currentView)
{
    return currentView->round;
}

// Get the id of current player - ie whose turn is it?
PlayerID getCurrentPlayer(GameView currentView)
{
    return currentView->currentPlayer;
}

// Get the current score
int getScore(GameView currentView)
{
    return currentView->score;
}

// Get the current health points for a given player
int getHealth(GameView currentView, PlayerID player)
{
    return currentView->playerID[player].playerHP;
}

// Get the current location id of a given player
LocationID getLocation(GameView currentView, PlayerID player)
{
    return currentView->playerID[player].locationNumber;
}

//// Functions that return information about the history of the game

// Fills the trail array with the location ids of the last 6 turns
void getHistory(GameView currentView, PlayerID player,
                            LocationID trail[TRAIL_SIZE])
{
    int i;
    for (i = 0;i < TRAIL_SIZE; i++) {
        trail[i] = currentView->playerID[player].trail[i];
    }
}

//// Functions that query the map to find information about connectivity

// Returns an array of LocationIDs for all directly connected locations
void getminions(GameView currentView,int *numTraps,
                int *numVamps,LocationID where){
    *numTraps = currentView->dracTraps[where];
    *numVamps = currentView->dracVamps[where];
    return;
}

LocationID *connectedLocations(GameView currentView, int *numLocations,
                               LocationID from, PlayerID player, Round round,
                               int road, int rail, int sea)
{
    if(road+rail+sea == 0){
        *numLocations = 0;
        return NULL;
    }
    
    int* nearlocs = malloc(sizeof(int) * NUM_MAP_LOCATIONS);
    int size = 0;
    int i = 0;
    int j = 0;
    int k = 0;
    int m = 0;
    LocationID spare = 0;
    int railsum = getScore(currentView) + player;
    Map g = newMap();
    

    
    if(from == NOWHERE){
        for(i=0;i<NUM_MAP_LOCATIONS;i++){
            if(sea ==1 && idToType(i)==SEA){
                nearlocs[j] = i;
                j++;
            }
            if(road == 1 && idToType(i)==LAND){
                nearlocs[j] = i;
                j++;
            }
        }
        *numLocations = j;
        return nearlocs;
    }
    i=0;
    j=0;
   
    while (i < NUM_MAP_LOCATIONS) {
        if (road == 1 && getTransport(g,from,i,ROAD) == ROAD) {
            if (isConnected(g,from,i) != -1) {
                nearlocs[j] = isConnected(g,from,i);
                j++;
                size++;
            }

        }

        if (sea == 1 && getTransport(g,from,i,BOAT) == BOAT) {
            if (isConnected(g,from,i) != -1) {
                
                nearlocs[j] = isConnected(g,from,i);
                j++;
                size++;
            }
        }

        if (rail == 1 && getTransport(g,from,i,RAIL) == RAIL && player != PLAYER_DRACULA) {
            
            if (railsum % 4 == 0) {
            // Can't Move so do nothing
            
            } else if (railsum % 4 == 1) {
                if (getTransport(g,from,i,RAIL) == RAIL) {
                    nearlocs[j] = isConnected(g,from,i);
                    j++;
                    size++;
                }
            } else if (railsum % 4 == 2) {
                if (getTransport(g,from,i,RAIL) == RAIL) {
                    spare = i;
                    while (k < NUM_MAP_LOCATIONS) {
                        if (getTransport(g,spare,k,RAIL) == RAIL) {
                            if (isConnected(g,spare,k) != -1) {
                                spare = isConnected(g,spare, k);
                                nearlocs[j] = spare;
                                j++;
                                size++;
                            }
                        }
                        k++;
                    }
                    k = 0;
                }
            
            } else if (railsum % 4 == 3) {
                if (getTransport(g,from,i,RAIL) == RAIL) {
                    spare = i;
                    while (k < NUM_MAP_LOCATIONS) {
                        if (getTransport(g,spare,k,RAIL) == RAIL) {
                            if (getTransport(g,from,k,RAIL) == RAIL) {
                                spare = k;
                                while (m < NUM_MAP_LOCATIONS) {
                                    if (getTransport(g,spare,m,RAIL) == RAIL) {
                                        if (isConnected(g,spare,m) != -1) {
                                        spare = isConnected(g,spare,m);
                                        nearlocs[j] = spare;
                                        j++;
                                        size++;
                                        }
                                    }
                                }
                                m++;
                            }
                        }
                        k++;
                    }
                    k = 0;
                }
            }
        }

        if (getTransport(g,from,i,ANY) == ANY && rail == 1 && sea == 1 && road == 1) {
            
            if (isConnected(g,from,i) != -1) {
                nearlocs[j] = isConnected(g,from,i);
                j++;
                size++;
            }
        }
        i++;
    }
    nearlocs[j] = from;
    size++;
    *numLocations = size;
    return nearlocs;
    
    
}



LocationID getCityID (char firstChar, char secondChar) {
    char *location = malloc(3);
    toString(location, firstChar, secondChar);
    LocationID cityID ;

    cityID = abbrevToID(location);
    if (strcmp(location, "C?") == 0) cityID = CITY_UNKNOWN;
    else if (strcmp(location, "HI") == 0)  cityID = HIDE;
    else if (strcmp(location, "S?") == 0)  cityID = SEA_UNKNOWN;
    else if (strcmp(location, "TP") == 0)  cityID = TELEPORT;
    else if (strcmp(location, "D1") == 0)  cityID = DOUBLE_BACK_1;
    else if (strcmp(location, "D2") == 0)  cityID = DOUBLE_BACK_2;
    else if (strcmp(location, "D3") == 0)  cityID = DOUBLE_BACK_3;
    else if (strcmp(location, "D4") == 0)  cityID = DOUBLE_BACK_4;
    else if (strcmp(location, "D5") == 0)  cityID = DOUBLE_BACK_5;
    free(location);

    return cityID;
}
void toString(char *location, char firstChar, char secondChar) {
    location[0] = firstChar;
    location[1] = secondChar;
    location[2] = '\0';
}

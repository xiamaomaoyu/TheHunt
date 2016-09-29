// DracView.c ... DracView ADT implementation

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "Globals.h"
#include "Game.h"
#include "GameView.h"
#include "DracView.h"
// #include "Map.h" ... if you decide to use the Map ADT

#define CHAR_PER_TURN 7
#define TURNS_PER_ROUND 5

typedef struct _playerDetail {
    LocationID locationNumber;
    int playerHP;
    int trail[TRAIL_SIZE];
}playerDetail;

struct dracView {
    GameView gameview;

};


// Creates a new DracView to summarise the current state of the game
DracView newDracView(char *pastPlays, PlayerMessage messages[])
{
    DracView dracView = malloc(sizeof(struct dracView));
    dracView->gameview = newGameView(pastPlays,messages);


    return dracView;
}


// Frees all memory previously allocated for the DracView toBeDeleted
void disposeDracView(DracView toBeDeleted)
{
    free( toBeDeleted );
    toBeDeleted = NULL;
}


//// Functions to return simple information about the current state of the game

// Get the current round
Round giveMeTheRound(DracView currentView)
{
    //REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    return getRound(currentView->gameview);
}

// Get the current score
int giveMeTheScore(DracView currentView)
{
    //REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    return getScore(currentView->gameview);
}

// Get the current health points for a given player
int howHealthyIs(DracView currentView, PlayerID player)
{
    //REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    return getHealth(currentView->gameview,player);
}

// Get the current location id of a given player
LocationID whereIs(DracView currentView, PlayerID player)
{
    //REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    return getLocation(currentView->gameview,player);
}

// Get the most recent move of a given player
void lastMove(DracView currentView, PlayerID player,
                 LocationID *start, LocationID *end)
{
    LocationID trail[TRAIL_SIZE];
    getHistory(currentView->gameview,player,
               trail);
    *start = trail[1];
    *end = trail[0];
    return;
}

// Find out what minions are placed at the specified location
void whatsThere(DracView currentView, LocationID where,
                         int *numTraps, int *numVamps)
{
    //REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    getminions(currentView->gameview,numTraps,
               numVamps,where);
    return;
}

//// Functions that return information about the history of the game

// Fills the trail array with the location ids of the last 6 turns
void giveMeTheTrail(DracView currentView, PlayerID player,
                            LocationID trail[TRAIL_SIZE])
{
    //REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    getHistory(currentView->gameview, player,trail);
    return;
}

//// Functions that query the map to find information about connectivity

// What are my (Dracula's) possible next moves (locations)
LocationID *whereCanIgo(DracView currentView, int *numLocations, int road, int sea)
{
    //REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    return connectedLocations(currentView->gameview,  numLocations,
                              getLocation(currentView->gameview, PLAYER_DRACULA),
                              PLAYER_DRACULA, giveMeTheRound(currentView),
                              road,     0,     sea);
}

// What are the specified player's next possible moves
LocationID *whereCanTheyGo(DracView currentView, int *numLocations,
                           PlayerID player, int road, int rail, int sea)
{
    //REPLACE THIS WITH YOUR OWN IMPLEMENTATION

    return connectedLocations(currentView->gameview, numLocations,
                              getLocation(currentView->gameview, player),
                              player, giveMeTheRound(currentView),
                              road, rail, sea);
}

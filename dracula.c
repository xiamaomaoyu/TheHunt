// dracula.c
// Implementation of your "Fury of Dracula" Dracula AI

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Game.h"
#include "DracView.h"
#include "Globals.h"
#include "Game.h"
#include "GameView.h"
#include "Map.h"
#include "Places.h"

void difference(LocationID* first,int numfirst,
                LocationID* second,int numsecond,
                LocationID* difference,int *numDiff);
void merege(LocationID *first,int *num1,LocationID *second,int *num2);



void decideDraculaMove(DracView gameState)
{
    
    LocationID *MeRoad;    int numMeRoad = 0;
    LocationID *MeSea;     int numMeSea = 0;

    MeRoad = whereCanIgo(gameState,&numMeRoad,1,0);
    MeSea = whereCanIgo(gameState,&numMeSea,0,1);

    //get weher they can go by road and train, store in TheyRT1
    LocationID *TheyRT1;   int numTheyRT1 = 0;
    TheyRT1 = whereCanTheyGo(gameState,&numTheyRT1,0,1,1,0);printf("can you see me?\n");
    LocationID *TheyRT2;   int numTheyRT2 = 0;
    TheyRT2 = whereCanTheyGo(gameState,&numTheyRT2,1,1,1,0);
    merege(TheyRT1,&numTheyRT1,TheyRT2,&numTheyRT2);
    TheyRT2 = whereCanTheyGo(gameState,&numTheyRT2,2,1,1,0);
    merege(TheyRT1,&numTheyRT1,TheyRT2,&numTheyRT2);
    TheyRT2 = whereCanTheyGo(gameState,&numTheyRT2,3,1,1,0);
    merege(TheyRT1,&numTheyRT1,TheyRT2,&numTheyRT2);
    

    LocationID *safeLand = malloc(NUM_MAP_LOCATIONS*sizeof(LocationID));
    int numSafeLand = 0;
    difference(MeRoad,numMeRoad,TheyRT1,numTheyRT1,safeLand,&numSafeLand);
    
    if(numSafeLand != 0){
        registerBestPlay(idToAbbrev(safeLand[numSafeLand/2]),"Ah-Ha,I am on the safe land!");
    }else if(numMeRoad > 1){
        registerBestPlay(idToAbbrev(MeRoad[numMeRoad/2]),"TAT..on a dangerous land, hope they don't find me");
    }else
        registerBestPlay(idToAbbrev(MeSea[numMeSea/2]),"I have no choice uuuuuuuuuu, I hate water uuuuu");
    
    LocationID *history = malloc(TRAIL_SIZE*sizeof(LocationID));
    giveMeTheTrail(gameState,PLAYER_DRACULA,history);
    difference(safeLand,numSafeLand,history,6,safeLand,&numSafeLand);
    if(numSafeLand != 0)
        registerBestPlay(idToAbbrev(safeLand[numSafeLand/2]),"Ah-Ha,I am on the safe land!");
    return;

}

//do A-B
//it takes O(n^2)
void difference(LocationID* first,int numfirst,
                LocationID* second,int numsecond,
                LocationID* difference,int *numDiff){
    if(numfirst == 1 && numsecond == 1 && first[0]!=second[0]){
        *numDiff = 0;
        return;
    }
    int counter1 = 0;
    int counter2 = 0;
    int counter3 = 0;
    int flag = 0;
    
    
    //for each element in first array
    for(counter1=0;counter1 < numfirst;counter1++){
        flag = 0;
        //look at all the element in second
        for(counter2 = 0;counter2 < numsecond; counter2++){
            if(first[counter1] == second[counter2]){
                //if any value in second matches
                flag = 1;   //set flag
            }
        }
        if(flag == 0){  //if flag is not set,
            //that means first[counter1] is not equal to any value in second
            difference[counter3] = first[counter1];     //record it
            counter3++;
        }
    }
    
    
    *numDiff = counter3;
    return;

}
void merege(LocationID *first,int *num1,LocationID *second,int *num2){
    int counter = 0;
    LocationID new[NUM_MAP_LOCATIONS];
    int numnew = 0;
    difference(first,*num1,second,*num2,new,&numnew);
    while(counter < numnew){
        first[*num1+counter] = new[counter];
        counter++;
    }
    *num1 = *num1+numnew;
}

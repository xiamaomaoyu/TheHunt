// Map.h ... interface to Map data type

#ifndef MAP_H
#define MAP_H

#include "Places.h"
#include "GameView.h"

typedef struct edge{
    LocationID  start;
    LocationID  end;
    TransportID type;
} Edge;

// graph representation is hidden
typedef struct MapRep *Map;

// operations on graphs 
Map  newMap();
int isConnected(Map g, LocationID from, LocationID to);
int getTransport(Map g, LocationID from, LocationID to, TransportID desired);
void disposeMap(Map g); 
void showMap(Map g); 
int  numV(Map g);
int  numE(Map g, TransportID t);

#endif

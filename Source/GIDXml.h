// -------- Include Library
#include <string.h> //Standard Library for strings 
#include <stdio.h>    //Standard Library
#include <stdlib.h>  //For allocation

#define BufferLimit 20

//Type Define Field
typedef struct Field{
	struct Field *Next;
	struct Field *Previous;
	int type; //0 by number, 1 by text
	unsigned char Info[32];
	int Effect; // If effect is 0 it will be define like None
	int Align; // 0 -> Left, 1 -> Center and 2 -> Right
	int FontWidth;
	int FontHeight; 

}Field;

//Type Define Page
typedef struct  Page{
	int ID;
	struct Page *Next;
	struct Page *Previous;
	Field *List;

}Page;

//Type Define Panel
typedef struct Panel{
	int Line;
	int Columns;
	struct Panel *Next;
	struct Panel *Previous;
	Page *List;

}Panel;

//Type Define Destination
typedef struct Destination{
	int ID;
	int Return;
	unsigned char Name[32];
	struct Destination *Next;
	struct Destination *Previous;
	Panel *List;

}Destination;

//Type Define Line
typedef struct Line{
	unsigned char Number[32];
	unsigned char Name[32];
	struct Line *Next;
	struct Line *Previous;
	Destination  *List;

}Line;

//Type Define Group
typedef struct Group{
	unsigned char ID[32];
	struct Group *Next;
	struct Group *Previous;
	Line *List;

}Group;

//Type Define GID
typedef struct GID{
	struct Group *List;

}GID;


int LoadField (FILE *Routes, Page *NewPage);
int LoadPage (FILE *Routes, Panel *NewPanel);
int LoadPanel (FILE *Routes, Destination *NewDestination);
int LoadDestination (FILE *Routes, Line *NewLine);
int LoadLines (FILE *Routes, Group *NewGroup);
int LoadGroups(FILE *Routes, GID *NewGID);
int LoadXMLSource (GID *NewGID, char *Source);
Destination* SearchingDestinationID (Destination *First, int ID);


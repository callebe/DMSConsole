// -------- Include Library
#include <string.h> //Standard Library for strings 
#include <stdio.h>    //Standard Library
#include <stdlib.h>  //For allocation

#ifndef DEF_GIDXml
	
	#define DEF_GIDXml

	#define LimitOfPages        3
	#define LimitOfFields       3
	#define LimitOfCharPerLine  29
	#define BufferLimit         20
	#define BufferLimitDisplay  512 //bits

	//Type Define Field
	typedef struct Field{
		unsigned char Info[LimitOfCharPerLine];
		unsigned char Effect; // If effect is 0 it will be define like None
		unsigned char Align; // 0 -> Left, 1 -> Center and 2 -> Right
		unsigned char FontWidth;
		unsigned char FontHeight; 

	}Field;

	//Type Define Page
	typedef struct  Page{
		unsigned char ID;
		unsigned char SpaceBetweenCharacters;
		unsigned char SpaceBetweenNumberAndCharacters;
		unsigned char PostingTime;
		unsigned char NumberOfFields;
		Field TextList[LimitOfFields];
		Field NumberList;

	}Page;

	//Type Define Panel
	typedef struct Panel{
		int Lines;
		int Columns;
		int NumberOfPages;
		struct Panel *Next;
		struct Panel *Previous;
		Page List[LimitOfPages];

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

#endif
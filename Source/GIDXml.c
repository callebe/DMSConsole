// -------- Include Library
#include <string.h> //Standard Library for strings 
#include <stdio.h>    //Standard Library
#include <stdlib.h>  //For allocation

#define XMLSource "Data/GID.xml"
#define BufferLimit 20 

//Type Define Field
typedef struct  Field{
	unsigned char Info[32];
	int Effect; // If effect is 0 it will be define like None
	int Align; // 0 -> Left, 1 -> Center and 2 -> Right
	int FontWidth;
	int FontHeight; 

}Field;

//Type Define Page
typedef struct  Page{
	Field Number;
	Field  Text;

}Page;

//Type Define Panel
typedef struct Panel{
	int Line;
	int Columns;
	struct Panel *Next;
	Page *List;

}Panel;

//Type Define Destination
typedef struct Destination{
	int ID;
	int Return;
	unsigned char Name[32];
	struct Destination *Next;
	Panel *List;

}Destination;

//Type Define Line
typedef struct Line{
	unsigned char Number[32];
	unsigned char Name[32];
	struct Line *Next;
	Destination  *List;

}Line;

//Type Define Group
typedef struct Group{
	unsigned char ID[32];
	struct Group *Next;
	Line *List;

}Group;

//Type Define GID
typedef struct GID{
	struct Group *List;

}GID;

int ReadPage (FILE *Routes, Panel *NewDestination){

	Panel *Last;
	char Buffer[BufferLimit];
	char BufferNum[8];
	unsigned char CounterLines = 0;
	int Counter = 0;

	while(!feof(Routes)){
		if(fgetc(Routes) == '<'){
			while(!feof(Routes)){
				Buffer[BufferLimit-2] = fgetc(Routes);
				if(strncmp(&Buffer[BufferLimit-7], "Panel ", 6) == 0){
					if(CounterLines == 0){
						CounterLines = 1;
						NewDestination->List = (Panel *)malloc(sizeof(Panel));
						Last = NewDestination->List;

					}
					else{
						while(Last->Next != NULL) Last = Last->Next;
						Last->Next = (Panel *)malloc(sizeof(Panel));
						Last = Last->Next;

					}
					Last->List = NULL;
					Last->Next = NULL;
					for(int i = 0; i<(BufferLimit-1); i++){
						Buffer[i] = Buffer[i+1];
					}
					while(!feof(Routes) && (Buffer[BufferLimit-2] = fgetc(Routes)) != '>'){
						if(strncmp(&Buffer[BufferLimit-8], "Lines=\"", 7) == 0){
							Counter = 0;
							while(!feof(Routes) && (BufferNum[Counter++] = fgetc(Routes)) != '\"' && Counter<8);
							if(Counter < 1){
								printf("Erro, Xml Reader found a Panel without Number of Lines!\n");
								return 1;
							}
							BufferNum[Counter-1] = '\0';
							Last->Line = atoi(BufferNum);

						}
						else{
							if(strncmp(&Buffer[BufferLimit-10], "Columns=\"", 9) == 0){
								Counter = 0;
								while(!feof(Routes) && (BufferNum[Counter++] = fgetc(Routes)) != '\"' && Counter<31);
								if(Counter < 2){
									printf("Erro, Xml Reader found a Panel without Number of Columns!\n");
									return 1;
								}
								BufferNum[Counter++] = '\0';
								Last->Columns = atoi(BufferNum);
							}
	
						}
						for(int i = 0; i<(BufferLimit-1); i++){
							Buffer[i] = Buffer[i+1];
						}
					}
					if(ReadPage(Routes, Last)){
					 	return 1;
					 	
					}

				}
				for(int i = 0; i<(BufferLimit-1); i++){
					Buffer[i] = Buffer[i+1];
				}
			}
			
		}

	}

	return 0;

}

int ReadPanel (FILE *Routes, Destination *NewDestination){

	Panel *Last;
	char Buffer[BufferLimit];
	char BufferNum[8];
	unsigned char CounterLines = 0;
	int Counter = 0;

	while(!feof(Routes)){
		if(fgetc(Routes) == '<'){
			while(!feof(Routes)){
				Buffer[BufferLimit-2] = fgetc(Routes);
				if(strncmp(&Buffer[BufferLimit-7], "Panel ", 6) == 0){
					if(CounterLines == 0){
						CounterLines = 1;
						NewDestination->List = (Panel *)malloc(sizeof(Panel));
						Last = NewDestination->List;

					}
					else{
						while(Last->Next != NULL) Last = Last->Next;
						Last->Next = (Panel *)malloc(sizeof(Panel));
						Last = Last->Next;

					}
					Last->List = NULL;
					Last->Next = NULL;
					for(int i = 0; i<(BufferLimit-1); i++){
						Buffer[i] = Buffer[i+1];
					}
					while(!feof(Routes) && (Buffer[BufferLimit-2] = fgetc(Routes)) != '>'){
						if(strncmp(&Buffer[BufferLimit-8], "Lines=\"", 7) == 0){
							Counter = 0;
							while(!feof(Routes) && (BufferNum[Counter++] = fgetc(Routes)) != '\"' && Counter<8);
							if(Counter < 1){
								printf("Erro, Xml Reader found a Panel without Number of Lines!\n");
								return 1;
							}
							BufferNum[Counter-1] = '\0';
							Last->Line = atoi(BufferNum);

						}
						else{
							if(strncmp(&Buffer[BufferLimit-10], "Columns=\"", 9) == 0){
								Counter = 0;
								while(!feof(Routes) && (BufferNum[Counter++] = fgetc(Routes)) != '\"' && Counter<31);
								if(Counter < 2){
									printf("Erro, Xml Reader found a Panel without Number of Columns!\n");
									return 1;
								}
								BufferNum[Counter++] = '\0';
								Last->Columns = atoi(BufferNum);
							}
	
						}
						for(int i = 0; i<(BufferLimit-1); i++){
							Buffer[i] = Buffer[i+1];
						}
					}
					if(ReadPage(Routes, Last)){
					 	return 1;
					 	
					}

				}
				for(int i = 0; i<(BufferLimit-1); i++){
					Buffer[i] = Buffer[i+1];
				}
			}
			
		}

	}

	return 0;

}

int ReadDestination (FILE *Routes, Line *NewGroup){

	Destination *Last;
	char Buffer[BufferLimit];
	char BufferNum[8];
	unsigned char CounterLines = 0;
	int Counter = 0;

	while(!feof(Routes)){
		if(fgetc(Routes) == '<'){
			while(!feof(Routes)){
				Buffer[BufferLimit-2] = fgetc(Routes);
				if(strncmp(&Buffer[BufferLimit-13], "Destination ", 12) == 0){
					if(CounterLines == 0){
						CounterLines = 1;
						NewGroup->List = (Destination *)malloc(sizeof(Destination));
						Last = NewGroup->List;

					}
					else{
						while(Last->Next != NULL) Last = Last->Next;
						Last->Next = (Destination *)malloc(sizeof(Destination));
						Last = Last->Next;

					}
					Last->List = NULL;
					Last->Next = NULL;
					for(int i = 0; i<(BufferLimit-1); i++){
						Buffer[i] = Buffer[i+1];
					}
					while(!feof(Routes) && (Buffer[BufferLimit-2] = fgetc(Routes)) != '>'){
						if(strncmp(&Buffer[BufferLimit-5], "ID=\"", 4) == 0){
							Counter = 0;
							while(!feof(Routes) && (BufferNum[Counter++] = fgetc(Routes)) != '\"' && Counter<8);
							if(Counter < 1){
								printf("Erro, Xml Reader found a Destination without ID!\n");
								return 1;
							}
							BufferNum[Counter-1] = '\0';
							Last->ID = atoi(BufferNum);

						}
						else{
							if(strncmp(&Buffer[BufferLimit-7], "Name=\"", 6) == 0){
								Counter = 0;
								while(!feof(Routes) && (Last->Name[Counter++] = fgetc(Routes)) != '\"' && Counter<31);
								if(Counter < 2){
									printf("Erro, Xml Reader found a Destination without Name!\n");
									return 1;
								}
								Last->Name[Counter-1] = '\0';
							}
							else{
								if(strncmp(&Buffer[BufferLimit-9], "Return=\"", 8) == 0){
									Counter = 0;
									while(!feof(Routes) && (BufferNum[Counter++] = fgetc(Routes)) != '\"' && Counter<31);
									if(Counter < 2){
										printf("Erro, Xml Reader found a Destination without Retrun!\n");
										return 1;
									}
									BufferNum[Counter-1] = '\0';
									Last->Return = atoi(BufferNum);

								}

							}
						}
						for(int i = 0; i<(BufferLimit-1); i++){
							Buffer[i] = Buffer[i+1];
						}
					}
					if(ReadPanel(Routes, Last)){
					 	return 1;

					}

				}
				for(int i = 0; i<(BufferLimit-1); i++){
					Buffer[i] = Buffer[i+1];
				}
			}
			
		}

	}

	return 0;

}

int ReadLines (FILE *Routes, Group *NewGroup){

	Line *Last;
	char Buffer[BufferLimit];
	unsigned char CounterLines = 0;

	while(!feof(Routes)){
		if(fgetc(Routes) == '<'){
			while(!feof(Routes)){
				Buffer[BufferLimit-2] = fgetc(Routes);
				if(strncmp(&Buffer[BufferLimit-6], "Line ", 5) == 0){
					if(CounterLines == 0){
						CounterLines = 1;
						NewGroup->List = (Line *)malloc(sizeof(Line));
						Last = NewGroup->List;

					}
					else{
						while(Last->Next != NULL) Last = Last->Next;
						Last->Next = (Line *)malloc(sizeof(Line));
						Last = Last->Next;

					}
					Last->List = NULL;
					Last->Next = NULL;
					for(int i = 0; i<(BufferLimit-1); i++){
						Buffer[i] = Buffer[i+1];
					}
					while(!feof(Routes) && (Buffer[BufferLimit-2] = fgetc(Routes)) != '>'){
						int Counter = 0;
						if(strncmp(&Buffer[BufferLimit-9], "Number=\"", 8) == 0){
							while(!feof(Routes) && (Last->Number[Counter++] = fgetc(Routes)) != '\"' && Counter<31);
							if(Counter < 2){
								printf("Erro, Xml Reader found a Line without Number!\n");
								return 1;
							}
							Last->Number[Counter-1] = '\0';
						}
						else{
							if(strncmp(&Buffer[BufferLimit-7], "Name=\"", 6) == 0){
								while(!feof(Routes) && (Last->Name[Counter++] = fgetc(Routes)) != '\"' && Counter<31);
								if(Counter < 2){
									printf("Erro, Xml Reader found a Line without Name!\n");
									return 1;
								}
								Last->Name[Counter-1] = '\0';
							}
						}
						for(int i = 0; i<(BufferLimit-1); i++){
							Buffer[i] = Buffer[i+1];
						}
					}
					if(ReadDestination(Routes, Last)){
					 	return 1;
					}
					printf("--> ID: %s\n", Last->Number);
					printf("\t  ID: %s\n", Last->Name);

				}
				for(int i = 0; i<(BufferLimit-1); i++){
					Buffer[i] = Buffer[i+1];
				}
			}
			
		}

	}

	return 0;

}

int ReadGroups(FILE *Routes, GID *NewGID){

	Group *Last;
	char Buffer[BufferLimit];
	unsigned char CounterGroups = 0;

	while(!feof(Routes)){
		if(fgetc(Routes) == '<'){
			while(!feof(Routes)){
				Buffer[BufferLimit-2] = fgetc(Routes);
				if(strncmp(&Buffer[BufferLimit-7], "Group ", 6) == 0){
					if(CounterGroups == 0){
						CounterGroups = 1;
						NewGID->List = (Group *)malloc(sizeof(Group));
						Last = NewGID->List;

					}
					else{
						while(Last->Next != NULL) Last = Last->Next;
						Last->Next = (Group *)malloc(sizeof(Group));
						Last = Last->Next;

					}
					Last->List = NULL;
					Last->Next = NULL;
					for(int i = 0; i<(BufferLimit-1); i++){
						Buffer[i] = Buffer[i+1];
					}
					while(!feof(Routes) && (Buffer[BufferLimit-2] = fgetc(Routes)) != '>'){
						int Counter = 0;
						if(strncmp(&Buffer[BufferLimit-5], "ID=\"", 4) == 0){
							while(!feof(Routes) && (Last->ID[Counter++] = fgetc(Routes)) != '\"' && Counter<31);
							if(Counter < 2){
								printf("Erro, Xml Reader found a Group without ID!\n");
								return 1;
							}
							Last->ID[Counter-1] = '\0';
						}
						for(int i = 0; i<(BufferLimit-1); i++){
							Buffer[i] = Buffer[i+1];
						}
					}
					if(ReadLines(Routes, Last)){
						return 1;
					}

				}
				for(int i = 0; i<(BufferLimit-1); i++){
					Buffer[i] = Buffer[i+1];
				}
			}
			
		}

	}

	return 0;

}

//Function by Open Xml Source File
int ReadXMLSource (GID *NewGID){
	
	char Buffer[BufferLimit];
	int Break;

	FILE *Routes;
	NewGID->List = NULL;

	Buffer[BufferLimit-1] = '\0';

	//Loading Routes file
	Routes = fopen(XMLSource,"r");
	if(Routes == NULL){
		printf("--> Read erro in %s !\n", XMLSource);
		return 1;

	}

	while(!feof(Routes)){
		if(fgetc(Routes) == '<'){
			Break = 0;
			while(Break == 0 && !feof(Routes)){
				Buffer[BufferLimit-2] = fgetc(Routes);
				if(strncmp(&Buffer[BufferLimit-5], "GID ", 4) == 0){
					if(ReadGroups(Routes, NewGID)){
						printf("Erro in Xml Source Sintax!\n");
					}
					Break = 1;
				}
				for(int i = 0; i<(BufferLimit-2); i++){
					Buffer[i] = Buffer[i+1];
				}
			}
			
		}

	}

	return 0;

}

int main (void){

	GID NewGID;
	ReadXMLSource(&NewGID);

}
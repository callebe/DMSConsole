// -------- Include Library
#include "GIDXml.h" //Headers Functions of XmlReader

//Function by Load Field
int LoadField (FILE *Routes, Page *NewPage){

	Field *Last = NewPage->List;
	char Buffer[BufferLimit];
	char BufferNum[8];
	int Counter = 0;
	unsigned char Break = 0;
	Buffer[BufferLimit-1] = '\0';

	while(!feof(Routes) && (Break != 1)){
		Buffer[BufferLimit-2] = fgetc(Routes);
		if((strncmp(&Buffer[BufferLimit-8], "Number ", 7) == 0) || (strncmp(&Buffer[BufferLimit-6], "Text ", 5) == 0)){
			if(Last == NULL){
				NewPage->List = (Field *)malloc(sizeof(Field));
				Last = NewPage->List;
				Last->Previous = NULL;

			}
			else{
				while(Last->Next != NULL) Last = Last->Next;
				Last->Next = (Field *)malloc(sizeof(Field));
				Last->Next->Previous = Last;
				Last = Last->Next;

			}
			Last->Next = NULL;
			if((strncmp(&Buffer[BufferLimit-6], "Text ", 5) == 0)) Last->type = 1;
			else Last->type = 0;
			for(int i = 0; i<(BufferLimit-1); i++){
				Buffer[i] = Buffer[i+1];
			}
			while(!feof(Routes) && (Buffer[BufferLimit-2] = fgetc(Routes)) != '>'){
				if(strncmp(&Buffer[BufferLimit-9], "Effect=\"", 8) == 0){
					Counter = 0;
					while(!feof(Routes) && (BufferNum[Counter++] = fgetc(Routes)) != '\"' && Counter<8);
					if(Counter < 1){
						printf("Erro, Xml Reader found a Fild without Effect!\n");
						return 1;
					}
					BufferNum[Counter-1] = '\0';
					if(strncmp(&BufferNum[0], "Piscar", 6) == 0) Last->Effect = 1;
					else{
						if(strncmp(&BufferNum[0], "Rodar", 5) == 0) Last->Effect = 2;
						else Last->Effect = 0;
							
					}
				}
				else{
					if(strncmp(&Buffer[BufferLimit-8], "Align=\"", 7) == 0){
						Counter = 0;
						while(!feof(Routes) && (BufferNum[Counter++] = fgetc(Routes)) != '\"' && Counter<8);
						if(Counter < 1){
							printf("Erro, Xml Reader found a Fild without Align!\n");
							return 1;
						}
						BufferNum[Counter-1] = '\0';
						if(strncmp(&BufferNum[0], "Center", 6) == 0) Last->Align = 1;
						else{
							if(strncmp(&BufferNum[0], "Right", 5) == 0) Last->Align = 2;
							else Last->Align = 0;
						}

					}
					else{
						if(strncmp(&Buffer[BufferLimit-7], "Font=\"", 6) == 0){
							Counter = 0;
							while(!feof(Routes) && (BufferNum[Counter++] = fgetc(Routes)) != 'x' && Counter<8);
							if(Counter < 1){
								printf("Erro, Xml Reader found a Fild without Font!\n");
								return 1;
							}
							BufferNum[Counter-1] = '\0';
							Last->FontHeight = atoi(BufferNum);
							Counter = 0;
							while(!feof(Routes) && (BufferNum[Counter++] = fgetc(Routes)) != '\"' && Counter<8);
							if(Counter < 1){
								printf("Erro, Xml Reader found a Fild without Font!\n");
								return 1;
							}
							BufferNum[Counter-1] = '\0';
							Last->FontWidth = atoi(BufferNum);

						}

					}

				}
				for(int i = 0; i<(BufferLimit-1); i++){
					Buffer[i] = Buffer[i+1];
				}
			
			}
			if(Buffer[BufferLimit-2] == '>'){
				Counter = 0;
			 	while(!feof(Routes) && (Last->Info[Counter++] = fgetc(Routes)) != '<');
			 	Last->Info[Counter-1] = '\0';

			}
			
		}
		else{
			if(strncmp(&Buffer[BufferLimit-8], "</Page>", 7) == 0) Break = 1;

		}
		for(int i = 0; i<(BufferLimit-1); i++){
			Buffer[i] = Buffer[i+1];
		}
	}
	if(Break == 0){
		printf("Error Page without Filds!\n\n");
		return 1;
	}
	else 
		return 0;

}

//Function by Load Page
int LoadPage (FILE *Routes, Panel *NewPanel){

	Page *Last = NewPanel->List;
	char Buffer[BufferLimit];
	char BufferNum[8];
	unsigned char Break = 0;
	int Counter = 0;
	Buffer[BufferLimit-1] = '\0';


	while(!feof(Routes) && (Break != 1)){
		Buffer[BufferLimit-2] = fgetc(Routes);
		if(strncmp(&Buffer[BufferLimit-6], "Page ", 5) == 0){
			if(Last == NULL){
				NewPanel->List = (Page *)malloc(sizeof(Page));
				Last = NewPanel->List;
				Last->Previous = NULL;

			}
			else{
				while(Last->Next != NULL) Last = Last->Next;
				Last->Next = (Page *)malloc(sizeof(Page));
				Last->Next->Previous = Last;
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
						printf("Erro, Xml Reader found a Page without ID!\n");
						return 1;
					}
					BufferNum[Counter-1] = '\0';
					Last->ID = atoi(BufferNum);

				}
				for(int i = 0; i<(BufferLimit-1); i++){
					Buffer[i] = Buffer[i+1];
				}
			}
			if(LoadField(Routes, Last)){
			 	return 1;
			 	
			}

		}
		else{
			if(strncmp(&Buffer[BufferLimit-9], "</Panel>", 8) == 0) Break = 1;

		}
		for(int i = 0; i<(BufferLimit-1); i++){
			Buffer[i] = Buffer[i+1];
		}
	}
			
	if(Break == 0){
		printf("Error Lines without Page!\n\n");
		return 1;
	}
	else 
		return 0;

}

//Function by Load Panel
int LoadPanel (FILE *Routes, Destination *NewDestination){

	Panel *Last = NewDestination->List;
	unsigned char Break = 0;
	char Buffer[BufferLimit];
	char BufferNum[8];
	int Counter = 0;


	while(!feof(Routes) && (Break !=1)){
		Buffer[BufferLimit-2] = fgetc(Routes);
		if(strncmp(&Buffer[BufferLimit-7], "Panel ", 6) == 0){
			if(Last == NULL){
				NewDestination->List = (Panel *)malloc(sizeof(Panel));
				Last = NewDestination->List;
				Last->Previous = NULL;

			}
			else{
				while(Last->Next != NULL) Last = Last->Next;
				Last->Next = (Panel *)malloc(sizeof(Panel));
				Last->Next->Previous = Last;
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
			if(LoadPage(Routes, Last)){
			 	return 1;
			 	
			}

		}
		else{
			if(strncmp(&Buffer[BufferLimit-15], "</Destination>", 14) == 0) Break = 1;

		}
		for(int i = 0; i<(BufferLimit-1); i++){
			Buffer[i] = Buffer[i+1];
		}
	}
	if(Break == 0){
		printf("Error Destination without Lines!\n\n");
		return 1;
	}
	else 
		return 0;

}

//Function by Load Destination
int LoadDestination (FILE *Routes, Line *NewLine){

	Destination *Last = NewLine->List;
	char Buffer[BufferLimit];
	char BufferNum[8];
	unsigned char Break = 0;
	int Counter = 0;


	while(!feof(Routes) && (Break != 1)){
		Buffer[BufferLimit-2] = fgetc(Routes);
		if(strncmp(&Buffer[BufferLimit-13], "Destination ", 12) == 0){
			if(Last == NULL){
				NewLine->List = (Destination *)malloc(sizeof(Destination));
				Last = NewLine->List;
				Last->Previous = NULL;

			}
			else{
				while(Last->Next != NULL) Last = Last->Next;
				Last->Next = (Destination *)malloc(sizeof(Destination));
				Last->Next->Previous = Last;
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
			if(LoadPanel(Routes, Last)){
			 	return 1;

			}

		}
		else{
			if(strncmp(&Buffer[BufferLimit-8], "</Line>", 7) == 0) Break = 1;

		}
		for(int i = 0; i<(BufferLimit-1); i++){
			Buffer[i] = Buffer[i+1];
		}
	}
	if(Break == 0){
		printf("Error Line without Destination!\n\n");
		return 1;
	}
	else 
		return 0;

}

//Function by Load Lines
int LoadLines (FILE *Routes, Group *NewGroup){

	Line *Last = NewGroup->List;
	char Buffer[BufferLimit];
	unsigned char Break = 0;


	while(!feof(Routes) && (Break != 1)){
		Buffer[BufferLimit-2] = fgetc(Routes);
		if(strncmp(&Buffer[BufferLimit-6], "Line ", 5) == 0){
			if(Last == NULL){
				NewGroup->List = (Line *)malloc(sizeof(Line));
				Last = NewGroup->List;
				Last->Previous = NULL;

			}
			else{
				while(Last->Next != NULL) Last = Last->Next;
				Last->Next = (Line *)malloc(sizeof(Line));
				Last->Next->Previous = Last;
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
			if(LoadDestination(Routes, Last)){
			 	return 1;
			}

		}
		else{
			if(strncmp(&Buffer[BufferLimit-9], "</Group>", 8) == 0) Break = 1;

		}
		for(int i = 0; i<(BufferLimit-1); i++){
			Buffer[i] = Buffer[i+1];
		}
	}

	if(Break == 0){
		printf("Error Group without Line!\n\n");
		return 1;
	}
	else 
		return 0;

}

//Function by Load Groups
int LoadGroups(FILE *Routes, GID *NewGID){

	Group *Last = NewGID->List;
	char Buffer[BufferLimit];
	unsigned char Break = 0;


	while(!feof(Routes) && Break != 1){
		Buffer[BufferLimit-2] = fgetc(Routes);
		if(strncmp(&Buffer[BufferLimit-7], "Group ", 6) == 0){
			if(Last == NULL){
				NewGID->List = (Group *)malloc(sizeof(Group));
				Last = NewGID->List;
				Last->Previous = NULL;

			}
			else{
				while(Last->Next != NULL) Last = Last->Next;
				Last->Next = (Group *)malloc(sizeof(Group));
				Last->Next->Previous = Last;
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
			if(LoadLines(Routes, Last)){
				return 1;
			}

		}
		else{
			if(strncmp(&Buffer[BufferLimit-7], "</GID>", 6) == 0) Break = 1;

		}
		for(int i = 0; i<(BufferLimit-1); i++){
			Buffer[i] = Buffer[i+1];
		}
	}

	if(Break == 0){
		printf("Error GID without Group!\n\n");
		return 1;
	}
	else 
		return 0;

}

//Function by Load Xml Source File
int LoadXMLSource (GID *NewGID, char *Source){
	
	char Buffer[BufferLimit];
	unsigned char Break = 0;

	FILE *Routes;
	NewGID->List = NULL;

	Buffer[BufferLimit-1] = '\0';

	//Loading Routes file
	Routes = fopen(Source,"r");
	if(Routes == NULL){
		printf("--> Read erro in %s !\n", Source);
		return 1;

	}

	while(!feof(Routes) && Break != 1){
		Buffer[BufferLimit-2] = fgetc(Routes);
		if(strncmp(&Buffer[BufferLimit-5], "GID ", 4) == 0){
			if(LoadGroups(Routes, NewGID)){
				printf("Erro in Xml Source Sintax!\n");
				return 1;
			}
			Break = 1;
		}
		for(int i = 0; i<(BufferLimit-2); i++){
			Buffer[i] = Buffer[i+1];
		}
	}
			
	if(Break == 0){
		printf("Error Xml without GID!\n\n");
		return 1;
	}
	else 
		return 0;

}

Destination* SearchingDestinationID (Destination *First, int ID){

	int Break = 0;
	Destination *Last = First;

	while(Last != NULL && Break != 1){
		if(Last->ID == ID){
			Break = 1;
		}
		else{
			Last = Last->Next;
		}

	}

	return Last;

}

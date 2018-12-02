#define _CRT_SECURE_NO_WARNINGS //nechat prvni

#include <stdio.h>
#include <stdlib.h>

#include "check.h" //nechat posledni
#define MIN_ALOKACE 10

typedef struct Node {
	int value;
	int nConnections;
	int *connections;
}Node;

typedef struct Graph {
	Node *nodes;
	int numNodes;
}Graph;

Graph* LoadGraphFromFile(FILE *f) {
	//alokace pole node podle delky nacitaneho grafu
	//ulozeni delky grafu do numNodes
	int c;
	int numNodes = 0; //pocet prvku
	int *numConnections; //pocet propoju mezi uzly 
	int numComma = 0; // pocet carek
	int alloccated = MIN_ALOKACE;
	Graph* graph;

	numConnections = calloc(MIN_ALOKACE, sizeof(int));
	do
	{
		c = fgetc(f);
		if (c == ':') //zjistovani poctu uzlu pomoci dvojtecky
		{
			numNodes++;
			if (numNodes == alloccated)
			{
				alloccated += MIN_ALOKACE;
				numConnections = realloc(numConnections, alloccated * sizeof(int)); // pokud nemam dostatek mista tak prealokuju
			}
		}
		if (c == ',')numConnections[numNodes - 1]++; //zjistovani poctu propoju pro dany uzel 


	} while (c != EOF);
	graph = malloc(sizeof(Graph));// alokace grafu
	graph->numNodes = numNodes;
	graph->nodes = malloc(numNodes * sizeof(Node));
	for (int i = 0; i < numNodes; i++)
	{
		graph->nodes[i].nConnections = numConnections[i] + 1;
		graph->nodes[i].connections = malloc(graph->nodes[i].nConnections * sizeof(int));
	}
	free(numConnections);
	numConnections = NULL;

	rewind(f);
	//prochazeni souboru a naplneni jiz naalokovaneho prostoru
	for (int i = 0; i < graph->numNodes; i++) {
		int read = fscanf(f, "%d:", &(graph->nodes[i].value));
		//printf("Read %d values; Node id: %d\r\n", read, graph->nodes[i].value);

		for (int j = 0; j < graph->nodes[i].nConnections; j++) {
			int readNodeId;
			int read;
			if (j < graph->nodes[i].nConnections - 1) read = fscanf(f, "%d,", &readNodeId);
			else read = fscanf(f, "%d", &readNodeId);
			//printf("	Read %d values; Conn to: %d\r\n", read, readNodeId);
			graph->nodes[i].connections[j] = readNodeId;
		}
	}
	return graph;
}

void PrintGraph(Graph *graph) {
	//vytiskne graf jako seznam uzluuu
	for (int i = 0; i < graph->numNodes; i++) {
		printf("Uzel s hodnotou %d\r\n", graph->nodes[i].value);
	}
}

void DestroyGraph(Graph *graph) {
	//odalokuje prostor v pameti zabrany grafem
	for (int i = 0; i < graph->numNodes; i++) {
		free(graph->nodes[i].connections); //odalokuje pole odkazu pro kazdy prvek
		graph->nodes[i].connections = NULL;
	}
	free(graph->nodes); //odalokuje pole prvku
	//free(graph);
}

Graph *FindIntersects(Graph *graphA, Graph *graphB) {
	int numIntersects = 0;
	
	//prohledam oba grafy a zjistim pocet spolecnych prvku
	for (int i = 0; i < graphA->numNodes; i++) {
		for (int j = 0; j < graphB->numNodes; j++) {
			if (graphA->nodes[i].value == graphB->nodes[j].value) {
				numIntersects++;
				break;
			}
		}
	}

	//ted uz vim kolik jich je, muzu naalokovat a naplnit daty
	Graph *intersects = malloc(sizeof(Graph)); //alokace vysledku
	intersects->numNodes = numIntersects;
	intersects->nodes = malloc(numIntersects * sizeof(Node));

	//prohledam oba grafy znova a ulozim shodne prvky
	int currentIndex = 0;
	for (int i = 0; i < graphA->numNodes; i++) {
		for (int j = 0; j < graphB->numNodes; j++) {
			if (graphA->nodes[i].value == graphB->nodes[j].value) {
				//oba grafy obsahuji tento prvek, nakopirovat i propoje
				intersects->nodes[currentIndex].value = graphA->nodes[i].value;
				intersects->nodes[currentIndex].nConnections = graphA->nodes[i].nConnections;
				intersects->nodes[currentIndex].connections = malloc(graphA->nodes[i].nConnections * sizeof(int));

				for (int x = 0; x < intersects->nodes[i].nConnections; x++) {
					intersects->nodes[currentIndex].connections[x] = graphA->nodes[i].connections[x];
				}
				currentIndex++;
				break;
			}
		}
	}
	
	return intersects;
}

/*
int N(int vertex){
	int c = 0;
	Graph l = ??
	for (?? {
		if (i==1) c+=1;
		return l
	}
}

*/

void FindCliques(Graph *source, Graph **foundCliques, int *numFoundCliques) {
	//vyhleda nejvetsi kliky v grafu source a vrati je v poli foundCliques, jejich pocet ulozi do numFoundCliques
	//Bron–Kerbosch algorithm https://en.wikipedia.org/wiki/Bron%E2%80%93Kerbosch_algorithm?fbclid=IwAR1LpZZxsoxn0MeqUbiZahtBIuifaBAsSLxdx7oUbqiekWcsbgwWxds-wQU
	
	int exampleNumCliques = 2;
	int exampleNumNodes1 = 3;
	int exampleNumNodes2 = 5;
	int exampleNumConnections = 1;

	/*
	Graph sourceNew;
	Graph foundCliquesNew;
	int numFoundCliquesNew;

	if ( sizeof(NumNodes1) == 0 && sizeof(NumNodes2){
		printf(&numFoundCliques);
	}
	for (vertex = 0; vertex <=kdovico; vertex++){
		foundCliquesNew = source[vertex];
		//sourceNew.numnodes = buhvico;
		if (source == N(vertex){
			sourceNew = N(vertex);
		}
		if (numFoundCliques == N(vertex){
			numFoundCliquesNew = N(vvertex);
		}

		free(*vertex);
		FindCliques (sourceNew, foundCliquesNew, numFoundCliquesNew);

	}
	*/

	
	
	(*foundCliques) = malloc(exampleNumCliques * sizeof(Graph)); //alokace poctu grafu
	(*foundCliques)[0].nodes = calloc(exampleNumNodes1, sizeof(Node)); //alokace poctu uzlu pro prvni graf
	(*foundCliques)[0].numNodes = exampleNumNodes1;
	for (int i = 0; i < exampleNumNodes1; i++) {
		(*foundCliques)[0].nodes[i].nConnections = exampleNumConnections;
		(*foundCliques)[0].nodes[i].connections = calloc(exampleNumConnections, sizeof(Node*)); //alokace poctu propoju pro kazdy uzel
	}


	(*foundCliques)[1].nodes = calloc(exampleNumNodes2, sizeof(Node)); //alokace poctu uzlu pro druhy graf
	(*foundCliques)[1].numNodes = exampleNumNodes2;
	for (int i = 0; i < exampleNumNodes2; i++) {
		(*foundCliques)[1].nodes[i].nConnections = exampleNumConnections;
		(*foundCliques)[1].nodes[i].connections = calloc(exampleNumConnections, sizeof(Node*)); //alokace poctu propoju pro kazdy uzel
	}
	
	*numFoundCliques = exampleNumCliques;
}

int main(int argc, char **argv) {
	//////////////////////////////// INICIALIZACE, NACTENI ZE SOUBORU ////////////////////////////////
	Graph *graph;

	if (argc != 2) {
		fprintf(stderr, "Tento program se spousti s jednim parametrem, kterym je nazev souboru s grafem!\r\n");
		return 1;
	}

	FILE *f = fopen(argv[1], "r");
	if (!f) {
		fprintf(stderr, "Nezdarilo se otevrit soubor \"%s\"!\r\n", argv[1]);
		return 2;
	}

	graph = LoadGraphFromFile(f);
	fclose(f); //zavru soubor, mam nacteno
	f = NULL;

	//TEST FindIntersects
	/*
	FILE *a = fopen("..\\..\\Test-Graph\\Graph1.txt", "r");
	FILE *b = fopen("..\\..\\Test-Graph\\Graph2.txt", "r");
	Graph *graphA = LoadGraphFromFile(a);
	Graph *graphB = LoadGraphFromFile(b);
	Graph *res = FindIntersects(graphA, graphB);

	printf("\r\nA:\r\n");
	PrintGraph(graphA);
	printf("\r\nB:\r\n");
	PrintGraph(graphB);
	printf("\r\nResult:\r\n");
	PrintGraph(res);
	*/
	////////////////////////////////////// HLEDANI KLIK /////////////////////////////////////////////////
	Graph *clicques = NULL;  //pole grafu, ktere budou obsahovat uzly nacteneho grafu, ktere jsou soucasti klik
	int numClicques = 0;  //pocet klik
	FindCliques(graph, &clicques, &numClicques);

	for (int i = 0; i < numClicques; i++) {
		printf("\r\nNalezena klika:\r\n");
		PrintGraph(&(clicques[i]));
	}

	////////////////////////////////////// ODALOKACE, KONEC ///////////////////////////////////////////////
	DestroyGraph(graph); //odalokuju pamet grafu nacteneho ze souboru
	free(graph); //odalokuje strukturu
	graph = NULL;

	for (int i = 0; i < numClicques; i++) { //odalokuju jednotlive grafy (kliky)
		DestroyGraph(&(clicques[i]));
	}
	free(clicques); //odalokuju pole grafu
	clicques = NULL;
	return 0;
}
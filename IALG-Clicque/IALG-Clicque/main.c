#define _CRT_SECURE_NO_WARNINGS //nechat prvni

#include <stdio.h>
#include <stdlib.h>
#define MIN_ALOKACE 10

typedef struct Node {
	int value;
	int nConnections;
	struct Node **connections;
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
		graph->nodes[i].connections = malloc(graph->nodes[i].nConnections * sizeof(Node*));
	}
	free(numConnections);
	rewind(f);
	//dopsat prochazeni souboru a naplneni jiz naalokovaneho prostoru

}

void PrintGraph(Graph *graph) {
	//vytiskne graf jako seznam uzluuu
	for (int i = 0; i < graph->numNodes; i++) {
		printf("Uzel s hodnotou %d\r\n", graph->nodes[i].value);
	}
}

void FindCliques(Graph *source, Graph **foundCliques, int *numFoundCliques) {
	//vyhleda nejvetsi kliky v grafu source a vrati je v poli foundCliques, jejich pocet ulozi do numFoundCliques
	//Bron–Kerbosch algorithm https://en.wikipedia.org/wiki/Bron%E2%80%93Kerbosch_algorithm?fbclid=IwAR1LpZZxsoxn0MeqUbiZahtBIuifaBAsSLxdx7oUbqiekWcsbgwWxds-wQU
}

int main(int argc, char **argv) {
	Graph graph;

	if (argc != 2) {
		fprintf(stderr, "Tento program se spousti s jednim parametrem, kterym je nazev souboru s grafem!\r\n");
		return 1;
	}

	FILE *f = fopen(argv[1], "r");
	if (!f) {
		fprintf(stderr, "Nezdarilo se otevrit soubor \"%s\"!\r\n", argv[1]);
		return 2;
	}

	LoadGraphFromFile(f, &graph);

	//.. algoritmus pro hledani kliky	
	Graph *clicques;  //bude obsahovat uzly grafu, ktere jsou soucasti kliky
	int numClicques;  //pocet klik
	FindCliques(&graph, &clicques, &numClicques);

	for (int i = 0; i < numClicques; i++) {
		printf("\r\nNalezena klika:\r\n");
		PrintGraph(&(clicques[i]));
	}

	return 0;
}
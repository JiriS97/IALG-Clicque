#define _CRT_SECURE_NO_WARNINGS //nechat prvni

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
				memset(&(numConnections[alloccated - MIN_ALOKACE]), 0, MIN_ALOKACE * sizeof(int));
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
	printf("Graf s %d uzly\r\n", graph->numNodes);
	for (int i = 0; i < graph->numNodes; i++) {
		printf(" Uzel s hodnotou %d, %d spoju: ", graph->nodes[i].value, graph->nodes[i].nConnections);

		for (int j = 0; j < graph->nodes[i].nConnections; j++) {
			printf("%d, ", graph->nodes[i].connections[j]);
		}
		printf("\r\n");
	}
}

void DestroyGraph(Graph *graph) {
	//odalokuje prostor v pameti zabrany grafem
	if (graph->numNodes) {
		for (int i = 0; i < graph->numNodes; i++) {
			if (graph->nodes[i].nConnections) {
				free(graph->nodes[i].connections); //odalokuje pole odkazu pro kazdy prvek
				graph->nodes[i].connections = NULL;
			}
		}
		free(graph->nodes); //odalokuje pole prvku
		graph->nodes = NULL;
		graph->numNodes = 0;
	}
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

	if (intersects->numNodes == 0) { //nema spolecne body, nic se alokovat nebude
		intersects->nodes = NULL;
		return intersects;
	}

	intersects->nodes = malloc(numIntersects * sizeof(Node));

	//prohledam oba grafy znova a ulozim shodne prvky
	int currentIndex = 0;
	for (int i = 0; i < graphA->numNodes; i++) {
		for (int j = 0; j < graphB->numNodes; j++) {
			if (graphA->nodes[i].value == graphB->nodes[j].value) {
				//oba grafy obsahuji tento prvek, nakopirovat i propoje
				intersects->nodes[currentIndex].value = graphA->nodes[i].value;
				intersects->nodes[currentIndex].nConnections = graphA->nodes[i].nConnections;
				if (graphA->nodes[i].nConnections) {
					intersects->nodes[currentIndex].connections = malloc(graphA->nodes[i].nConnections * sizeof(int));

					for (int x = 0; x < intersects->nodes[currentIndex].nConnections; x++) {
						intersects->nodes[currentIndex].connections[x] = graphA->nodes[i].connections[x];
					}
				}
				else {
					intersects->nodes[currentIndex].connections = NULL;
				}

				currentIndex++;
				break;
			}
		}
	}

	return intersects;
}

Graph *GetNeighbors(Node *node) {
	Graph *neighbors = malloc(sizeof(Graph)); //alokace vysledku
	neighbors->numNodes = node->nConnections;

	if (neighbors->numNodes == 0) { //nema sousedy, nic se alokovat nebude
		neighbors->nodes = NULL;
		return neighbors;
	}

	neighbors->nodes = malloc(node->nConnections * sizeof(Node));

	for (int i = 0; i < node->nConnections; i++) {
		neighbors->nodes[i].nConnections = 0;
		neighbors->nodes[i].connections = NULL;
		neighbors->nodes[i].value = node->connections[i];
	}
	return neighbors;
}

Graph *Copy(Graph *toCopy) {
	Graph *copy = malloc(sizeof(Graph)); //alokace vysledku
	copy->numNodes = toCopy->numNodes;

	if (copy->numNodes == 0) { //nema prvky, nic se alokovat nebude
		copy->nodes = NULL;
		return copy;
	}
	copy->nodes = malloc((toCopy->numNodes) * sizeof(Node));

	//nakopiruju graf
	for (int i = 0; i < toCopy->numNodes; i++) {
		copy->nodes[i].value = toCopy->nodes[i].value;
		copy->nodes[i].nConnections = toCopy->nodes[i].nConnections;

		if (toCopy->nodes[i].nConnections) {
			copy->nodes[i].connections = malloc(toCopy->nodes[i].nConnections * sizeof(int));

			for (int j = 0; j < toCopy->nodes[i].nConnections; j++) {
				copy->nodes[i].connections[j] = toCopy->nodes[i].connections[j];
			}
		}
		else {
			copy->nodes[i].connections = NULL;
		}

	}

	return copy;
}

Graph *CopyAndAdd(Graph *toCopy, Node *toAdd) {
	Graph *copy = malloc(sizeof(Graph)); //alokace vysledku
	copy->numNodes = toCopy->numNodes + 1;
	copy->nodes = malloc((toCopy->numNodes + 1) * sizeof(Node));

	//nakopiruju graf
	for (int i = 0; i < toCopy->numNodes; i++) {
		copy->nodes[i].value = toCopy->nodes[i].value;
		copy->nodes[i].nConnections = toCopy->nodes[i].nConnections;

		if (toCopy->nodes[i].nConnections) {
			copy->nodes[i].connections = malloc(toCopy->nodes[i].nConnections * sizeof(int));

			for (int j = 0; j < toCopy->nodes[i].nConnections; j++) {
				copy->nodes[i].connections[j] = toCopy->nodes[i].connections[j];
			}
		}
		else {
			copy->nodes[i].connections = NULL;
		}
	}

	//a pridam jeden node
	copy->nodes[toCopy->numNodes].value = toAdd->value;
	copy->nodes[toCopy->numNodes].nConnections = toAdd->nConnections;

	if (toAdd->nConnections) {
		copy->nodes[toCopy->numNodes].connections = malloc(toAdd->nConnections * sizeof(int));

		for (int j = 0; j < toAdd->nConnections; j++) {
			copy->nodes[toCopy->numNodes].connections[j] = toAdd->connections[j];
		}
	}
	else {
		copy->nodes[toCopy->numNodes].connections = NULL;
	}

	return copy;
}

Graph *CreateEmptyGraph() {
	Graph *graph = malloc(sizeof(Graph)); //alokace vysledku
	graph->numNodes = 0;
	graph->nodes = NULL;
	return graph;
}

Graph *CopyAndRemove(Graph *toCopy, Node *toRemove) {
	Graph *copy = malloc(sizeof(Graph)); //alokace vysledku
	copy->numNodes = toCopy->numNodes - 1;

	if (toCopy->numNodes - 1 == 0) { //vysledny graf je prazdny
		copy->nodes = NULL;
		return copy;
	}

	copy->nodes = malloc((toCopy->numNodes - 1) * sizeof(Node));

	//nakopiruju graf
	int destIndex = 0;
	for (int i = 0; i < toCopy->numNodes; i++) {
		if (toCopy->nodes[i].value == toRemove->value) continue;

		copy->nodes[destIndex].value = toCopy->nodes[i].value;
		copy->nodes[destIndex].nConnections = toCopy->nodes[i].nConnections;

		if (toCopy->nodes[i].nConnections) {
			copy->nodes[destIndex].connections = malloc(toCopy->nodes[i].nConnections * sizeof(int));
			for (int j = 0; j < toCopy->nodes[i].nConnections; j++) {
				copy->nodes[destIndex].connections[j] = toCopy->nodes[i].connections[j];
			}
		}
		else {
			copy->nodes[destIndex].connections = NULL;
		}

		destIndex++;
	}

	return copy;
}

void BronKerbosch(Graph *r, Graph *p, Graph *x, Graph ***clicqueDestination, int *numClicques) {
	if (p->numNodes == 0 && x->numNodes == 0) {
		(*clicqueDestination) = realloc(*clicqueDestination, ((*numClicques) + 1) * sizeof(Graph*));
		(*clicqueDestination)[(*numClicques)++] = Copy(r);
		return;
	}

	Graph *pCopy = Copy(p);
	for (int i = 0; i < pCopy->numNodes; i++) {
		Node *vertex = &(pCopy->nodes[i]);

		Graph *r_new = CopyAndAdd(r, vertex);
		Graph *neighborsOfVertex = GetNeighbors(vertex);
		Graph *p_new = FindIntersects(p, neighborsOfVertex);
		Graph *x_new = FindIntersects(x, neighborsOfVertex);
		DestroyGraph(neighborsOfVertex);

		BronKerbosch(r_new, p_new, x_new, clicqueDestination, numClicques);
		DestroyGraph(r_new);
		DestroyGraph(p_new);
		DestroyGraph(x_new);

		Graph *p2 = CopyAndRemove(p, vertex);
		DestroyGraph(p);
		p = p2;

		Graph *x2 = CopyAndAdd(x, vertex);
		DestroyGraph(x);
		x = x2;
	}
	DestroyGraph(pCopy);
}

void FindCliques(Graph *source, Graph ***foundCliques, int *numFoundCliques) {
	//vyhleda nejvetsi kliky v grafu source a vrati je v poli foundCliques, jejich pocet ulozi do numFoundCliques
	//Bron-Kerbosch algorithm https://en.wikipedia.org/wiki/Bron%E2%80%93Kerbosch_algorithm?fbclid=IwAR1LpZZxsoxn0MeqUbiZahtBIuifaBAsSLxdx7oUbqiekWcsbgwWxds-wQU

	Graph *r_start = CreateEmptyGraph();
	Graph *x_start = CreateEmptyGraph();
	BronKerbosch(r_start, source, x_start, foundCliques, numFoundCliques);

	DestroyGraph(r_start);
	free(r_start);
	DestroyGraph(x_start);
	free(x_start);
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

	printf("Nacteny graf ze souboru:\r\n");
	PrintGraph(graph);
	printf("\r\n\r\n");


	////////////////////////////////////// HLEDANI KLIK /////////////////////////////////////////////////
	Graph **clicques = NULL;  //pole grafu, ktere budou obsahovat uzly nacteneho grafu, ktere jsou soucasti klik
	int numClicques = 0;  //pocet klik

	FindCliques(graph, &clicques, &numClicques);

	//zjisteni nejvetsi kliky
	int maxClicque = 0;
	for (int i = 0; i < numClicques; i++) {
		if (clicques[i]->numNodes > maxClicque) {
			maxClicque = clicques[i]->numNodes;
		}
	}


	//vypis nejvetsi kliky (nebo klik)
	printf("\r\nGraf obsahuje celkem %d klik, nejvetsi z nich ma velikost %d:\r\n", numClicques, maxClicque);
	for (int i = 0; i < numClicques; i++) {
		if (clicques[i]->numNodes == maxClicque) {
			printf("Nejvetsi klika:\r\n");
			PrintGraph(clicques[i]);
		}
	}

	////////////////////////////////////// ODALOKACE, KONEC ///////////////////////////////////////////////
	DestroyGraph(graph); //odalokuju pamet grafu nacteneho ze souboru
	free(graph); //odalokuje strukturu
	graph = NULL;

	for (int i = 0; i < numClicques; i++) { //odalokuju jednotlive grafy (kliky)
		DestroyGraph(clicques[i]);
	}
	free(clicques); //odalokuju pole grafu
	clicques = NULL;
	return 0;
}
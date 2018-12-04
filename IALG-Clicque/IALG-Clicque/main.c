/**
 * @file main.c
 * @author Jiří Šrámek, Matěj Kroulík, Tomáš Macko, Jiří Bekr
 * @version 1.1
 * @date 2018-12-04
 * @copyright Copyright (c) 2018
 * @note Zpracováno v rámci předmětu IALG, projekt č.8: Největší klika v neorientovaném grafu
 * 
 * @brief Tento soubor implementuje Bron-Kerbosch algoritmus pro hledání klik v grafu
 * 
 * Zadání projektu:
 *  Klika grafu je podgraf, který je úplným grafem. 
 *  Kterýkoliv vrchol kliky je tedy spojen hranou se všemi ostatními vrcholy kliky. 
 *  Vytvořte program pro hledání největší kliky v neorientovaném grafu. Pokud existuje více řešení,
 *  nalezněte všechna. Výsledky prezentujte vhodným způsobem. Součástí projektu bude načítání grafů
 *  ze souboru a vhodné testovací grafy. V dokumentaci uveďte teoretickou složitost úlohy a porovnejte ji
 *  s experimentálními výsledky.
 * 
 */

#define _CRT_SECURE_NO_WARNINGS ///< scanf, printf warning ve VS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "check.h" //pro kontrolu dynamické paměti

#define MIN_ALOKACE 10 ///< minimální velikost alokace při načítání grafu


/**
 * @brief Struktura popisujíjí obsah každého uzlu grafu
 * 
 */
typedef struct Node {
	int value; ///< Hodnota uzlu
	int nConnections; ///< Počet propojů
	int *connections; ///< Pole hodnot uzlů, na které jsou propoje (seznam propojů)
}Node;

/**
 * @brief Struktura popisující graf
 * 
 */
typedef struct Graph {
	Node *nodes; ///< Pole uzlů grafu
	int numNodes; ///< Počet uzlů v grafu
}Graph;

int numIterations = 0; ///< Počet iterací hlavního algoritmu

/**
 * @brief Funkce ošetřující chybu při načítání grafu ze souboru
 * 
 */
void ErrorLoad(){
printf("Nespravny format grafu");
exit(2);
}

/**
 * @brief Funkce ošetřující chybu při alokaci paměti
 * 
 */
void ErrorMalloc() {
	printf("Malloc: chyba alokace pametoveho prostoru\n");
	printf("Program nahlasil kritickou chybu a bude ukoncen");
	exit(3);
}

/**
 * @brief Načítá graf ze souboru
 * 
 * @param f 		Soubor pro načtení
 * @return Graph* 	Výsledný graf
 */
Graph* LoadGraphFromFile(FILE *f) {
	//alokace pole node podle delky nacitaneho grafu
	//ulozeni delky grafu do numNodes
	int c;
	int numNodes = 0; //pocet prvku
	int *numConnections; //pocet propoju mezi uzly 
	int alloccated = MIN_ALOKACE;
	Graph* graph;

	numConnections = calloc(MIN_ALOKACE, sizeof(int));
	if (numConnections == NULL) ErrorMalloc();
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
				if (numConnections == NULL) ErrorMalloc();
				memset(&(numConnections[alloccated - MIN_ALOKACE]), 0, MIN_ALOKACE * sizeof(int));
			}
		}
		if (c == ',')numConnections[numNodes - 1]++; //zjistovani poctu propoju pro dany uzel 


	} while (c != EOF);
	graph = malloc(sizeof(Graph));// alokace grafu
	if (graph == NULL) ErrorMalloc();
	graph->numNodes = numNodes;
	graph->nodes = malloc(numNodes * sizeof(Node));
	if (graph->nodes == NULL) ErrorMalloc();
	for (int i = 0; i < numNodes; i++)
	{
		graph->nodes[i].nConnections = numConnections[i] + 1;
		graph->nodes[i].connections = malloc(graph->nodes[i].nConnections * sizeof(int));
		if (graph->nodes[i].connections == NULL) ErrorMalloc();
	}
	free(numConnections);
	numConnections = NULL;

	rewind(f);
	//Prochazeni souboru a naplneni jiz naalokovaneho prostoru
	for (int i = 0; i < graph->numNodes; i++) {
		int read = fscanf(f, "%d:", &(graph->nodes[i].value));
		if (read < 1){
			ErrorLoad();
		}

		for (int j = 0; j < graph->nodes[i].nConnections; j++) {
			int readNodeId;
			int read;
			if (j < graph->nodes[i].nConnections - 1) read = fscanf(f, "%d,", &readNodeId);
			else read = fscanf(f, "%d", &readNodeId);
			if (read < 1) {
				ErrorLoad();
			}
			graph->nodes[i].connections[j] = readNodeId;
		}
	}
	return graph;
}

/**
 * @brief Tiskne graf včetně všech uzlů a propojů
 * 
 * @param graph 	Graf pro vytisknutí
 */
void PrintGraph(Graph *graph) {
	printf("Graf s %d uzly\r\n", graph->numNodes);
	for (int i = 0; i < graph->numNodes; i++) {
		printf(" Uzel s hodnotou %d, %d spoju: ", graph->nodes[i].value, graph->nodes[i].nConnections);

		for (int j = 0; j < graph->nodes[i].nConnections; j++) {
			printf("%d, ", graph->nodes[i].connections[j]);
		}
		printf("\r\n");
	}
}

/**
 * @brief Odstraní veškerou dynamickou paměť zabranou grafem
 * 
 * @param graph 	Graf pro smazání
 */
void DestroyGraph(Graph **graph) {
	if (*graph == NULL) return;
	if ((*graph)->numNodes) {
		for (int i = 0; i < (*graph)->numNodes; i++) {
			if ((*graph)->nodes[i].nConnections) {
				free((*graph)->nodes[i].connections); //odalokuje pole odkazu pro kazdy prvek
				(*graph)->nodes[i].connections = NULL;
			}
		}
		free((*graph)->nodes); //odalokuje pole prvku
		(*graph)->nodes = NULL;
		(*graph)->numNodes = 0;

		
	}
	free(*graph);
	*graph = NULL;
}

/**
 * @brief Spočítá průnik prvků dvou grafů
 * 
 * @param graphA 	První graf pro prohledání
 * @param graphB 	Druhý graf pro prohledání
 * @return Graph* 	Výsledný graf, průnik \c graphA a \c graphB
 */
Graph *FindIntersects(Graph *graphA, Graph *graphB) {
	if (graphA == NULL || graphB == NULL) return NULL;
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

	//Teď už vím, kolik bude prvků, mohu provést alokaci
	Graph *intersects = malloc(sizeof(Graph)); //Alokace grafu
	if (intersects == NULL) ErrorMalloc();
	intersects->numNodes = numIntersects;

	if (intersects->numNodes == 0) { //Grafy nemají společné body
		intersects->nodes = NULL;
		return intersects;
	}
	intersects->nodes = malloc(numIntersects * sizeof(Node));
	if (intersects->nodes == NULL) ErrorMalloc();

	//Prohledám znovu grafy a zkopíruji společné prvky
	int currentIndex = 0;
	for (int i = 0; i < graphA->numNodes; i++) {
		for (int j = 0; j < graphB->numNodes; j++) {
			if (graphA->nodes[i].value == graphB->nodes[j].value) {
				//Oba grafy obsahují tento prvek, nakopíruji i propoje
				intersects->nodes[currentIndex].value = graphA->nodes[i].value;
				intersects->nodes[currentIndex].nConnections = graphA->nodes[i].nConnections;
				if (graphA->nodes[i].nConnections) {
					intersects->nodes[currentIndex].connections = malloc(graphA->nodes[i].nConnections * sizeof(int));
					if (intersects->nodes[currentIndex].connections == NULL) ErrorMalloc();

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

/**
 * @brief Vyhledá sousední uzly zadaného uzlu a vrátí je jako seznam uzlů (graf)
 * 
 * @param node 		Uzel grafu, pro který hledat
 * @return Graph* 	Výsledný seznam sousedů
 */
Graph *GetNeighbors(Node *node) {
	if (node == NULL) return NULL;

	Graph *neighbors = malloc(sizeof(Graph)); //Alokace výsledku
	if (neighbors == NULL) ErrorMalloc();
	neighbors->numNodes = node->nConnections;

	if (neighbors->numNodes == 0) { //Uzel nemá sousedy, netřeba alokovat
		neighbors->nodes = NULL;
		return neighbors;
	}

	neighbors->nodes = malloc(node->nConnections * sizeof(Node));
	if (neighbors->nodes == NULL) ErrorMalloc();

	for (int i = 0; i < node->nConnections; i++) {
		neighbors->nodes[i].nConnections = 0;
		neighbors->nodes[i].connections = NULL;
		neighbors->nodes[i].value = node->connections[i];
	}
	return neighbors;
}

/**
 * @brief Vytvoří kopii grafu
 * 
 * @param toCopy 	Graf, který se má zkopírovat
 * @return Graph* 	Kopie vstupního grafu
 */
Graph *Copy(Graph *toCopy) {
	if (toCopy == NULL) return NULL;

	Graph *copy = malloc(sizeof(Graph)); //Alokace výsledku
	if (copy == NULL) ErrorMalloc();
	copy->numNodes = toCopy->numNodes;

	if (copy->numNodes == 0) { //Nemá prvky, netřeba alokovat
		copy->nodes = NULL;
		return copy;
	}
	copy->nodes = malloc((toCopy->numNodes) * sizeof(Node));
	if (copy->nodes == NULL) ErrorMalloc();

	//Nakopíruji graf
	for (int i = 0; i < toCopy->numNodes; i++) {
		copy->nodes[i].value = toCopy->nodes[i].value;
		copy->nodes[i].nConnections = toCopy->nodes[i].nConnections;

		if (toCopy->nodes[i].nConnections) {
			copy->nodes[i].connections = malloc(toCopy->nodes[i].nConnections * sizeof(int));
			if (copy->nodes[i].connections == NULL) ErrorMalloc();

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

/**
 * @brief Vytvoří kopii grafu a přidá k ní uzel
 * 
 * @param toCopy 	Graf, který se má kopírovat
 * @param toAdd 	Uzel pro přidání
 * @return Graph* 	Zkopírovaný graf s přidaným uzlem
 */
Graph *CopyAndAdd(Graph *toCopy, Node *toAdd) {
	if (toCopy == NULL || toAdd == NULL) return NULL;

	Graph *copy = malloc(sizeof(Graph)); //alokace vysledku
	if (copy == NULL) ErrorMalloc();
	copy->numNodes = toCopy->numNodes + 1;
	copy->nodes = malloc((toCopy->numNodes + 1) * sizeof(Node));
	if (copy->nodes == NULL) ErrorMalloc();

	//Nakopíruji graf...
	for (int i = 0; i < toCopy->numNodes; i++) {
		copy->nodes[i].value = toCopy->nodes[i].value;
		copy->nodes[i].nConnections = toCopy->nodes[i].nConnections;

		if (toCopy->nodes[i].nConnections) {
			copy->nodes[i].connections = malloc(toCopy->nodes[i].nConnections * sizeof(int));
			if (copy->nodes[i].connections == NULL) ErrorMalloc();

			for (int j = 0; j < toCopy->nodes[i].nConnections; j++) {
				copy->nodes[i].connections[j] = toCopy->nodes[i].connections[j];
			}
		}
		else {
			copy->nodes[i].connections = NULL;
		}
	}

	//...A přidám uzel
	copy->nodes[toCopy->numNodes].value = toAdd->value;
	copy->nodes[toCopy->numNodes].nConnections = toAdd->nConnections;

	if (toAdd->nConnections) {
		copy->nodes[toCopy->numNodes].connections = malloc(toAdd->nConnections * sizeof(int));
		if (copy->nodes[toCopy->numNodes].connections == NULL) ErrorMalloc();

		for (int j = 0; j < toAdd->nConnections; j++) {
			copy->nodes[toCopy->numNodes].connections[j] = toAdd->connections[j];
		}
	}
	else {
		copy->nodes[toCopy->numNodes].connections = NULL;
	}
	return copy;
}

/**
 * @brief Vytvoří prázdný graf
 * 
 * @return Graph* 	Prázdný graf
 * 
 * @note Prázdným grafem se rozumí graf, který nemá žádné uzly
 */
Graph *CreateEmptyGraph() {
	Graph *graph = malloc(sizeof(Graph)); //alokace vysledku
	if (graph == NULL) ErrorMalloc();
	graph->numNodes = 0;
	graph->nodes = NULL;
	return graph;
}

/**
 * @brief Odstraní uzel z grafu
 * 
 * @param source 	Zdrojový graf
 * @param toRemove 	Uzel, který se má odstranit
 */
void RemoveFromGraph(Graph *source, Node *toRemove) {
	//Projdu celý graf...
	int destIndex = 0;
	for (int i = 0; i < source->numNodes; i++) {
		if (source->nodes[i].value == toRemove->value) { //Pokud najdu prvek, který mám odstranit,
			if (source->nodes[i].nConnections) {
				free(source->nodes[i].connections); //tak ho odstraním a uvolním po něm paměť
				source->nodes[i].connections = NULL;
				source->nodes[i].nConnections = 0;
			}
			continue;
		}
		//Jinak uzel i jeho propoje přesunu
		source->nodes[destIndex].value = source->nodes[i].value;
		source->nodes[destIndex].nConnections = source->nodes[i].nConnections;
		source->nodes[destIndex].connections = source->nodes[i].connections;
		destIndex++;
	}
	source->numNodes = destIndex;
	source->nodes = realloc(source->nodes, source->numNodes * sizeof(Node)); //Přealokuji graf po odstranění uzlu
	if ((source->numNodes !=0) && (source->nodes == NULL)) ErrorMalloc();
}

/**
 * @brief Přidá uzel do grafu
 * 
 * @param source 	Zdrojový grag
 * @param toAdd 	Uzel, který se má přidat
 */
void AddToGraph(Graph *source, Node *toAdd) {
	//nakopiruju graf
	source->numNodes++;
	source->nodes = realloc(source->nodes, source->numNodes * sizeof(Node));
	if (source->nodes == NULL) ErrorMalloc();

	//a pridam jeden node
	source->nodes[source->numNodes-1].value = toAdd->value;
	source->nodes[source->numNodes - 1].nConnections = toAdd->nConnections;

	if (toAdd->nConnections) {
		source->nodes[source->numNodes - 1].connections = malloc(toAdd->nConnections * sizeof(int));
		if (source->nodes[source->numNodes - 1].connections == NULL) ErrorMalloc();

		for (int i = 0; i < toAdd->nConnections; i++) {
			source->nodes[source->numNodes - 1].connections[i] = toAdd->connections[i];
		}
	}
	else {
		source->nodes[source->numNodes - 1].connections = NULL;
	}
}

/**
 * @brief Algoritmus Bron-Kerbosch pro hledání klik v grafu
 * 
 * @param r 						Nalezená klika
 * @param p 						Graf, ve kterém hledat
 * @param x 						Seznam vyloučených uzlů
 * @param clicqueDestination 		Výstup algoritmu, pole nalezených klik
 * @param numClicques 				Výstup algoritmu, počet nalezených klik
 * 
 * @note Bron-Kerbosch algoritmus: https://en.wikipedia.org/wiki/Bron%E2%80%93Kerbosch_algorithm
 */
void BronKerbosch(Graph *r, Graph *p, Graph *x, Graph ***clicqueDestination, int *numClicques) {
	numIterations++;
	if (p->numNodes == 0 && x->numNodes == 0) {
		(*clicqueDestination) = realloc(*clicqueDestination, ((*numClicques) + 1) * sizeof(Graph*));
		if (clicqueDestination == NULL) ErrorMalloc();
		(*clicqueDestination)[(*numClicques)++] = Copy(r);
		return;
	}

	Graph *pCopy = Copy(p);
	for (int i = 0; i < pCopy->numNodes; i++) {
		Node *vertex = &(pCopy->nodes[i]);

		Graph *r_new = CopyAndAdd(r, vertex);//////////////////////////////////////////////////
		Graph *neighborsOfVertex = GetNeighbors(vertex);
		Graph *p_new = FindIntersects(p, neighborsOfVertex);
		Graph *x_new = FindIntersects(x, neighborsOfVertex);
		DestroyGraph(&neighborsOfVertex);
		
		BronKerbosch(r_new, p_new, x_new, clicqueDestination, numClicques);
		DestroyGraph(&r_new);
		DestroyGraph(&p_new);
		DestroyGraph(&x_new);

		RemoveFromGraph(p, vertex);
		AddToGraph(x, vertex);
	}

	DestroyGraph(&pCopy);
}

/**
 * @brief Pomocná funkce pro volání Bron-Kerbosch algoritmu
 * 
 * @param source 			Graf, ve kterém hledat kliky
 * @param foundCliques 		Nalezené kliky
 * @param numFoundCliques 	Počet nalezených klik
 */
void FindCliques(Graph *source, Graph ***foundCliques, int *numFoundCliques) {
	Graph *r_start = CreateEmptyGraph();
	Graph *x_start = CreateEmptyGraph();
	BronKerbosch(r_start, source, x_start, foundCliques, numFoundCliques);

	DestroyGraph(&r_start);
	DestroyGraph(&x_start);
}

/**
 * @brief Hlavní funkce programu
 * 
 * @param argc 	Počet argumentů
 * @param argv 	Argumenty příkazové řádky
 * @return int 	Návratová hodnota programu
 */
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
	DestroyGraph(&graph); //odalokuju pamet grafu nacteneho ze souboru
	graph = NULL;

	for (int i = 0; i < numClicques; i++) { //odalokuju jednotlive grafy (kliky)
		DestroyGraph(&(clicques[i]));
	}
	free(clicques); //odalokuju pole grafu
	clicques = NULL;

	printf("%d iteraci B-k\r\n", numIterations);
	return 0;
}
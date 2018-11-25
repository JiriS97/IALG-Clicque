#include <stdio.h>

typedef struct {
	int value;
	int nConnections;
	Node **connections;
}Node;

typedef struct {
	Node *nodes;
	int numNodes;
}Graph;

void LoadGraphFromFile(FILE *f, Graph *graph) {
	//alokace pole node podle delky nacitaneho grafu
	//ulozeni delky grafu do numNodes

}

void PrintGraph(Graph *graph) {
	//vytiskne graf jako seznam uzluuu
	for (int i = 0; i < graph->numNodes; i++) {
		printf("Uzel s hodnotou %d\r\n", graph->nodes[i]);
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
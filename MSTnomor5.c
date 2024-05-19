#include <stdio.h>
#include <stdlib.h>

#define MAX 100

typedef struct {
    int awal, akhir, bobot;
} Edge;

typedef struct {
    Edge *edges;
    int JumEdge;
    int JumVertice;
} Graph;

typedef struct {
    int parent;
    int rank;
} Subset;

int find(Subset subsets[], int i) {
    if (subsets[i].parent != i)
        subsets[i].parent = find(subsets, subsets[i].parent);
    return subsets[i].parent;
}

void unionSubsets(Subset subsets[], int x, int y) {
    int rootX = find(subsets, x);
    int rootY = find(subsets, y);

    if (subsets[rootX].rank < subsets[rootY].rank)
        subsets[rootX].parent = rootY;
    else if (subsets[rootX].rank > subsets[rootY].rank)
        subsets[rootY].parent = rootX;
    else {
        subsets[rootY].parent = rootX;
        subsets[rootX].rank++;
    }
}

int banding_Edge(const void *a, const void *b) {
    Edge *edgeA = (Edge *)a;
    Edge *edgeB = (Edge *)b;
    return edgeA->bobot - edgeB->bobot;
}

void KruskalMST(Graph *graph) {
    int V = graph->JumEdge;
    Edge result[MAX];
    int e = 0;
    int i = 0;

    qsort(graph->edges, graph->JumEdge, sizeof(graph->edges[0]), banding_Edge);

    Subset *subsets = (Subset *)malloc(V * sizeof(Subset));
    for (int v = 0; v < V; ++v) {
        subsets[v].parent = v;
        subsets[v].rank = 0;
    }

    while (e < V - 1 && i < graph->JumEdge) {
        Edge nextEdge = graph->edges[i++];

        int x = find(subsets, nextEdge.awal);
        int y = find(subsets, nextEdge.akhir);

        if (x != y) {
            result[e++] = nextEdge;
            unionSubsets(subsets, x, y);
        }
    }

    printf("Hasil MST Kruskal \n");
    for (i = 0; i < e; ++i)
        printf("%d ---->  %d == %d\n", result[i].awal, result[i].akhir, result[i].bobot);

    free(subsets);
}

int main() {
    int JumEdge, JumVertice;

    printf("Total Vertices: ");
    scanf("%d", &JumVertice);

    printf("Total Edge: ");
    scanf("%d", &JumEdge);

    Graph *graph = (Graph *)malloc(sizeof(Graph));
    graph->JumEdge = JumEdge;
    graph->JumVertice = JumVertice;
    graph->edges = (Edge *)malloc(JumEdge * sizeof(Edge));

    printf("Masuukan edge awal, edge akhir, dan bobot:\n");
    for (int i = 0; i < JumEdge; i++) {
        printf("Edge %d: ", i + 1);
        scanf("%d %d %d", &graph->edges[i].awal, &graph->edges[i].akhir, &graph->edges[i].bobot);
    }

    KruskalMST(graph);

    free(graph->edges);
    free(graph);

    return 0;
}

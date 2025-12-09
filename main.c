#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#define V 35

// GRAPH STRUCTURES IMPLEMENTATION
typedef struct AdjListNode
{
    int vertex;
    struct AdjListNode *next;
    int weight;
} AdjListNode;

typedef struct Graph
{
    AdjListNode *AdjLists[V];
    int numV;
} Graph;

// GRAPH FUNCS
AdjListNode *newAdjNode(int v, int weight)
{
    AdjListNode *newNode = (AdjListNode *)malloc(sizeof(AdjListNode));
    if (newNode == NULL)
        return NULL;
    newNode->vertex = v;
    newNode->next = NULL;
    newNode->weight = weight;
    return newNode;
}

Graph *newGraph(int vertices)
{
    Graph *graph = (Graph *)malloc(sizeof(Graph));
    if (graph == NULL)
        return NULL;

    graph->numV = vertices - 1;
    for (int i = 0; i < vertices; i++)
    {
        graph->AdjLists[i] = NULL;
    }

    return graph;
}

void freeGraph(Graph *graph)
{
    if (graph == NULL)
        return;

    for (int i = 1; i <= graph->numV; i++)
    {
        AdjListNode *current = graph->AdjLists[i];
        while (current != NULL)
        {
            AdjListNode *next = current->next;
            free(current);
            current = next;
        }
    }
    free(graph);
}

void addEdge(Graph *graph, int source, int dest, int weight)
{
    if (weight == 0)
        return;
    if (source == dest)
    {
        AdjListNode *temp = graph->AdjLists[source];

        while (temp != NULL)
        {
            if (temp->vertex == source)
            {
                temp->weight += weight;
                break;
            }
            temp = temp->next;
        }
        AdjListNode *newNode = newAdjNode(dest, weight);
        newNode->next = graph->AdjLists[source];
        graph->AdjLists[source] = newNode;
        return;
    }

    // source -> dest
    AdjListNode *temp = graph->AdjLists[source];
    while (temp != NULL)
    {
        if (temp->vertex == dest)
        {
            temp->weight += weight;
            break;
        }
        temp = temp->next;
    }

    if (temp == NULL)
    {
        AdjListNode *newNode = newAdjNode(dest, weight);
        newNode->next = graph->AdjLists[source];
        graph->AdjLists[source] = newNode;
    }

    // dest -> source
    temp = graph->AdjLists[dest];
    while (temp != NULL)
    {
        if (temp->vertex == source)
        {
            temp->weight += weight;
            break;
        }
        temp = temp->next;
    }

    AdjListNode *newNode = newAdjNode(source, weight);
    newNode->next = graph->AdjLists[dest];
    graph->AdjLists[dest] = newNode;
}

void printGraph(Graph *graph)
{
    for (int v = 1; v <= graph->numV; v++)
    {
        printf("%d", v);
        AdjListNode *node = graph->AdjLists[v];
        while (node != NULL)
        {
            printf(" -%d-> %d", node->weight, node->vertex);
            node = node->next;
        }
        printf("\n");
    }
}

// MODULARITY FUNCS
int nodeDegree(Graph *g, int v)
{
    int sum = 0;
    AdjListNode *temp = g->AdjLists[v];
    while (temp != NULL)
    {
        sum += temp->weight;
        temp = temp->next;
    }
    return sum;
}

int edgeCount(Graph *graph)
{
    int m = 0;
    for (int i = 1; i <= graph->numV; i++)
    {
        AdjListNode *temp = graph->AdjLists[i];
        while (temp != NULL)
        {
            m += 1;
            temp = temp->next;
        }
    }

    return (m / 2);
}

int isConnected(Graph *graph, int vertex1, int vertex2)
{
    AdjListNode *temp = graph->AdjLists[vertex1];
    while (temp != NULL)
    {
        if (temp->vertex == vertex2)
            return temp->weight;
        temp = temp->next;
    }
    return 0;
}

double internalCommunityWeight(Graph *graph, int *communities, int targetCom)
{
    double sum = 0;
    for (int v = 1; v <= graph->numV; v++)
    {
        if (communities[v] == targetCom)
        {
            AdjListNode *temp = graph->AdjLists[v];
            while (temp != NULL)
            {
                if (communities[temp->vertex] == targetCom)
                {
                    sum += temp->weight;
                }
                temp = temp->next;
            }
        }
    }
    return sum / 2.0;
}

double totalGraphWeight(Graph *g)
{
    double sum = 0;
    for (int i = 1; i <= g->numV; i++)
    {
        AdjListNode *temp = g->AdjLists[i];
        while (temp != NULL)
        {
            sum += temp->weight;
            temp = temp->next;
        }
    }
    return sum / 2.0; // divide by 2 because undirected
}

double modularity(Graph *graph, int *communities)
{
    double Q = 0.0;
    int m = totalGraphWeight(graph);

    for (int i = 1; i <= graph->numV; i++)
    {
        for (int j = 1; j <= graph->numV; j++)
        {

            if (communities[i] == communities[j])
            {
                Q += isConnected(graph, i, j) - (nodeDegree(graph, i) * nodeDegree(graph, j)) / (2.0 * m);
            }
        }
    }
    Q /= (2.0 * m);
    return Q;
}

// OTHER FUNCS
int isInside(int x, int *arr, int size)
{
    for (int i = 0; i < size; i++)
    {
        if (arr[i] == x)
            return 1;
    }
    return 0;
}

void addToArray(int x, int *arr, int size)
{
    int i = 0;
    while (i < size && arr[i] != 0)
    {
        i++;
    }
    if (i >= size)
        return;
    arr[i] = x;
}

int communityCount(Graph *graph, int *communities)
{
    int counter = 0;
    int comList[graph->numV];
    for (int i = 0; i < graph->numV; i++)
    {
        comList[i] = 0;
    }

    for (int i = 1; i <= graph->numV; i++)
    {
        if (isInside(communities[i], comList, graph->numV) == 0)
        {
            counter++;
            addToArray(communities[i], comList, graph->numV);
        }
    }
    return counter;
}

// PARTITION ALGORITHMS
double computeKi_in(Graph *g, int i, int targetCom, int *communities)
{
    double sum = 0.0;
    AdjListNode *temp = g->AdjLists[i];
    while (temp != NULL)
    {
        if (communities[temp->vertex] == targetCom)
            sum += temp->weight;

        temp = temp->next;
    }
    return sum;
}

double communityWeight(Graph *g, int *communities, int targetCom)
{
    double sum = 0;

    for (int v = 1; v <= g->numV; v++)
    {
        if (communities[v] == targetCom)
            sum += 1.0 * nodeDegree(g, v);
    }

    return sum;
}

double deltaQ(Graph *g, int *communities, int vertex, int targetCom)
{
    double m2 = 2.0 * totalGraphWeight(g);
    double ki = nodeDegree(g, vertex);

    int oldC = communities[vertex];
    communities[vertex] = 0;

    double ki_in_target = computeKi_in(g, vertex, targetCom, communities);
    double tot_target = communityWeight(g, communities, targetCom);

    double ki_in_old = computeKi_in(g, vertex, oldC, communities);
    double tot_old = communityWeight(g, communities, oldC);

    communities[vertex] = oldC;

    double dQ = ((ki_in_target / m2) - ((ki * tot_target) / (m2 * m2))) - ((ki_in_old / m2) - ((ki * tot_old) / (m2 * m2)));

    return dQ;
}
//Louvain
int louvainPhase1(Graph *graph, int *communities)
{
    int ever_improved = 0;
    for (int i = 0; i <= graph->numV; i++)
    {
        communities[i] = i;
    }

    int improved = 1;
    while (improved)
    {
        improved = 0;

        for (int i = 1; i <= graph->numV; i++)
        {
            double bestGain = 0.0;
            int bestC = communities[i];
            AdjListNode *temp = graph->AdjLists[i];
            while (temp != NULL)
            {
                double gain = deltaQ(graph, communities, i, communities[temp->vertex]);
                if (gain > bestGain)
                {
                    bestGain = gain;
                    bestC = communities[temp->vertex];
                }
                temp = temp->next;
            }

            if (bestC != communities[i])
            {
                improved = 1;
                ever_improved = 1;
                communities[i] = bestC;
            }
        }
    }
    return ever_improved;
}

Graph *louvainAggregate(Graph *graph, int *communities)
{

    int *mapOldToNew = (int *)calloc(graph->numV + 1, sizeof(int)); // CommunityID-> CommunityNodeID eg. 22->1 17->2 etc.
    int *mapNewToOLd = (int *)calloc(graph->numV + 1, sizeof(int)); // CommunityNodeID -> CommunityID eg. 1->22 2->17 etc.
    int numCom = 1;
    for (int v = 1; v <= graph->numV; v++)
    {
        if (mapOldToNew[communities[v]] == 0)
        {
            mapOldToNew[communities[v]] = numCom;
            mapNewToOLd[numCom] = communities[v];
            numCom++;
        }
    }

    Graph *superGraph = newGraph(communityCount(graph, communities) + 1);

    // Adding self loops of each community
    for (int cv = 1; cv <= superGraph->numV; cv++)
    {
        int weight = 0;
        int c = mapNewToOLd[cv];
        for (int v = 1; v <= graph->numV; v++)
        {
            if (communities[v] == c)
            {
                AdjListNode *temp = graph->AdjLists[v];
                while (temp)
                {
                    if (communities[temp->vertex] == c)
                    {
                        weight += temp->weight;
                    }
                    temp = temp->next;
                }
            }
        }

        addEdge(superGraph, cv, cv, weight / 2);
    }

    // Adding edges between communities
    for (int cn = 1; cn <= superGraph->numV; cn++)
    {
        int co = mapNewToOLd[cn];
        int *neighbourWeights = (int *)calloc(superGraph->numV + 1, sizeof(int));
        for (int vertex = 1; vertex <= graph->numV; vertex++)
        {
            if (communities[vertex] != co)
                continue;
            AdjListNode *temp = graph->AdjLists[vertex];
            while (temp)
            {
                int targetComOld = communities[temp->vertex];
                int targetComNew = mapOldToNew[targetComOld];
                neighbourWeights[targetComNew] += temp->weight;
                temp = temp->next;
            }
        }
        for (int i = 1; i <= superGraph->numV; i++)
        {
            addEdge(superGraph, cn, i, neighbourWeights[i] / 2);
        }

        free(neighbourWeights);
    }

    free(mapOldToNew);
    free(mapNewToOLd);
    return superGraph;
}

int louvainFull(Graph *graph, int *communities)
{
    int improved = 1;
    Graph *currentGraph = graph;
    int *currentCommunities = communities;

    while (improved)
    {
        improved = louvainPhase1(currentGraph, currentCommunities);
        // Community Mapping
        int *mapOldToNew = (int *)calloc(graph->numV + 1, sizeof(int)); // CommunityID-> CommunityNodeID eg. 22->1 17->2 etc.
        int *mapNewToOLd = (int *)calloc(graph->numV + 1, sizeof(int)); // CommunityNodeID -> CommunityID eg. 1->22 2->17 etc.
        int numCom = 1;
        for (int v = 1; v <= currentGraph->numV; v++)
        {
            if (mapOldToNew[currentCommunities[v]] == 0)
            {
                mapOldToNew[currentCommunities[v]] = numCom;
                mapNewToOLd[numCom] = currentCommunities[v];
                numCom++;
            }
        }
        Graph *superGraph = louvainAggregate(currentGraph, currentCommunities);
        int *superCommunities = (int *)calloc(superGraph->numV + 1, sizeof(int));
        improved = louvainPhase1(superGraph, superCommunities);
        for (int vertex = 1; vertex <= currentGraph->numV; vertex++)
        {
            currentCommunities[vertex] = superCommunities[mapOldToNew[currentCommunities[vertex]]];
        }
        currentCommunities = superCommunities;
        currentGraph = superGraph;
        free(mapNewToOLd);
        free(mapOldToNew);
    }
    free(currentCommunities);
    free(currentGraph);
    return 0;
}
//Fast Greedy
int fastGreedy(Graph *graph, int *communities)
{
    int n = graph->numV;
    for (int i = 1; i <= n; i++)
        communities[i] = i;

    int changed = 1;
    double currentQ = modularity(graph, communities);

    while (changed)
    {
        changed = 0;
        double bestDelta = 0.0;
        int mergeA = -1, mergeB = -1;

        int comCount = communityCount(graph, communities);
        int comList[comCount];
        for (int i = 0; i < comCount; i++)
            comList[i] = 0;

        for (int i = 1; i <= n; i++)
            if (!isInside(communities[i], comList, comCount))
                addToArray(communities[i], comList, comCount);

        for (int i = 0; i < comCount; i++)
        {
            for (int j = i + 1; j < comCount; j++)
            {
                int c1 = comList[i];
                int c2 = comList[j];

                int backup[n + 1];
                for (int k = 1; k <= n; k++)
                    backup[k] = communities[k];
                for (int k = 1; k <= n; k++)
                    if (communities[k] == c1)
                        communities[k] = c2;
                double newQ = modularity(graph, communities);
                double deltaQ = newQ - currentQ;

                for (int k = 1; k <= n; k++)
                    communities[k] = backup[k];

                if (deltaQ > bestDelta)
                {
                    bestDelta = deltaQ;
                    mergeA = c1;
                    mergeB = c2;
                }
            }
        }

        if (bestDelta > 0)
        {
            for (int k = 1; k <= n; k++)
                if (communities[k] == mergeA)
                    communities[k] = mergeB;

            currentQ += bestDelta;
            changed = 1;
        }
    }
    return 0;
}
//My Algorithms
int commonNeighbours(Graph *graph, int v1, int v2)
{
    int count = 0;

    AdjListNode *pv1 = graph->AdjLists[v1];
    while (pv1)
    {
        AdjListNode *pv2 = graph->AdjLists[v2];
        while (pv2)
        {
            if (pv1->vertex == pv2->vertex)
                count++;
            pv2 = pv2->next;
        }
        pv1 = pv1->next;
    }
    return count;
}

int areStrong(Graph *graph, int v, int u, double alpha)
{
    int cn = commonNeighbours(graph, v, u);
    int dv = nodeDegree(graph, v);
    int du = nodeDegree(graph, u);

    int unionSize = dv + du - cn;
    if (unionSize == 0) return 0;

    double jaccard = (double)cn / unionSize;

    return (jaccard > alpha);
}

int findParent(int x, int* parent) {
        while (parent[x] != x) x = parent[x];
        return x;
    }

void join(int* parent, int newParent, int a) {
        a = findParent(a, parent);
        newParent = findParent(newParent, parent);
        if (a != newParent) parent[a] = newParent;
    }

int SCP(Graph *graph, int *communities)
{
    int parent[V];
    double alpha = 0.1;  // strong-edge threshold

    for (int i = 1; i <= graph->numV; i++)
        parent[i] = i;

    for (int v = 1; v <= graph->numV; v++)
    {
        AdjListNode *temp = graph->AdjLists[v];
        while (temp)
        {
            int u = temp->vertex;
            if (areStrong(graph, v, u, alpha))
                join(parent, v, u);
            temp = temp->next;
        }
    }

    for (int i = 1; i <= graph->numV; i++)
        communities[i] = findParent(i, parent);

    return 0;
}

// Merge function for degrees array
void merge(int arr[][2], int l, int m, int r)
{
    int n1 = m - l + 1;
    int n2 = r - m;

    int L[n1][2], R[n2][2];

    for (int i = 0; i < n1; i++)
    {
        L[i][0] = arr[l + i][0];
        L[i][1] = arr[l + i][1];
    }
    for (int j = 0; j < n2; j++)
    {
        R[j][0] = arr[m + 1 + j][0];
        R[j][1] = arr[m + 1 + j][1];
    }

    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2)
    {
        if (L[i][1] >= R[j][1]) // descending
        {
            arr[k][0] = L[i][0];
            arr[k][1] = L[i][1];
            i++;
        }
        else
        {
            arr[k][0] = R[j][0];
            arr[k][1] = R[j][1];
            j++;
        }
        k++;
    }

    while (i < n1)
    {
        arr[k][0] = L[i][0];
        arr[k][1] = L[i][1];
        i++;
        k++;
    }
    while (j < n2)
    {
        arr[k][0] = R[j][0];
        arr[k][1] = R[j][1];
        j++;
        k++;
    }
}

void mergeSort(int arr[][2], int l, int r)
{
    if (l < r)
    {
        int m = l + (r - l) / 2;
        mergeSort(arr, l, m);
        mergeSort(arr, m + 1, r);
        merge(arr, l, m, r);
    }
}

int fastLouvain(Graph* graph,  int* communities)
{
    int n = graph->numV;
    int degrees[V+1][2]; // [0] = vertex, [1] = degree

    //Calculate degrees
    for (int i = 1; i <= n; i++)
    {
        degrees[i][0] = i;
        degrees[i][1] = nodeDegree(graph, i);
        communities[i] = 0; // initially no community
    }

    //Sort nodes by degree descending (high-degree first)
    mergeSort(degrees, 1, n);

    double m2 = 2.0 * totalGraphWeight(graph);
    int nextCommunity = 1;

    //Assign communities based on ΔQ
    for (int i = 1; i <= n; i++)
    {
        int v = degrees[i][0];
        int bestCommunity = 0;
        double bestDeltaQ = 0;

        // Check all existing communities
        for (int c = 1; c < nextCommunity; c++)
        {
            double ki = nodeDegree(graph, v);
            double ki_in = computeKi_in(graph, v, c, communities);
            double totC = communityWeight(graph, communities, c);

            double dQ = (ki_in / m2) - ((ki * totC) / (m2 * m2));

            if (dQ > bestDeltaQ)
            {
                bestDeltaQ = dQ;
                bestCommunity = c;
            }
        }

        // If no positive gain, create a new community
        if (bestDeltaQ <= 0)
        {
            communities[v] = nextCommunity;
            nextCommunity++;
        }
        else
        {
            communities[v] = bestCommunity;
        }
    }
    return 0;
}

//Toold funcs for Kernel Partition
int findKernel(Graph* graph, int* communities)
{
    int maxDegree = 0;
    int bestNode = 0;
    for (int v = 1; v <= graph->numV; v++)
    {
        if (communities[v] == 0 && nodeDegree(graph, v) > maxDegree)
        {
            maxDegree = nodeDegree(graph, v);
            bestNode = v;
        }
    }

    return bestNode;
}

int allAssigned(Graph* graph, int* communities)
{
    for (int i = 1; i <= graph->numV; i++)
    {
        if (communities[i] == 0)
        {
            return 0;
        }
    }
    return 1;
}

int kernelPartition(Graph* graph, int* communities)
{
    for (int vertex = 0; vertex <= graph->numV; vertex++)
    {
        communities[vertex] = 0;
    }
    int comId = 1;
    while (!allAssigned(graph, communities))
    {
        int kernel = findKernel(graph,communities);
        if (kernel == 0) break;
        //Assigning community around the kernel
        communities[kernel] = comId;
        AdjListNode* temp = graph->AdjLists[kernel];
        while (temp)
        {
            int nv = temp->vertex;
            if (communities[nv] != 0)
            {
                temp = temp->next;
                continue;
            }
            communities[nv] = comId;
            temp = temp->next;
        }
        comId++;
    }
    return 0;
}

// TOOL FUNCS
double mesureElapsedTime(int (*func)(Graph *, int *), Graph *graph, int *communities)
{
    double elapsed_time;
    LARGE_INTEGER frequency, start_time, end_time;
    QueryPerformanceFrequency(&frequency);

    QueryPerformanceCounter(&start_time);
    func(graph, communities);
    QueryPerformanceCounter(&end_time);
    elapsed_time = (double)(end_time.QuadPart - start_time.QuadPart) * 1000.0 / frequency.QuadPart;

    return elapsed_time;
}

// MAIN - COMPARING THE ALGORITHMS ON DATASET
int main()
{
    // Creating Graph From File
    const char filename[14] = "data.txt";
    Graph *graph = newGraph(V);
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("\nError! Cannot open the file.");
        return -1;
    }

    int source, dest;
    while (fscanf(file, " [%d %d]", &source, &dest) == 2)
    {
        addEdge(graph, source, dest, 1);
    }

    fclose(file);

    int communities[V] = {0};
    // Louvain Algrithm Phase 1
    double elapsed_time = mesureElapsedTime(louvainPhase1, graph, communities);
    printf("Louvain Phase 1:\nQ: %.4lf      Com: %d     Passed Time: %.4lfms\n", modularity(graph, communities), communityCount(graph, communities), elapsed_time);
    elapsed_time = mesureElapsedTime(louvainFull, graph, communities);
    printf("Louvain Complete Version:\nQ: %.4lf      Com: %d    Passed Time: %.4lfms\n", modularity(graph, communities), communityCount(graph, communities), elapsed_time);
    // Fast Greedy Algorithm
    elapsed_time = mesureElapsedTime(fastGreedy, graph, communities);
    printf("FastGreedy:\nQ: %.4lf       Com: %d       Passed Time: %.4lfms\n", modularity(graph, communities), communityCount(graph, communities), elapsed_time);
    // My Algorithm - Structural Connectivity Partition (SCP)
    elapsed_time = mesureElapsedTime(SCP, graph, communities);
    printf("My Algorithm SCP - (Structural Connectivity Partition):\nQ: %.4lf   Com: %d     Passed Time: %4.lfms\n", modularity(graph, communities), communityCount(graph, communities), elapsed_time);
    // My Algorithm - Fast Louvain
    elapsed_time = mesureElapsedTime(fastLouvain, graph, communities);
    printf("My Algorithm Fast Louvain:\nQ: %.4lf   Com: %d     Passed Time: %.4lfms\n", modularity(graph, communities), communityCount(graph, communities), elapsed_time);
    //My Algorithm - Kernel Partition
    elapsed_time = mesureElapsedTime(kernelPartition, graph, communities);
    printf("My Algorithm Kernel Partition:\nQ: %.4lf   Com: %d     Passed Time: %.4lfms\n", modularity(graph, communities), communityCount(graph, communities), elapsed_time);
    freeGraph(graph);
    return 0;
}
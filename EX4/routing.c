#include <stdio.h>

#define MAX 10
#define INF 999

/* Display Cost Matrix */
void showMatrix(int n, int graph[MAX][MAX])
{
    int i, j;
    printf("\n========================================\n");
    printf(" CURRENT COST MATRIX\n");
    printf("========================================\n");
    printf("\n\t");
    for (i = 0; i < n; i++)
        printf("%c\t", 'A' + i);
    printf("\n");
    for (i = 0; i < n; i++)
    {
        printf("%c\t", 'A' + i);
        for (j = 0; j < n; j++)
        {
            if (graph[i][j] == INF)
                printf("INF\t");
            else
                printf("%d\t", graph[i][j]);
        }
        printf("\n");
    }
}

/* Helper function to display complete path for Distance Vector */
void showDistanceVectorPath(int src, int dest, int next[MAX][MAX])
{
    int current = src;
    printf("%c", 'A' + current);
    while (current != dest)
    {
        current = next[current][dest];
        if (current == -1)
            break;
        printf(" -> %c", 'A' + current);
    }
}

/* Standalone function to render the finalized absolute path configurations */
void printFinalRoutingTable(int n, int dist[MAX][MAX], int next[MAX][MAX])
{
    int i, j;
    printf("\n========================================================================\n");
    printf(" COMPLETE FINAL ROUTING METRICS TABLE\n");
    printf("========================================================================\n");

    for (i = 0; i < n; i++)
    {
        printf("\nRouting Table for Node: [ %c ]\n", 'A' + i);
        printf("------------------------------------------------------------------------\n");
        printf("Destination\tNext Hop\tMin Cost\tShortest Path\n");
        printf("------------------------------------------------------------------------\n");
        for (j = 0; j < n; j++)
        {
            printf("%c\t\t", 'A' + j);
            if (i == j)
            {
                printf("-\t\t0\t\t%c\n", 'A' + i);
            }
            else if (dist[i][j] == INF)
            {
                printf("-\t\tINF\t\tNo Path\n");
            }
            else
            {
                printf("%c\t\t%d\t\t", 'A' + next[i][j], dist[i][j]);
                showDistanceVectorPath(i, j, next);
                printf("\n");
            }
        }
        printf("------------------------------------------------------------------------\n");
    }
}

/* Distance Vector Routing */
void distanceVector(int n, int cost[MAX][MAX])
{
    int dist[MAX][MAX];
    int next[MAX][MAX];
    int i, j, k;
    int change;
    int round = 0;

    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
        {
            dist[i][j] = cost[i][j];
            if (cost[i][j] != INF && i != j)
                next[i][j] = j;
            else
                next[i][j] = -1;
        }
    }

    do
    {
        change = 0;
        round++;
        for (i = 0; i < n; i++)
        {
            for (j = 0; j < n; j++)
            {
                for (k = 0; k < n; k++)
                {
                    if (dist[i][k] != INF && dist[k][j] != INF)
                    {
                        if (dist[i][k] + dist[k][j] < dist[i][j])
                        {
                            dist[i][j] = dist[i][k] + dist[k][j];
                            next[i][j] = next[i][k];
                            change = 1;
                        }
                    }
                }
            }
        }
    } while (change);

    printf("\nDistance Vector Algorithm converged in %d rounds.", round);
    printFinalRoutingTable(n, dist, next);
}

/* Find Minimum Distance for Link State */
int findMin(int dist[MAX], int visited[MAX], int n)
{
    int min = INF;
    int pos = -1;
    int i;
    for (i = 0; i < n; i++)
    {
        if (!visited[i] && dist[i] < min)
        {
            min = dist[i];
            pos = i;
        }
    }
    return pos;
}

/* Helper to trace standard Link State Parents */
void getLinkStatePath(int parent[MAX], int node, int pathStore[MAX], int *pathLen)
{
    if (node == -1) return;
    getLinkStatePath(parent, parent[node], pathStore, pathLen);
    pathStore[(*pathLen)++] = node;
}

/* Link State Routing - Dijkstra */
void linkState(int n, int graph[MAX][MAX])
{
    int dist[MAX][MAX];
    int next[MAX][MAX];
    int i, j, count, current;

    // Run Dijkstra for every node to match the global routing context
    for (int source = 0; source < n; source++)
    {
        int singleDist[MAX];
        int visited[MAX] = {0};
        int parent[MAX];

        for (i = 0; i < n; i++)
        {
            singleDist[i] = INF;
            parent[i] = -1;
        }
        singleDist[source] = 0;

        for (count = 0; count < n - 1; count++)
        {
            current = findMin(singleDist, visited, n);
            if (current == -1) break;
            visited[current] = 1;
            for (i = 0; i < n; i++)
            {
                if (!visited[i] && graph[current][i] != INF)
                {
                    int newCost = singleDist[current] + graph[current][i];
                    if (newCost < singleDist[i])
                    {
                        singleDist[i] = newCost;
                        parent[i] = current;
                    }
                }
            }
        }

        // Convert the computed single-source Dijkstra structure into global next-hop matrices
        for (j = 0; j < n; j++)
        {
            dist[source][j] = singleDist[j];
            if (source == j || singleDist[j] == INF)
            {
                next[source][j] = -1;
            }
            else
            {
                int pathStore[MAX];
                int pathLen = 0;
                getLinkStatePath(parent, j, pathStore, &pathLen);
                if (pathLen > 1)
                    next[source][j] = pathStore[1]; // Target adjacent hop index
                else
                    next[source][j] = j;
            }
        }
    }

    printf("\nLink State Algorithm path tree construction completed.");
    printFinalRoutingTable(n, dist, next);
}

/* Function to change edge weights */
void changeRouteCost(int n, int graph[MAX][MAX])
{
    char uChar, vChar;
    int u, v, newCost;
    printf("\nEnter source router letter for the link to modify (A-%c): ", 'A' + n - 1);
    scanf(" %c", &uChar);
    printf("Enter destination router letter for the link to modify (A-%c): ", 'A' + n - 1);
    scanf(" %c", &vChar);

    if (uChar >= 'a' && uChar <= 'z') uChar -= 32;
    if (vChar >= 'a' && vChar <= 'z') vChar -= 32;

    u = uChar - 'A';
    v = vChar - 'A';

    if (u < 0 || u >= n || v < 0 || v >= n)
    {
        printf("Invalid router selections!\n");
        return;
    }
    if (u == v)
    {
        printf("Self-loop costs cannot be changed from 0.\n");
        return;
    }

    printf("Enter new cost (Enter %d for no connection / broken link): ", INF);
    scanf("%d", &newCost);

    graph[u][v] = newCost;
    graph[v][u] = newCost;
    printf("\nLink cost between %c and %c successfully updated!\n", 'A' + u, 'A' + v);
}

/* Main Function */
int main()
{
    int n;
    int graph[MAX][MAX];
    int i, j;
    int choice;

    printf("========================================\n");
    printf(" ROUTING ALGORITHM SIMULATOR\n");
    printf("========================================\n");

    printf("\nEnter number of routers (maximum %d): ", MAX);
    scanf("%d", &n);
    if (n <= 0 || n > MAX)
    {
        printf("\nInvalid number of routers!\n");
        return 0;
    }

    printf("\nEnter the initial cost matrix.\n");
    printf("Enter %d for no connection.\n\n", INF);
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
        {
            if (i == j)
            {
                graph[i][j] = 0;
            }
            else if (i < j)
            {
                printf("Cost %c <-> %c: ", 'A' + i, 'A' + j);
                scanf("%d", &graph[i][j]);
                graph[j][i] = graph[i][j];
            }
        }
    }

    showMatrix(n, graph);

    while (1)
    {
        printf("\n========================================\n");
        printf(" OPERATIONS MENU\n");
        printf("========================================\n");
        printf("1. Run Distance Vector Algorithm & Print Final Tables\n");
        printf("2. Run Link State Algorithm & Print Final Tables\n");
        printf("3. Run Both Algorithms\n");
        printf("4. Modify an Existing Route Cost\n");
        printf("5. Exit\n");
        printf("\nEnter your choice: ");
        scanf("%d", &choice);

        if (choice == 5)
        {
            printf("\nProgram terminated.\n");
            break;
        }

        switch (choice)
        {
        case 1:
            distanceVector(n, graph);
            break;
        case 2:
            linkState(n, graph);
            break;
        case 3:
            printf("\n--- Running Distance Vector Subroutine ---\n");
            distanceVector(n, graph);
            printf("\n--- Running Link State Subroutine ---\n");
            linkState(n, graph);
            break;
        case 4:
            changeRouteCost(n, graph);
            showMatrix(n, graph);
            break;
        default:
            printf("\nInvalid selection! Please enter a choice between 1 and 5.\n");
        }
    }

    return 0;
}

// Name : Lara Abu Asfour

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 100 // Define maximum size for various arrays
#define INF 999999  // large value to represent infinity
#define MAX_COURSES 100 // Define maximum number of courses
#define MAX_LENGTH 20 // Define maximum length of strings for course names

// Structure to store building information
typedef struct {
    char name[50];// Name of the building
    int index; // Index of the building
} Building;

// Structure to store graph edges
typedef struct {
    int vertex;
    int weight;// Weight of the edge
} Edge;

// Structure to represent a graph for Dijkstra's algorithm
typedef struct {
    int *dist;// Array to store shortest distances
    int *prev;// Array to store previous nodes in shortest path
    int *visited;// Array to track visited nodes
    int *adjMatrix;// Adjacency matrix to represent the graph
    int size;// Size of the graph
} Graph;

// Structure to store course information
typedef struct {
    char course[50];// Name of the course
    char prerequisites[5][50];// Names of prerequisites
    int prerequisitesCount;// Number of prerequisites for the course
} Course;

// Structure to represent a node in the graph for topological sort
typedef struct {
    char course[MAX_LENGTH];
    int inDegree;// Indegree of the course node
    char dependents[MAX_COURSES][MAX_LENGTH];// Names of courses dependent on this course
    int dependentsCount;// Number of courses dependent on this course
} GraphNode;

// Global variables
Building buildings[MAX];// Array to store buildings
int buildingCount = 0;
Course courses[MAX];// Array to store courses
int courseCount = 0;
char sortedCourses[MAX][50];// Array to store sorted courses
int sortedCourseCount = 0;
GraphNode graph[MAX_COURSES];// Array to store graph nodes for topological sort

// Function to initialize the graph for Dijkstra's algorithm
void initializeGraph(Graph *g, int size) {
    g->size = size; // Set the size of the graph
    g->dist = (int *)malloc(size * sizeof(int));// Allocate memory for shortest distances array
    g->prev = (int *)malloc(size * sizeof(int));// Allocate memory for previous nodes array
    g->visited = (int *)malloc(size * sizeof(int));// Allocate memory for visited nodes array
    g->adjMatrix = (int *)malloc(size * size * sizeof(int)); // Allocate memory for adjacency matrix
    for (int i = 0; i < size; i++) { // Loop through rows and columns of adjacency matrix
        for (int j = 0; j < size; j++) {
            g->adjMatrix[i * size + j] = (i == j) ? 0 : INF;// Set diagonal elements to 0, others to infinity
        }
    }
}

// Function to add a new building
void addBuilding(char *name) {
    strcpy(buildings[buildingCount].name, name);// Copy building name to the next available index in the buildings array
    buildings[buildingCount].index = buildingCount;// Set the index of the building
    buildingCount++;
}

// Function to get the index of a building by its name
int getBuildingIndex(char *name) {
    for (int i = 0; i < buildingCount; i++) {// Loop through buildings array
        if (strcmp(buildings[i].name, name) == 0) {// Compare building names
            return buildings[i].index;// Return index if found
        }
    }
    return -1;// Return -1 if not
}

// Function to add an edge between two buildings in the graph
void addEdge(Graph *g, char *start, char *end, int weight) {
    int u = getBuildingIndex(start);// Get index of starting building
    int v = getBuildingIndex(end);// Get index of ending building
    if (u == -1) {// If starting building not found
        addBuilding(start);// Add it to the buildings array
        u = buildingCount - 1;// Update index
    }
    if (v == -1) {// If ending building not found
        addBuilding(end);// Add it to the buildings array
        v = buildingCount - 1;
    }
    g->adjMatrix[u * g->size + v] = weight;// Set weight of edge in adjacency matrix
}

// Function to implement Dijkstra's algorithm to find the shortest path from the source to all other vertices
void dijkstra(Graph *g, int src) {
    for (int i = 0; i < g->size; i++) {// Loop through nodes in the graph
        g->dist[i] = INF;// Set initial distances to infinity
        g->prev[i] = -1;// Set initial previous nodes to -1
        g->visited[i] = 0;
    }
    g->dist[src] = 0;// Set distance of source node to 0

    for (int i = 0; i < g->size; i++) {// Loop through nodes in the graph
        int min = INF, minIndex = -1;// Initialize minimum distance and index
        for (int j = 0; j < g->size; j++) {// Loop through nodes in the graph
            if (!g->visited[j] && g->dist[j] < min) {// If node is not visited and has smaller distance
                min = g->dist[j];// Update minimum distance
                minIndex = j;
            }
        }
        if (minIndex == -1) break;// If no more nodes to visit, break the loop
        g->visited[minIndex] = 1;// Mark node as visited

        for (int j = 0; j < g->size; j++) {
            if (!g->visited[j] && g->adjMatrix[minIndex * g->size + j] != INF && g->dist[minIndex] + g->adjMatrix[minIndex * g->size + j] < g->dist[j]) {// If node is not visited and there is a shorter path to it
                g->dist[j] = g->dist[minIndex] + g->adjMatrix[minIndex * g->size + j];// Update distance
                g->prev[j] = minIndex;
            }
        }
    }
}

// Function to print the shortest path from source to destination
void printPath(Graph *g, int dest) {
    if (g->prev[dest] != -1) { // If destination has a previous node
        printPath(g, g->prev[dest]);// Recursively print path from source to previous node
    }
    printf("%s ", buildings[dest].name);// Print the name of the current building
}

// Function to load the buildings and their connections from a file
void loadFile(Graph *g) {
    FILE *file = fopen("input_buildings.txt", "r");
    if (!file) {
        printf("Error opening file!\n");
        return;
    }

    char line[100];//to store each line of the file
    while (fgets(line, sizeof(line), file)) {// Read each line of the file
        char *start = strtok(line, "#");// Tokenize line to get start building name
        char *end = strtok(NULL, "#");// Tokenize line to get end building name
        char *weightStr = strtok(NULL, "#");
        if (start && end && weightStr) {// Check if all required fields are present
            int weight = atoi(weightStr);// Convert weight string to integer
            addEdge(g, start, end, weight);// Add edge between buildings to the graph
        }
    }

    fclose(file);
}

// Function to read courses and their prerequisites from a file
void readInputFile() {
    FILE *file = fopen("input_courses.txt", "r");
    if (!file) {
        printf("Error opening file!\n");
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {// Read each line of the file
        Course course;
        char *token = strtok(line, "#\n");// Tokenize line to get course name
        if (token == NULL) {// Check if token is NULL
            continue;
        }
        strcpy(course.course, token);// Copy course name to Course object
        course.prerequisitesCount = 0;// Initialize prerequisites count

        // Parse prerequisites
        while ((token = strtok(NULL, "#\n"))) {// Continue tokenizing until end of line
            if (course.prerequisitesCount < MAX_COURSES) {// Check if prerequisites count is within limits
                strcpy(course.prerequisites[course.prerequisitesCount++], token);// Copy prerequisite to Course object
            } else {
                fprintf(stderr, "Too many prerequisites for course %s\n", course.course);
                return;
            }
        }
        if (courseCount < MAX_COURSES) {// Check if course count is within limits
            courses[courseCount++] = course;// Add course to courses array
        } else {
            fprintf(stderr, "Too many courses\n");
            return;
        }
    }
    fclose(file);
}

// Function to find the index of a course in the graph
int findGraphNodeIndex(const char *course) {
    for (int i = 0; i < MAX_COURSES; i++) {// Loop through graph nodes
        if (strcmp(graph[i].course, course) == 0) {// Check if course name matches
            return i;// Return index if course found
        }
    }
    return -1;
}

// Function to build the graph for topological sorting of courses
void buildGraph() {
    int graphNodeCount = 0;// Initialize

    for (int i = 0; i < courseCount; i++) {// Loop through courses
        int currentIndex = findGraphNodeIndex(courses[i].course);// Find index of current course in graph
        if (currentIndex == -1) {// If course not found in graph
            strcpy(graph[graphNodeCount].course, courses[i].course);// Copy course name to graph node
            graph[graphNodeCount].inDegree = 0;// Initialize in-degree to 0
            graph[graphNodeCount].dependentsCount = 0;// Initialize dependents count to 0
            currentIndex = graphNodeCount;// Set currentIndex to current graph node index
            graphNodeCount++;
        }

        for (int j = 0; j < courses[i].prerequisitesCount; j++) {// Loop through course prerequisites
            int prereqIndex = findGraphNodeIndex(courses[i].prerequisites[j]);// Find index of prerequisite in graph
            if (prereqIndex == -1) {// If prerequisite not found in graph
                strcpy(graph[graphNodeCount].course, courses[i].prerequisites[j]);// Copy prerequisite name to graph node
                graph[graphNodeCount].inDegree = 0;
                graph[graphNodeCount].dependentsCount = 0;
                prereqIndex = graphNodeCount;// Set prereqIndex to current graph node index

                graphNodeCount++;
            }

            graph[currentIndex].inDegree++; // Increment in-degree of current course
            strcpy(graph[prereqIndex].dependents[graph[prereqIndex].dependentsCount++], courses[i].course);// Add current course as dependent to prerequisite
        }
    }
}

// Function to perform topological sorting of the courses
void topologicalSortCourses() {
    char result[MAX_COURSES][MAX_LENGTH];// Array to store sorted courses
    int resultCount = 0;
    int inDegreeZeroQueue[MAX_COURSES];// Queue to store courses with in-degree 0
    int queueStart = 0, queueEnd = 0;// Initialize queue indices

    // Enqueue courses with in-degree 0
    for (int i = 0; i < MAX_COURSES; i++) {// Loop through graph nodes
        if (graph[i].course[0] != '\0' && graph[i].inDegree == 0) {// Check if graph node is valid and has in-degree 0
            inDegreeZeroQueue[queueEnd++] = i;// Add node index to queue
        }
    }

    // Sort courses with in-degree 0 lexicographically
    for (int i = 0; i < queueEnd - 1; i++) {// Loop through queue indices
        for (int j = i + 1; j < queueEnd; j++) {// Loop through remaining queue indices
            if (strcmp(graph[inDegreeZeroQueue[i]].course, graph[inDegreeZeroQueue[j]].course) > 0) {// Compare course names lexicographically
                int temp = inDegreeZeroQueue[i];// Swap queue indices if necessary
                inDegreeZeroQueue[i] = inDegreeZeroQueue[j];
                inDegreeZeroQueue[j] = temp;
            }
        }
    }

    // Perform topological sort
    while (queueStart < queueEnd) {// Loop until queue is empty
        int currentIndex = inDegreeZeroQueue[queueStart++];// Dequeue current node index
        strcpy(result[resultCount++], graph[currentIndex].course);// Add current course to result array

        for (int i = 0; i < graph[currentIndex].dependentsCount; i++) {// Loop through dependents of current course
            int dependentIndex = findGraphNodeIndex(graph[currentIndex].dependents[i]);// Find index of dependent in graph
            graph[dependentIndex].inDegree--;
            if (graph[dependentIndex].inDegree == 0) {// If in-degree becomes 0
                inDegreeZeroQueue[queueEnd++] = dependentIndex;// Enqueue dependent node index
                // Keep the queue sorted
                for (int i = queueStart; i < queueEnd - 1; i++) {// Loop through queue indices
                    for (int j = i + 1; j < queueEnd; j++) {
                        if (strcmp(graph[inDegreeZeroQueue[i]].course, graph[inDegreeZeroQueue[j]].course) > 0) {
                            int temp = inDegreeZeroQueue[i];
                            inDegreeZeroQueue[i] = inDegreeZeroQueue[j];
                            inDegreeZeroQueue[j] = temp;
                        }
                    }
                }
            }
        }
    }

    // Copy sorted courses to global array
    sortedCourseCount = resultCount;// Update sorted course count
    for (int i = 0; i < resultCount; i++) {// Loop through sorted courses
        strcpy(sortedCourses[i], result[i]);// Copy course name to global array
    }
}

// Function to print the sorted courses
void printSortedCourses() {
    printf("The order of courses to take is:\n");
    for (int i = 0; i < sortedCourseCount; i++) {// Loop through sorted courses
        printf("%s\n", sortedCourses[i]);
    }
}

void displayMainMenu() {
    printf("Please choose a choice from 1 to 6:\n");
    printf("1. Load the two files.\n");
    printf("2. Calculate the shortest distance between two buildings.\n");
    printf("3. Print the shortest route between two buildings and the total distance.\n");
    printf("4. Sort the courses using topological sort.\n");
    printf("5. Print the sorted courses.\n");
    printf("6. Exit the application.\n");
}

int main() {
    Graph g;
    initializeGraph(&g, MAX);// Initialize graph

    while (1) {
        displayMainMenu();
        int choice;
        scanf("%d", &choice);

        if (choice == 1) {
            loadFile(&g);  // Load building data
            readInputFile();  // Load course data
            buildGraph();  // Build the graph for topological sorting
        }
        else if (choice == 2) {
            char start[50], end[50];// variables to store input for starting and destination buildings
            printf("Enter the starting building: ");
            scanf("%s", start);//name of the starting building
            printf("Enter the destination building: ");
            scanf("%s", end);
            int src = getBuildingIndex(start);// Get the index of the starting
            int dest = getBuildingIndex(end);// Get the index of the destination building
            if (src == -1 || dest == -1) {
                printf("Building not found!\n");
            } else {
                dijkstra(&g, src);  // Calculate shortest path
                printf("Shortest distance between %s and %s is %d\n", start, end, g.dist[dest]);
            }
        }
        else if (choice == 3) {
            char start[50], end[50];
            printf("Enter the starting building: ");
            scanf("%s", start);
            printf("Enter the destination building: ");
            scanf("%s", end);
            int src = getBuildingIndex(start);
            int dest = getBuildingIndex(end);
            if (src == -1 || dest == -1) {
                printf("Building not found!\n");
            } else {
                dijkstra(&g, src);  // Calculate shortest path
                printf("The shortest route between %s and %s is: ", start, end);
                printPath(&g, dest);  // Print the path
                printf("\nTotal distance: %d\n", g.dist[dest]);
            }
        }
        else if (choice == 4) {
            topologicalSortCourses();  // Sort courses
            printf("Courses sorted using topological sort.\n");
        }
        else if (choice == 5) {
            printSortedCourses();  // Print sorted courses
        }
        else if (choice == 6) {
            break;
        } else {
            printf("Please choose a valid operation from 1 to 6.\n\n");
        }
    }

    // Free allocated memory
    free(g.adjMatrix);
    free(g.dist);
    free(g.prev);
    free(g.visited);
    return 0;
}

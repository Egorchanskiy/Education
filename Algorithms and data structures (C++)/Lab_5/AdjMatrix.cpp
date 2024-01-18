#include <iostream>
#include <fstream>
#include <limits>
#include <time.h>

#define INF std::numeric_limits<int>::max()

void dijkstra(int** graph, int vertices, int start)
{
    int* distances = new int[vertices];
    bool* visited = new bool[vertices];

    for (int i = 0; i < vertices; ++i)
    {
        distances[i] = INF;
        visited[i] = false;
    }

    distances[start] = 0;

    bool hasPath = true;

    for (int count = 0; count < vertices - 1; ++count)
    {
        int minDistance = INF;
        int minIndex = -1;

        for (int v = 0; v < vertices; ++v)
        {
            if (!visited[v] && distances[v] < minDistance)
            {
                minDistance = distances[v];
                minIndex = v;
            }
        }

        visited[minIndex] = true;

        if (minIndex == -1)
        {
            //Если не найдено непосещенных вершин, выходим из цикла
            break;
        }

        for (int v = 0; v < vertices; ++v)
        {
            if (!visited[v] && graph[minIndex][v] && distances[minIndex] != INF &&
                distances[minIndex] + graph[minIndex][v] < distances[v])
            {
                distances[v] = distances[minIndex] + graph[minIndex][v];
            }
        }

        if (distances[minIndex] == INF)
        {
            hasPath = false;
        }
    }

    std::cout << "Shortest distances from the starting vertex " << start << ":\n";
    for (int i = 0; i < vertices; ++i)
    {
        if (distances[i] == INF)
        {
            std::cout << "To vertex " << i << ": "; //i+1 для удобного сравнения с graphonline
            if (!hasPath)
            {
                std::cout << "No way\n";
            }
            else
            {
                std::cout << "Infinity\n";
            }
        }
        else
        {
            std::cout << "To vertex " << i << ": " << distances[i] << "\n"; //i+1 для удобного сравнения с graphonline
        }
    }

    delete[] distances;
    delete[] visited;
}



int main()
{
	
	clock_t start_time, end_time;
    double execution_time;
	
	
    std::ifstream inputFile("graph.txt");
    if (!inputFile)
    {
        std::cout << "Impossible to open file";
        return 1;
    }

    int vertices;
    inputFile >> vertices;
    inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //Пропускаем пустую строку

    int** graph = new int*[vertices];
    for (int i = 0; i < vertices; ++i)
    {
        graph[i] = new int[vertices];
        for (int j = 0; j < vertices; ++j)
        {
            inputFile >> graph[i][j];
        }
    }

    inputFile.close();  //Закрываем файл после чтения данных

    int startVertex;

    std::cout << "Enter starting vertex: "; //ввод с 0
    std::cin >> startVertex;

    if (startVertex >= 0 && startVertex < vertices)
    {
		start_time = clock();
        dijkstra(graph, vertices, startVertex);
				end_time = clock();
		execution_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

		printf("Время выполнения программы: %f секунд\n", execution_time);
    }
    else
    {
        std::cout << "Starting vertex incorrect.\n";
    }

    for (int i = 0; i < vertices; ++i)
    {
        delete[] graph[i];
    }
    delete[] graph;

    return 0;
}

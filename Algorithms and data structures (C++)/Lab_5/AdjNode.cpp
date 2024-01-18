#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <limits.h>
#include <time.h>

//Алгоритм Дейкстры с графом, представленным списком смежности

//Структура для представления узла в списке смежности
struct AdjListNode
{
    int dest;
    int weight;
    int isDirected; //0 - неориентированный граф, 1 - ориентированный граф
    struct AdjListNode* next;
};

//Структура для представления списка смежности
struct AdjList
{
    //Указатель на голову списка
    struct AdjListNode* head;
};

//Структура для представления графа
//Граф представляет собой массив списков смежности
//Размер массива будет V количество вершин в графе
struct Graph
{
    int V;
    struct AdjList* array;
};

//Функция для создания нового узла списка смежности
struct AdjListNode* newAdjListNode(int dest, int weight)
{
    struct AdjListNode* newNode = (struct AdjListNode*)malloc(sizeof(struct AdjListNode));
    newNode->dest = dest;
    newNode->weight = weight;
    newNode->isDirected = 0; //По умолчанию неориентированное ребро
    newNode->next = NULL;
    return newNode;
}

//Функция для создания графа с V вершинами
struct Graph* createGraph(int V)
{
    struct Graph* graph = (struct Graph*)malloc(sizeof(struct Graph));
    graph->V = V;

    //Создание массива списков смежности.
    //Размер массива будет V
    graph->array = (struct AdjList*)malloc(V * sizeof(struct AdjList));

    //Инициализация каждого списка смежности как пустого путем установки головы в NULL
    for (int i = 0; i < V; ++i)
        graph->array[i].head = NULL;

    return graph;
}

//Добавление ребра в ориентированный или неориентированный граф
void addEdge(struct Graph* graph, int src, int dest, int weight)
{
    //Добавление ребра от src к dest.
    //Новый узел добавляется в начало списка смежности src.
    struct AdjListNode* newNode = newAdjListNode(dest, weight);
    newNode->isDirected = 1; //Ориентированное ребро
    newNode->next = graph->array[src].head;
    graph->array[src].head = newNode;

    //Если граф неориентированный, добавить ребро от dest к src
    if (!newNode->isDirected)
    {
        newNode = newAdjListNode(src, weight);
        newNode->next = graph->array[dest].head;
        graph->array[dest].head = newNode;
    }
}

//Структура для представления узла мин-кучи
struct MinHeapNode
{
    int  v;
    int dist;
};

//Структура для представления мин-кучи
struct MinHeap
{
    //Количество узлов кучи
    int size;
    //Емкость мин-кучи
    int capacity;
    //Это нужно для функции decreaseKey()
    int *pos;
    struct MinHeapNode **array;
};

//Функция для создания нового узла мин-кучи
struct MinHeapNode* newMinHeapNode(int v, int dist)
{
    struct MinHeapNode* minHeapNode = (struct MinHeapNode*)malloc(sizeof(struct MinHeapNode));
    minHeapNode->v = v;
    minHeapNode->dist = dist;
    return minHeapNode;
}

//Функция для создания мин-кучи
struct MinHeap* createMinHeap(int capacity)
{
    struct MinHeap* minHeap = (struct MinHeap*)malloc(sizeof(struct MinHeap));
    minHeap->pos = (int *)malloc(capacity * sizeof(int));
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->array = (struct MinHeapNode**)malloc(capacity * sizeof(struct MinHeapNode*));
    return minHeap;
}

//Функция для обмена двух узлов мин-кучи.
//Необходимо для функции minHeapify
void swapMinHeapNode(struct MinHeapNode** a, struct MinHeapNode** b)
{
    struct MinHeapNode* t = *a;
    *a = *b;
    *b = t;
}

//Стандартная функция для преобразования кучи в мин-кучу при заданном индексе
//Эта функция плюсом обновляет позицию узлов при их обмене.
//Позиция необходима для функции decreaseKey()
void minHeapify(struct MinHeap* minHeap, int idx)
{
    int smallest, left, right;
    smallest = idx;
    left = 2 * idx + 1;
    right = 2 * idx + 2;

    if (left < minHeap->size &&
        minHeap->array[left]->dist < minHeap->array[smallest]->dist)
        smallest = left;

    if (right < minHeap->size &&
        minHeap->array[right]->dist < minHeap->array[smallest]->dist)
        smallest = right;

    if (smallest != idx)
    {
        //Узлы, которые нужно обменять в мин-куче
        struct MinHeapNode* smallestNode = minHeap->array[smallest];
        struct MinHeapNode* idxNode = minHeap->array[idx];

        //Обмен позициями
        minHeap->pos[smallestNode->v] = idx;
        minHeap->pos[idxNode->v] = smallest;

        //Обмен узлами
        swapMinHeapNode(&minHeap->array[smallest], &minHeap->array[idx]);

        minHeapify(minHeap, smallest);
    }
}

//Вспомогательная функция для проверки, пустая ли данная мин-куча или нет
int isEmpty(struct MinHeap* minHeap)
{
    return minHeap->size == 0;
}

//Стандартная функция для извлечения минимального узла из кучи
struct MinHeapNode* extractMin(struct MinHeap* minHeap)
{
    if (isEmpty(minHeap))
        return NULL;

    //Сохранение корневого узла
    struct MinHeapNode* root = minHeap->array[0];

    //Замена корневого узла последним узлом
    struct MinHeapNode* lastNode = minHeap->array[minHeap->size - 1];
    minHeap->array[0] = lastNode;

    //Обновление позиции последнего узла
    minHeap->pos[root->v] = minHeap->size - 1;
    minHeap->pos[lastNode->v] = 0;

    //Уменьшение размера кучи и преобразование корня
    --minHeap->size;
    minHeapify(minHeap, 0);

    return root;
}

//Функция для уменьшения значения dist у заданной вершины v. Эта функция
//использует pos[] мин-кучи для получения текущего индекса узла в мин-куче.
void decreaseKey(struct MinHeap* minHeap, int v, int dist)
{
    //Получение индекса v в массиве кучи
    int i = minHeap->pos[v];

    //Получение узла и обновление его значения dist
    minHeap->array[i]->dist = dist;

    //Проход вверх по дереву, пока не будет сформирована полная мин-куча.
    //Это цикл с O(Logn) итерациями
    while (i && minHeap->array[i]->dist < minHeap->array[(i - 1) / 2]->dist)
    {
        //Обмен этого узла с его родителем
        minHeap->pos[minHeap->array[i]->v] = (i - 1) / 2;
        minHeap->pos[minHeap->array[(i - 1) / 2]->v] = i;
        swapMinHeapNode(&minHeap->array[i], &minHeap->array[(i - 1) / 2]);

        //Переход к индексу родителя
        i = (i - 1) / 2;
    }
}

//Вспомогательная функция для проверки, находится ли заданная вершина 'v'
//в мин-куче или нет
bool isInMinHeap(struct MinHeap* minHeap, int v)
{
    if (minHeap->pos[v] < minHeap->size)
        return true;
    return false;
}

//Вспомогательная функция для печати решения
void printArr(int dist[], int n)
{
    printf("Vertex | Path from the initial vertex\n");
    for (int i = 0; i < n; ++i)
        printf("%d \t\t %d\n", i, dist[i]);
}

//Основная функция, которая вычисляет кратчайшие пути от источника 'src' ко всем
//вершинам. Сложность функции O(ELogV)
void dijkstra(struct Graph* graph, int src)
{
    int V = graph->V; //Получение количества вершин в графе
    int dist[V];      //Значения расстояний, используемые для выбора ребра минимального веса в разрезе

    //minHeap представляет множество E
    struct MinHeap* minHeap = createMinHeap(V);

    //Инициализация мин-кучи со всеми вершинами и расстояниями равными INT_MAX
    for (int v = 0; v < V; ++v)
    {
        dist[v] = INT_MAX;
        minHeap->array[v] = newMinHeapNode(v, dist[v]);
        minHeap->pos[v] = v;
    }

    //Установка расстояния для исходной вершины в 0,
    //так как расстояние от исходной вершины до себя равно 0
    minHeap->array[src] = newMinHeapNode(src, dist[src]);
    minHeap->pos[src] = src;
    dist[src] = 0;
    decreaseKey(minHeap, src, dist[src]);

    //Размер мин-кучи равен V
    minHeap->size = V;

    //В конечном итоге размер minHeap станет равен 0
    while (!isEmpty(minHeap))
    {
        //Извлечение вершины с минимальным расстоянием из кучи
        struct MinHeapNode* minHeapNode = extractMin(minHeap);
        int u = minHeapNode->v; //Сохранение извлеченной вершины
        struct AdjListNode* pCrawl = graph->array[u].head;

        //Перебор всех смежных вершин данной извлеченной вершины (взвешенные ребра)
        while (pCrawl != NULL)
        {
            int v = pCrawl->dest;

            //Если v находится в мин-куче и его расстояние больше нового расстояния u + weight
            if (isInMinHeap(minHeap, v) && dist[u] != INT_MAX && pCrawl->weight + dist[u] < dist[v])
            {
                dist[v] = dist[u] + pCrawl->weight;

                //Обновление расстояния v в мин-куче
                decreaseKey(minHeap, v, dist[v]);
            }
            pCrawl = pCrawl->next;
        }
    }

    //Печать рассчитанных расстояний
    printArr(dist, V);
}

//Задание графа
int main()
{
    clock_t start_time, end_time;
    double execution_time;
	
	
	
    //Создание графа
    int V = 5; //Число вершин
	if (V != 0) {  
		struct Graph* graph = createGraph(V);

		//Добавление ребер
		addEdge(graph, 0, 1, 10);
		addEdge(graph, 0, 3, 30);
		addEdge(graph, 0, 4, 100);
		addEdge(graph, 1, 2, 50);
		addEdge(graph, 2, 4, 10);
		addEdge(graph, 3, 2, 20);
		addEdge(graph, 3, 4, 60);

		start_time = clock();


		dijkstra(graph, 0);


		end_time = clock();
		execution_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

		printf("Время выполнения программы: %f секунд\n", execution_time);
		
	}
	else {
		std::cout << "Empty graph!";
	}

    return 0;
}

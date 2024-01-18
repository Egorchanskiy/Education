#include <iostream>
//#include <fstream>
//#include <time.h>


using namespace std;

short int errs = 0;


//Функция обмена значениями двух переменных
void swap(int &a, int &b)
{
    int temp = a;
    a = b;
    b = temp;
}

//Функция для разбиения массива на разделы и возврата сводного индекса
int partition(int arr[], int low, int high)
{
    //Самый правый элемент выбирается в качестве опорного
    int pivot = arr[high];
    int i = low - 1;

    //Разбиение массива на разделы
    for (int j = low; j <= high - 1; j++)
    {
        if (arr[j] < pivot)
        {
            i++;
            swap(arr[i], arr[j]);
        }
    }
    swap(arr[i + 1], arr[high]);

    return i + 1;
}

//Быстрая сортировка
void quickSort(int arr[], int low, int high)
{
    if (low < high)
    {
        int pivotIndex = partition(arr, low, high);

        //Рекурсивная сортировка "подмассивов"
        quickSort(arr, low, pivotIndex - 1);
        quickSort(arr, pivotIndex + 1, high);
    }
}

//Функция вывода массива
void printArray(int arr[], int size)
{
    for (int i = 0; i < size; i++)
    {
        std::cout << arr[i] << " ";
    }
    std::cout << std::endl;
}

void inputArray(int *&arr, int &size)
{
    std::cout << "Enter array size: ";
    std::cin >> size;
	if(size != 0){
	
		//Выделение памяти для массива
		arr = new int[size];

		std::cout << "Enter array: ";
		for (int i = 0; i < size; i++)
		{
			std::cin >> arr[i];
		}
	} else {
		errs = 1;
		std::cout << "ENTERED EMPTY ARRAY " << std::endl;
	}
}

int main()
{
//	    //Измерение времени
//	time_t t1,t2;
//	double t_diff;	
	
    int *arr;
    int size;

    inputArray(arr, size);

	if(errs == 0) {
		std::cout << "Input array: ";
		printArray(arr, size);

	//	time(&t1);

		quickSort(arr, 0, size - 1);

	//	time(&t2);
	//	t_diff = difftime(t2,t1);
	//	printf("The time: %f seconds\n", t_diff);

		std::cout << "Output array: ";
		printArray(arr, size);

		//Освобождение памяти
		delete[] arr;
	}

    return 0;
}

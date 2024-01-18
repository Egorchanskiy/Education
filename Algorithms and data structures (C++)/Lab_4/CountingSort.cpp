#include <iostream>
//#include <fstream>
//#include <time.h>


using namespace std;

short int errs = 0;

void countingSort(int array[], int size, int range)
{
    // Создание массива для подсчета частоты элементов
    int *countArray = new int[range + 1]();

    // Подсчет частоты элементов
    for (int i = 0; i < size; i++)
    {
        countArray[array[i]]++;
    }

    // Восстановление отсортированного массива
    int index = 0;
    for (int i = 0; i <= range; i++)
    {
        while (countArray[i] > 0)
        {
            array[index] = i;
            index++;
            countArray[i]--;
        }
    }

    // Освобождение памяти
    delete[] countArray;
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

void printArray(int arr[], int size)
{
    for (int i = 0; i < size; i++)
    {
        std::cout << arr[i] << " ";
    }
    std::cout << std::endl;
}

int main()
{
	    //Измерение времени
//	time_t t1,t2;
//	double t_diff;
	
    int size;
    int *array;

	inputArray(array, size);

	if(errs == 0) {
		//Нахождение максимального элемента в массиве
		int maxElement = array[0];
		for (int i = 1; i < size; i++)
		{
			if (array[i] > maxElement)
			{
				maxElement = array[i];
			}
		}
		

		std::cout << "Input array ";
		printArray(array, size);
		
		
		//Сортировка подсчетом
		countingSort(array, size, maxElement);

		//Вывод отсортированного массива
		std::cout << "Output array ";
		printArray(array, size);

		// Освобождение памяти
		delete[] array;
	}

    return 0;
}
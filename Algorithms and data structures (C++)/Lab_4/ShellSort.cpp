#include <iostream>
//#include <fstream>
//#include <time.h>


using namespace std;

short int errs = 0;

//Сортировка Шелла
void shellSort(int arr[], int size)
{
    //Начало с большого промежутка и дальнейшее уменьшение
    for (int gap = size / 2; gap > 0; gap /= 2)
    {
        //Сортировка вставки по элементам, разделенным промежутком
        for (int i = gap; i < size; i++)
        {
            int temp = arr[i];
            int j;
            for (j = i; j >= gap && arr[j - gap] > temp; j -= gap)
            {
                arr[j] = arr[j - gap];
            }
            arr[j] = temp;
        }
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
    //Измерение времени
//	time_t t1,t2;
//	double t_diff;
	
	
	
	
	
	int *arr;
    int size;

    inputArray(arr, size);
	if(errs ==0) {
		std::cout << "Input array: ";
		printArray(arr, size);
		
	//	time(&t1);
		
		shellSort(arr, size);
		
	//	time(&t2);
	//	t_diff = difftime(t2,t1);
	//	printf("The time: %f seconds\n", t_diff);
		
		std::cout << "Output array: ";
		printArray(arr, size);

		// Освобождение памяти
		delete[] arr;
	}
    return 0;
}

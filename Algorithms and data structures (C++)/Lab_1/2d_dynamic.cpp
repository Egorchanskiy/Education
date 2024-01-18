#include <iostream>
#include <time.h>

using namespace std;

//Массив массивов + динамические выделение


//функция для освобождения памяти от двумерного динамического массива
void deleteMatrix(double** matrix, int rows) {
    for (int i = 0; i < rows; i++) {
        delete[] matrix[i];
    }
    delete[] matrix;
}

//функция для решения системы линейных алгебраических уравнений методом Гаусса
double* solveSystemOfEquations(double** matrix, int size) {
    //приведение к треугольному виду
    for (int i = 0; i < size - 1; i++) {
        for (int j = i + 1; j < size; j++) {
            double ratio = matrix[j][i] / matrix[i][i];
            for (int k = i; k < size + 1; k++) {
                matrix[j][k] -= ratio * matrix[i][k];
            }
        }
    }

    //обратный ход
    double* solution = new double[size];
    for (int i = size - 1; i >= 0; i--) {
        double sum = 0;
        for (int j = i + 1; j < size; j++) {
            sum += matrix[i][j] * solution[j];
        }
        solution[i] = (matrix[i][size] - sum) / matrix[i][i];
    }

    return solution;
}

int main() {
	
		    clock_t start_time, end_time;
    double execution_time;
	
    int size;

    std::cout << "Enter the number of rows (size) of the system: ";
    std::cin >> size;

    //Выделение памяти под расширенную матрицу
    double** matrix = new double*[size];
    for (int i = 0; i < size; i++) {
        matrix[i] = new double[size + 1];
    }

    //Ввод расширенной матрицы
    std::cout << "A|B original extended matrix:\n";
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size + 1; j++) {
            std::cin >> matrix[i][j];
        }
    }

    //Решение системы уравнений
					start_time = clock();
	
    double* solution = solveSystemOfEquations(matrix, size);
	
					end_time = clock();
		execution_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

		printf("Время выполнения программы: %f секунд\n", execution_time);

    //Вывод решения
    std::cout << "X solution matrix:\n";
    for (int i = 0; i < size; i++) {
        std::cout << "x" << i + 1 << " = " << solution[i] << std::endl;
    }

    //Освобождение памяти
    deleteMatrix(matrix, size);
    delete[] solution;

    return 0;
}

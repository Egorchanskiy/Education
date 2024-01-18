#include <iostream>
#include <fstream>
#include <sstream>
#include <time.h>

using namespace std;

//Одномерный массив + статическое выделение

const int MAX_SIZE = 100; //Максимальный размер матрицы

//Своеобразные errno
int errf = 0;
int errm = 0;



//Функция для чтения матрицы из текстового файла
void readMatrixFromFile(const std::string& filename, double matrix[], int& rows, int& cols) {
    std::ifstream file(filename);

    if (file.is_open()) {
        std::string line;
        int row = 0;
        int col = 0;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            double value;
            while (iss >> value) {
                matrix[row * cols + col] = value;
                col++;
            }
            if (cols == 0) {
                cols = col;
            }
            row++;
            col = 0;
        }
        rows = row;
        file.close();
    } else {
        std::cout << "UNABLE TO OPEN FILE: " << filename << std::endl;
		errf = 1;
    }
}


//Функция для вывода матрицы
void printMatrix(const double matrix[], int rows, int cols) {
    if((rows == 0) || (cols == 0)) {
		std::cout << "ENTERED EMPTY MATRIX" << std::endl;
		errm = 1;
	} else{
		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < cols; j++) {
				std::cout << matrix[i * cols + j] << " ";
			}
			std::cout << std::endl;
		}
	}
}


// Метод Гаусса
void solveLinearSystem(double matrix[], double solution[], int size) {
    //Прямой ход метода Гаусса
    for (int k = 0; k < size; k++) {
        //Поиск максимального элемента в столбце k
        int maxRow = k;
        for (int i = k + 1; i < size; i++) {
            if (std::abs(matrix[i * (size + 1) + k]) > std::abs(matrix[maxRow * (size + 1) + k])) {
                maxRow = i;
            }
        }

        //Перестановка строк
        if (maxRow != k) {
            for (int j = k; j <= size; j++) {
                std::swap(matrix[k * (size + 1) + j], matrix[maxRow * (size + 1) + j]);
            }
        }

        //Приведение матрицы к треугольному виду
        for (int i = k + 1; i < size; i++) {
            double factor = matrix[i * (size + 1) + k] / matrix[k * (size + 1) + k];
            for (int j = k; j <= size; j++) {
                matrix[i * (size + 1) + j] -= factor * matrix[k * (size + 1) + j];
            }
        }
    }

    //Обратный ход метода Гаусса
    for (int i = size - 1; i >= 0; i--) {
        solution[i] = matrix[i * (size + 1) + size];
        for (int j = i + 1; j < size; j++) {
            solution[i] -= matrix[i * (size + 1) + j] * solution[j];
        }
        solution[i] /= matrix[i * (size + 1) + i];
    }
	printMatrix(solution, size, 1);
}


int main() {
	
	    clock_t start_time, end_time;
    double execution_time;
	//Задаем файл
    std::string filename = "matrix.txt";
    double matrix[MAX_SIZE * (MAX_SIZE+1)];
	double solution[MAX_SIZE];
    int rows = 0;
    int cols = 0;

    //Чтение матрицы из файла
    readMatrixFromFile(filename, matrix, rows, cols);

	if (errf == 0) {
		//Вывод исходной матрицы
		std::cout << "A|B original extended matrix:" << std::endl;
		printMatrix(matrix, rows, cols);

		int size = rows;
		
		if (errm == 0) {
		//Решение системы линейных алгебраических уравнений
		std::cout << "X solution matrix:" << std::endl;
		
				start_time = clock();
		
		solveLinearSystem(matrix, solution, size);
		
		
				end_time = clock();
		execution_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

		printf("Время выполнения программы: %f секунд\n", execution_time);
		} 
	}

    return 0;
}

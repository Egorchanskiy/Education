#include <iostream>
#include <fstream>
#include <sstream>
#include <time.h>

//Массив массивов + статическое выделение

const int MAX_SIZE = 100; //Максимальный размер матрицы


//Своеобразные errno
int errf = 0;
int errm = 0;


//Функция для чтения матрицы из текстового файла
void readMatrixFromFile(const std::string& filename, double matrix[MAX_SIZE][MAX_SIZE+1], int& size) {
    std::ifstream file(filename);

    if (file.is_open()) {
        std::string line;
        int row = 0;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            int col = 0;
            double value;
            while (iss >> value) {
                matrix[row][col] = value;
                col++;
            }
            row++;
        }
        size = row; //Запоминаем размер матрицы
        file.close();
    } else {
        std::cout << "UNABLE TO OPEN FILE: " << filename << std::endl;
		errf = 1;
    }
}

//Функция для вывода матрицы
void printMatrix(double matrix[MAX_SIZE][MAX_SIZE+1], int row, int col) { 
	if(row == 0) {
		std::cout << "ENTERED EMPTY MATRIX" << std::endl;
		errm = 1;
	} else {		
		for (int i = 0; i < row; i++) {
			for (int j = 0; j < col; j++) {
				std::cout << matrix[i][j] << " ";
			}
			std::cout << std::endl;
		}
	}
}


void solveLinearSystem(double matrix[][MAX_SIZE + 1], double solution[MAX_SIZE], int size) {
    //Прямой ход метода Гаусса
    for (int k = 0; k < size; k++) {
        //Поиск максимального элемента в столбце k
        int maxRow = k;
        for (int i = k + 1; i < size; i++) {
            if (std::abs(matrix[i][k]) > std::abs(matrix[maxRow][k])) {
                maxRow = i;
            }
        }

        //Перестановка строк
        if (maxRow != k) {
            for (int j = k; j <= size; j++) {
                std::swap(matrix[k][j], matrix[maxRow][j]);
            }
        }

        //Приведение матрицы к треугольному виду
        for (int i = k + 1; i < size; i++) {
            double factor = matrix[i][k] / matrix[k][k];
            for (int j = k; j <= size; j++) {
                matrix[i][j] -= factor * matrix[k][j];
            }
        }
    }

    //Обратный ход метода Гаусса
    for (int i = size - 1; i >= 0; i--) {
        solution[i] = matrix[i][size];
        for (int j = i + 1; j < size; j++) {
            solution[i] -= matrix[i][j] * solution[j];
        }
        solution[i] /= matrix[i][i];
    }
}



int main() {
		    clock_t start_time, end_time;
    double execution_time;
	
    std::string filename = "matrix.txt";
    double matrix[MAX_SIZE][MAX_SIZE+1];
	double solution[MAX_SIZE];
	int size = 0;

    //Чтение матрицы из файла
    readMatrixFromFile(filename, matrix, size);


	if (errf == 0) {
		//Вывод исходной матрицы
		std::cout << "A|B original extended matrix:" << std::endl;
		printMatrix(matrix, size, size + 1);
		
		if (errm == 0) {
			//Решение системы линейных алгебраических уравнений
							start_time = clock();
			
			solveLinearSystem(matrix, solution, size);
			
			
							end_time = clock();
		execution_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

		printf("Время выполнения программы: %f секунд\n", execution_time);
			
			
			//Вывод решения
			std::cout << "X solution matrix:" << std::endl;
			    for (int i = 0; i < size; i++) {
					std::cout << solution[i] << std::endl;
				}
		}
	} 



    return 0;
}

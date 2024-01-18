#include <iostream>
#include <numeric>
#include <vector>
#include <time.h>

using namespace std;

//Метод контурных токов (МКТ)


//Решение СЛАУ
vector<double> solveLinearSystem(vector<vector<double>>& A, vector<double>& b) {
    int n = A.size();

    //Прямой ход метода Гаусса
    for (int k = 0; k < n - 1; k++) {
        for (int i = k + 1; i < n; i++) {
            double factor = A[i][k] / A[k][k];
            for (int j = k + 1; j < n; j++) {
                A[i][j] -= factor * A[k][j];
            }
            b[i] -= factor * b[k];
        }
    }

    //Обратный ход метода Гаусса
    vector<double> x(n);
    x[n - 1] = b[n - 1] / A[n - 1][n - 1];
    for (int i = n - 2; i >= 0; i--) {
        double sum = 0.0;
        for (int j = i + 1; j < n; j++) {
            sum += A[i][j] * x[j];
        }
        x[i] = (b[i] - sum) / A[i][i];
    }

    return x;
}

//contour_currents реализует МКТ, функция принимает два параметра: adj_matrix, и loop_EMF
//adj_matrix - матрица смежности цепи, loop_EMF - вектор контурных ЭДС

vector<double> contour_currents(vector<vector<double>> adj_matrix,
                                vector<double> loop_EMF) {
    int num_nodes = adj_matrix.size();	//Число узлов
    int num_loops = num_nodes - 1;  //Число уравнений (контуров) = число узлов - 1

    //Формирование системы уравнений
    vector<vector<double>> A(num_loops, vector<double>(num_loops));

//Заполнение матрицы коэффициентов A

    //если все контурные токи направить по (или против) часовой то все
    //недиагональные элементы будут взяты с отрицательным знаком т.к. через
    //контур i ток будет течь в прямом направлении а через контур j в обратном

    for (int i = 0; i < num_loops; i++) {
        for (int j = 0; j < num_loops; j++) {
            if (i == j) {
                A[i][j] =
                    accumulate(adj_matrix[i].begin(), adj_matrix[i].end(), 0.0);
            } else {
                A[i][j] = -adj_matrix[i][j];
            }
        }
    }

    //Решение системы уравнений
    vector<double> loop_currents(num_loops);
    loop_currents = solveLinearSystem(A, loop_EMF);

    return loop_currents;
}

int main() {
	clock_t start_time, end_time;
    double execution_time;
	
	
	
    //Матрица смежности для цепи в виде вектора векторов
    //Элемент (i, j) отвечает сопротивлению между i-ым и j-ым узлом
    vector<vector<double>> adj_matrix = {
        {0, 1, 0, 1},
		{1, 0, 1, 0},
		{0, 1, 0, 1},
		{1, 0, 1, 0}}; //size-узловая цепь с соответсвующими сопротивлениями между узлами

    //Вектор контурных ЭДС (знак ЭДС учтен для выбранного
    //(по/против часовой) направления контуров)
    vector<double> loop_EMF(adj_matrix.size() - 1);
    loop_EMF = {7, -4, 8};  //Вектор контурных ЭДС для каждого контура в цепи

try {
    // Проверка на пустой вектор adj_matrix
    if (adj_matrix.empty()) {
        throw runtime_error("Adjacency matrix is empty.");
    }

    // Проверка на пустой вектор loop_EMF
    if (loop_EMF.empty()) {
        throw runtime_error("Loop EMF vector is empty.");
    }

    // Векторы не пусты, продолжаем выполнение кода

    // Ваш код для расчета контурных токов и вывода результатов

} catch (const exception& e) {
    cout << "Error: " << e.what() << endl;
}

							start_time = clock();

    //Непосредственно расчет контурных токов
    vector<double> result = contour_currents(adj_matrix, loop_EMF);
	
								end_time = clock();
		execution_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

		printf("Время выполнения программы: %f секунд\n", execution_time);
			
	
//ВЫВОД	
	
	//Вывод матрицы смежности adj_matrix электрической цепи
    cout << "Adjacency matrix: " << endl;	
    for (vector<double> node : adj_matrix) {
        for (double i : node) {
            cout << i << " ";
        }
        cout << endl;
    }
	
	//Вывод вектора контурных ЭДС loop_EMF электрической цепи 
    cout << "Vector of contour EMF: " << endl; 
    for (double volt : loop_EMF) {
        cout << volt << " ";
    }
    cout << endl;

	//Вывод контурных токов электрической цепи
    cout << "Loop currents:" << endl; 
    for (double i : result) {
        cout << i << " ";
    }
    cout << endl;

    //Расчет и вывод всех токов в цепи
    cout << "All currents:" << endl;
    for (int i = 0; i < result.size(); i++) {
        cout << "I_" << i << i << " = " << result[i] << endl;
    }
    for (int i = 0; i < result.size(); i++) {
        for (int j = 0; j < i; j++) {
            cout << "I_" << j << i << " = " << result[i] - result[j] << endl;
        }
    }

    return 0;
}
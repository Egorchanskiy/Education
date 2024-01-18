#include <iostream>
#include <numeric>
#include <vector>
#include <time.h>

using namespace std;

//����� ��������� ����� (���)


//������� ����
vector<double> solveLinearSystem(vector<vector<double>>& A, vector<double>& b) {
    int n = A.size();

    //������ ��� ������ ������
    for (int k = 0; k < n - 1; k++) {
        for (int i = k + 1; i < n; i++) {
            double factor = A[i][k] / A[k][k];
            for (int j = k + 1; j < n; j++) {
                A[i][j] -= factor * A[k][j];
            }
            b[i] -= factor * b[k];
        }
    }

    //�������� ��� ������ ������
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

//contour_currents ��������� ���, ������� ��������� ��� ���������: adj_matrix, � loop_EMF
//adj_matrix - ������� ��������� ����, loop_EMF - ������ ��������� ���

vector<double> contour_currents(vector<vector<double>> adj_matrix,
                                vector<double> loop_EMF) {
    int num_nodes = adj_matrix.size();	//����� �����
    int num_loops = num_nodes - 1;  //����� ��������� (��������) = ����� ����� - 1

    //������������ ������� ���������
    vector<vector<double>> A(num_loops, vector<double>(num_loops));

//���������� ������� ������������� A

    //���� ��� ��������� ���� ��������� �� (��� ������) ������� �� ���
    //�������������� �������� ����� ����� � ������������� ������ �.�. �����
    //������ i ��� ����� ���� � ������ ����������� � ����� ������ j � ��������

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

    //������� ������� ���������
    vector<double> loop_currents(num_loops);
    loop_currents = solveLinearSystem(A, loop_EMF);

    return loop_currents;
}

int main() {
	clock_t start_time, end_time;
    double execution_time;
	
	
	
    //������� ��������� ��� ���� � ���� ������� ��������
    //������� (i, j) �������� ������������� ����� i-�� � j-�� �����
    vector<vector<double>> adj_matrix = {
        {0, 1, 0, 1},
		{1, 0, 1, 0},
		{0, 1, 0, 1},
		{1, 0, 1, 0}}; //size-������� ���� � ��������������� ��������������� ����� ������

    //������ ��������� ��� (���� ��� ����� ��� ����������
    //(��/������ �������) ����������� ��������)
    vector<double> loop_EMF(adj_matrix.size() - 1);
    loop_EMF = {7, -4, 8};  //������ ��������� ��� ��� ������� ������� � ����

try {
    // �������� �� ������ ������ adj_matrix
    if (adj_matrix.empty()) {
        throw runtime_error("Adjacency matrix is empty.");
    }

    // �������� �� ������ ������ loop_EMF
    if (loop_EMF.empty()) {
        throw runtime_error("Loop EMF vector is empty.");
    }

    // ������� �� �����, ���������� ���������� ����

    // ��� ��� ��� ������� ��������� ����� � ������ �����������

} catch (const exception& e) {
    cout << "Error: " << e.what() << endl;
}

							start_time = clock();

    //��������������� ������ ��������� �����
    vector<double> result = contour_currents(adj_matrix, loop_EMF);
	
								end_time = clock();
		execution_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

		printf("����� ���������� ���������: %f ������\n", execution_time);
			
	
//�����	
	
	//����� ������� ��������� adj_matrix ������������� ����
    cout << "Adjacency matrix: " << endl;	
    for (vector<double> node : adj_matrix) {
        for (double i : node) {
            cout << i << " ";
        }
        cout << endl;
    }
	
	//����� ������� ��������� ��� loop_EMF ������������� ���� 
    cout << "Vector of contour EMF: " << endl; 
    for (double volt : loop_EMF) {
        cout << volt << " ";
    }
    cout << endl;

	//����� ��������� ����� ������������� ����
    cout << "Loop currents:" << endl; 
    for (double i : result) {
        cout << i << " ";
    }
    cout << endl;

    //������ � ����� ���� ����� � ����
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
#include "stdafx.h"
#include<iostream>
#include <Windows.h>
#include <omp.h>
#include <iomanip>
#include <ctime>

using namespace std;

//Вывести параллельно “HelloWorld” 5 - 10 раз
void task1();
/*Исследовать влияние настроек параметра shedule (не менее 4 опций ) парадигмы
#omp parallel for. Вывести время выполнения циклов для:
 Итераций с равномерным временем выполнения;
 Итераций с неравномерным временем выполнени
я – нечетные итерации
выполняются дольше.
 Итераций с неравномерным временем выполнения –итерации в первой половине
цикла выполняются дольше.
Результат вывести в виде таблице (4 х 3). Объяснить результат.*/
void task2();
/*Провести суммирование элементов массива с использованием критических секций
(critical) и параметра reduction. Сравнить время работы, результат объяснить.*/
void task3();
//Придумать пример на барьерную синхронизацию #pragma omp barrier
void task4();
/*Распараллелить цикл:
For(i=2;i&lt;N;i++)
For(j=2;i&lt;N;j++)
A[i,j] =A[i-2,j] +A[i,j-2];*/
void task5();

int main()
{
	setlocale(LC_ALL, "rus");
	//cout << "Первое задание:\n"; task1();
	//cout << "Второе задание:\n"; task2();
	//cout << "Третье задание:\n"; task3();
	//cout << "Четвёртое задание:\n"; task4();
	cout << "Пятое задание:\n"; task5();

	cin.get();
}

void task1() {
#pragma omp parallel num_threads(10)
	{ cout << "Hello\n"; }
	printf("\n");
}

void somecalculate()
{
	double x = 999.542, y = 0.77;
	for (int i = 0; i < 100000; i++)
	{
		x *= y;
	}
}

void iter(int t, int i, int N) {
	if (t == 0) {
		somecalculate();
	}
	if (t == 1) {
		if (i % 2 == 0) somecalculate();
	}
	if (t == 2) {
		if (i < N / 2) somecalculate();
	}
}

void leftside(int t) {
	if (t == 0) cout << "равномерно            ";
	else if (t == 1) cout << "нечетные дольше       ";
	else cout << "первая половина дольше ";
}



void task2() {
	cout << "                       "
		<< setw(10) << "static  "
		<< setw(10) << "dynamic  "
		<< setw(10) << "dynamic 3  "
		<< setw(10) << "guided  "
		<< endl;

	for (int i = 0; i < 3; i++) {
		int k = 100;
		double start_time, end_time;
		leftside(i);

		start_time = omp_get_wtime();
#pragma omp for schedule(static) // ключевое слово static. Программист может контролировать то, каким образом потоки будут загружаться работой при обработке цикла.
		//Существует несколько вариантов.
		for (int n = 0; n < k; n++) iter(i, n, k);
		end_time = omp_get_wtime();;
		cout << setw(6) << end_time - start_time;

		start_time = omp_get_wtime();
#pragma omp for schedule(dynamic) //ключевое слово dynamic. В данном случае невозможно предсказать порядок, в котором итерации цикла будут назначены потокам.
		//Каждый поток выполняет указанное число итераций. Если это число не задано, по умолчанию оно равно 1. 
		for (int n = 0; n < k; n++) iter(i, n, k);
		end_time = omp_get_wtime();;
		cout << setw(10) << end_time - start_time;

		start_time = omp_get_wtime();
#pragma omp for schedule(dynamic, 3) //В данном примере, каждый поток выполняет три итерации, затем «берёт» следующие три и так далее.
		//Последние, конечно, могут иметь размер меньше трёх.

		for (int n = 0; n < k; n++) iter(i, n, k);
		end_time = omp_get_wtime();;
		cout << setw(10) << end_time - start_time;

		start_time = omp_get_wtime();
#pragma omp for schedule(guided) //Он похож на dynamic, но размер порции уменьшается экспоненциально.
		for (int n = 0; n < k; n++) iter(i, n, k);
		end_time = omp_get_wtime();;
		cout << setw(10) << end_time - start_time;

		printf("\n\n");
	}
}

void task3() {
	long k = 1000;
	long* N = new long[k];
	double start_time, end_time;
	long sum = 0;
	for (int i = 0; i < k; i++) {
		N[i] = i;
	}



	start_time = omp_get_wtime();

#pragma omp parallel for
	for (int i = 0; i < k; i += 1)
#pragma omp critical
	{
		sum += N[i];
	}
	end_time = omp_get_wtime();
	cout << "Результат " << sum << " " << "Время " << end_time - start_time << endl;

	sum = 0;
	start_time = omp_get_wtime();
#pragma omp parallel for reduction(+:sum) //суммирование с параметром reduction
	for (intptr_t i = 0; i < k; i++) sum += N[i];
	end_time = omp_get_wtime();
	cout << "Результат " << sum << " " << "Время " << end_time - start_time << endl;

}

void task4() {
#pragma omp parallel
	{
		printf("Сообщение    1\n");
		printf("Сообщение    2\n");
#pragma omp barrier
		printf("Сообщение    3\n");
	}
}

void paralltask5(int** arr, int count1, int s1, int s2) {
#pragma omp parallel for num_threads(4) //создаётся 4 потока
	for (int i = 0; i < 4; ++i) {
		s1 = int(i / 2);
		s2 = int(i % 2);
		for (int j = s1 + 2; j < count1; j += 2) {
			for (int k = s2 + 2; k < count1; k += 2) {
				arr[j][k] = arr[j][k - 2] + arr[j - 2][k]; //параллельно вычисляются 4 значения 
			}
		}
	}
}

void usualtask5(int** arr, int count1) {
	for (int i = 2; i < count1; i++)
		for (int j = 2; j < count1; j++)
			arr[i][j] = arr[i - 2][j] + arr[i][j - 2];
}

void checkmethodtask5(int** arr, int count) {
	for (int i = 0; i < count; i++) {
		for (int j = 0; j < count; j++)
			printf("%4d", arr[i][j]);
		printf("\n");
	}
}
void zeroizetask5(int** arr, int count) {
	for (int i = 0; i < count; i++) {
		arr[i] = new int[count];
		for (int j = 0; j < count; j++) {
			arr[i][j] = i + j;
		}
	}
}

void task5() {
	int count, s1 = 0, s2 = 0;
	double start, end, finaltime, finaltime2;
	cout << "введите кол-во элементов массива: ";
	cin >> count;
	printf("\n\n");
	int **arr = new int*[count];

	zeroizetask5(arr, count);

	start = omp_get_wtime();
	paralltask5(arr, count, s1, s2);
	end = omp_get_wtime();
	finaltime = end - start;
	cout << "время паралл: " << finaltime << endl;

	//checkmethodtask5(arr, count);
	zeroizetask5(arr, count);

	start = omp_get_wtime();
	usualtask5(arr, count);
	end = omp_get_wtime();
	finaltime2 = end - start;
	cout << "время обычн: " << finaltime2 << endl;
	//checkmethodtask5(arr, count);
	cout << "соотношение: " << finaltime2 / finaltime << endl;

	cin.get();
}
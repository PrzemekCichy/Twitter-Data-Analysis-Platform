//// TwitterPLatform.cpp : Defines the entry point for the console application.
////
//


#include "stdafx.h"

#include "amp.h"

#include "stdafx.cpp"

using namespace concurrency;

/* Remove if already defined */
typedef long long int64;
typedef unsigned long long uint64;

/* Returns the amount of milliseconds elapsed since the UNIX epoch. Works on both
* windows and linux. */
uint64 GetTimeMs64() {
	FILETIME ft;
	LARGE_INTEGER li;

	/* Get the amount of 100 nano seconds intervals elapsed since January 1, 1601 (UTC) and copy it
	* to a LARGE_INTEGER structure. */
	GetSystemTimeAsFileTime(&ft);
	li.LowPart = ft.dwLowDateTime;
	li.HighPart = ft.dwHighDateTime;

	uint64 ret = li.QuadPart;
	ret -= 116444736000000000LL; /* Convert from file time to UNIX epoch time. */
	ret /= 10000; /* From 100 nano seconds (10^-7) to 1 millisecond (10^-3) intervals */

	return ret;
}

void readCharInFile(char* fileName) {
	uint64 start = GetTimeMs64();

	std::ifstream ifs;

	ifs.open(fileName, std::ifstream::in);

	char c = ifs.get();

	while (ifs.good()) {
		c = ifs.get();
	}

	ifs.close();
	uint64 stop = GetTimeMs64();
	std::cout << "Read " << fileName << " in " << (double)(stop - start) / 1000 << "s." << endl;
	//return 0;
}

void readLineInFile(char* fileName) {
	uint64 start = GetTimeMs64();

	std::ifstream ifs;
	ifs.open(fileName, std::ifstream::in);

	//This might have to be converted to dynamic allocations....
	const int no_of_lines = 4500;
	const int line_size = 2500;

	string line[no_of_lines];
	char line_content[line_size];
	int i = 0;
	while (ifs.good()) {
		ifs.getline(line_content, line_size);
		line[i++] = line_content;
	}
	cout << i << endl;
	cout << "line 0" << line[0] << endl;

	ifs.close();
	uint64 stop = GetTimeMs64();
	std::cout << "Read " << fileName << " in " << (double)(stop - start) / 1000 << "s." << endl;
	//return 0;
}

void ReadFileWithGPU(int *v, int size, int to_add) {
	//alias for original data
	array_view<int, 1> av(size, v);

	parallel_for_each(av.extent, [=](index<1> idx) restrict(amp) {
		av[idx] += to_add;
	});
	av.synchronize();

	//Need to wait untill the above is completed due to parralel structure

	for (int i = 0; i < size; i++) {
		cout << static_cast<char>(v[i]);
	}
}

accelerator pick_accelerator() {
	//THe default picked at runtime
	accelerator chosen_acc;

	//querry all accelerators and pick one based on your criteria
	for (accelerator acc : accelerator::get_all()) {
		if (!acc.has_display)
			chosen_acc = acc;
	}
	return chosen_acc;
}

void do_it(int *v, int size, int to_add) {
	//number of times, lambda(capture list([=] capture by value), signature, body)
	//Need to change to 1d array

	//extent<1> e(size);
	accelerator ac = pick_accelerator();
	//alias for original data
	array_view<int, 1> av(size, v);

	parallel_for_each(av.extent, [=](index<1> idx) restrict(amp) {
		//readLineInFile("");
	});
	av.synchronize();

	//Need to wait untill the above is completed due to parralel structure

	for (int i = 0; i < size; i++) {
		cout << static_cast<char>(v[i]);
	}
}

void main() {
	uint64 start = GetTimeMs64();
	const int size = 12;
	int v[] = { 'G', 'd', 'k', 'k', 'n', 31, 'v' ,'n', 'q', 'k','c','-' };
	int to_add = 1;
	//do_it(v, size, to_add);
	//SSD vs HDD
	// Read char  is 5x slower than line reading
	/*cout << "Read Char in file:" << endl;
	readCharInFile("C:\\Users\\Przemek\\Desktop\\Project\\30.json");*/

	cout << "Read Line in File" << endl;
	readLineInFile("C:\\Users\\Przemek\\Desktop\\Project\\30.json");

	uint64 stop = GetTimeMs64();
	std::cout << "total time:" << (double)(stop - start) / 1000 << "s." << endl;
}


//
//const int array_size = 3;
//
//accelerator_view setup() {
//	accelerator gpu(accelerator::default_accelerator);
//	accelerator::direct3d_ref;
//	accelerator::direct3d_warp;
//
//	std::vector<accelerator> all = accelerator::get_all();
//
//	for (accelerator&a : all)
//	{
//		wcout << a.description;
//		cout << " has " << a.dedicated_memory / 1e6 << " GB of memory." << endl;
//	}
//
//	return gpu.default_view;
//}
//
//void add_in_amp(accelerator_view acc_view) {
//	int ah[] = { 1, 2, 3 };
//	int bh[] = { 5, 7, 9 };
//	int sumh[array_size];
//
//	array_view<const int, 1> ad(array_size, ah);
//	array_view<const int, 1> bd(array_size, bh);
//	array_view<int, 1> sumd(array_size, sumh);
//
//	parallel_for_each(acc_view, sumd.extent, [=](index<1> idx) restrict(amp) {
//		int a = ad[idx];
//		int b = bd[idx];
//		sumd[idx] = a + b;
//	});
//
//	sumd.synchronize();
//
//	for (size_t i = 0; i < array_size; i++)
//	{
//		std::cout << sumh[i] << "\t";
//	}
//	std::cout << std::endl;
//}
//
//void add_in_cpp() {
//	int ah[] = { 1, 2, 3 };
//	int bh[] = { 5, 7, 9 };
//	int sumh[array_size];
//
//	for (size_t i = 0; i < array_size; i++)
//	{
//		sumh[i] = ah[i] + bh[i];
//	}
//
//	for (size_t i = 0; i < array_size; i++)
//	{
//		std::cout << sumh[i] << std::endl;
//	}
//}
//
//string print_matrix(const float* mtx, const int dim)
//{
//	ostringstream oss;
//	oss << "\n";
//	for (int i = 0; i < dim; i++)
//	{
//		oss << "[";
//		for (int j = 0; j < dim; j++)
//		{
//			oss << mtx[i *dim + j];
//			if (j + 1 < dim) {
//				oss << ".\t";
//			}
//		}
//		return oss.str();
//
//	}
//}
//
//template<int ts>
//void tiled_multiply(accelerator_view acc_view, float* a, float* b, float* c, const int dim) {
//	array_view<float, 2> av(dim, dim, a);
//	array_view<float, 2> bv(dim, dim, b);
//	array_view<float, 2> cv(dim, dim, c);
//	cv.discard_data();
//
//	parallel_for_each(cv.extent.tile<ts, ts>, [=](tiled_index<ts, ts> idx) restrict(amp) {
//	
//		tile_static float al[ts][ts];
//		tile_static float bl[ts][ts];
//
//		//int rl = idx.local[0];
//		//int cl = idx.local[1];
//		//int rg = idx.global[0];
//		//int cg = idx.global[1];
//
//		//auto sum = 0.f;
//
//		//for (int i = 0; i < dim; i += ts)
//		//{
//		//	al[rl][cl] = av(rg, cl + i);
//		//	bl[rl][cl] = bv(rl + i, cg);
//
//		//	idx.barrier.wait();
//
//		//	for (int j = 0; j < ts; ++j)
//		//	{
//		//		sum += al[rl][j] * bl[j][cl];
//		//	}
//		//	idx.barrier.wait();
//		//}
//
//		//cv[idx.global] = sum;
//	});
//	cv.synchronize();
//}
//
//
//void matrix_multiplication(accelerator_view acc_view) {
//	const int dim = 4;
//	float a[dim*dim];
//	float b[dim*dim];
//	float c[dim*dim];
//
//	for (int i = 0; i < dim; i++)
//	{
//		for (size_t j = 0; j < dim; j++)
//		{
//			a[i * dim + i] = i * dim + i;
//			b[i * dim + j] = j * dim + j;
//		}
//	}
//	tiled_multiply<2>(acc_view, a, b, c, dim);
//
//	cout << "The product of " << print_matrix(a, dim) << " and " <<
//		print_matrix(b, dim) << " is " << print_matrix(c, dim) << endl;
//}
//
//
//int main()
//{
//	// h = host, d = device
//	accelerator_view acc_view = setup();
//	add_in_amp(acc_view);
//
//	//add_in_cpp();
//	matrix_multiplication(acc_view);
//	getchar();
//	return 0;
//}
//

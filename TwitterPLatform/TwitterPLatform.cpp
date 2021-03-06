/// TwitterPLatform.cpp : Defines the entry point for the console application.
/// Honestly this file could use some clean up at later stages.
/// Some separation of concerns could also be nice, eg, aho corasic search, file reading, unzip etc.
///

#include "stdafx.h"

// Returns the amount of milliseconds elapsed since the UNIX epoch. 
uint64 get_epoch_time() {
	FILETIME ft;
	LARGE_INTEGER li;

	GetSystemTimeAsFileTime(&ft);
	li.LowPart = ft.dwLowDateTime;
	li.HighPart = ft.dwHighDateTime;

	uint64 ret = li.QuadPart;
	ret -= 116444736000000000LL; /* Convert from file time to UNIX epoch time. */
	ret /= 10000; /* From 100 nano seconds (10^-7) to 1 millisecond (10^-3) intervals */

	return ret;
}

/*
* Reads and returns file content
* no_of_lines & line_size might have to be converted to dynamic allocations....
* This will give small performance improvement?
* As well as less memmory used which is important for parralel application
*/
std::vector<std::string > readFile(char* fileName) {
	uint64 start = get_epoch_time();

	std::ifstream ifs;
	ifs.open(fileName, std::ifstream::in);
	std::stringstream buffer;

	int i = 0;
	const int no_of_lines = 10000;
	const int line_size = 499999;
	char line_content[line_size];
	std::vector<std::string > arr;

	//We need to check whether the line is empty unless this does not bother us?
	//Probably not an issue at this point 
	while (ifs.good()) {
		ifs.getline(line_content, line_size);
		arr.push_back(line_content);
	}
	ifs.close();

	arr.shrink_to_fit();
	std::cout << "Read " << fileName << " in " << (double)(get_epoch_time() - start) / 1000 << "s." << std::endl;

	return arr;
}

//Returns folders and files inside a directory
std::vector<std::experimental::filesystem::v1::directory_entry> getFiles(char* directory) {

	/*std::string path = "path_to_directory";*/
	std::vector<std::experimental::filesystem::v1::directory_entry> paths;
	std::vector<std::experimental::filesystem::v1::path> lol;
	for (auto & p : fs::recursive_directory_iterator(directory)) {
		lol.push_back(p.path());
		//std::cout << p << std::endl;
	}

	return paths;
}

std::string search(std::string line, std::vector<std::string> list_of_keywords) {
	std::string ret;
	std::string::size_type n;
	std::for_each(list_of_keywords.begin(), list_of_keywords.end(), [&](std::string item) {
		// find the first occurrence match sub-string.
		n = line.find(item);
		if (n != std::string::npos) {
			std::cout << n << line << std::endl;
		}
	});
	return "Finished file filter";
}

class FilesList
{
public:
	FilesList() {}
	std::vector<std::string> standard;
	std::vector<std::string> compressed;
	long TotalFound;
	long TotalFiles;
};

extern "C++" __declspec(dllexport) FilesList Start(char* path) {
	uint64 start = get_epoch_time();
	FilesList filesList;
	std::cout << "Read files from directory:" << std::endl;

	for (auto & p : fs::recursive_directory_iterator(path)) {
		if (p.path().extension() == ".json") {
			InterlockedIncrement(&filesList.TotalFound);
			filesList.standard.push_back(p.path().string());
		}
		else {
			if (p.path().extension() == ".bz2") {
				InterlockedIncrement(&filesList.TotalFound);
				filesList.standard.push_back(p.path().string());
			}
		}
		InterlockedIncrement(&filesList.TotalFound);
	}
	std::cout << filesList.TotalFiles << filesList.TotalFound << filesList.compressed.size() << filesList.standard.size() << std::endl;

	std::vector<std::experimental::filesystem::v1::directory_entry> files;// = getFiles(path);

	std::cout << "Total time reading" << (double)(get_epoch_time() - start) / 1000 << "s." << std::endl;

	start = get_epoch_time();
	parallel_for_each(files.begin(), files.end(), [&](auto item) {
		if (item.path().extension() == ".json") {
			InterlockedIncrement(&filesList.TotalFound);
			//filesList.standard.push_back(item.path());
		}
		else {
			if (item.path().extension() == ".bz2") {
				InterlockedIncrement(&filesList.TotalFound);
				//filesList.compressed.push_back(&item);
			}
		}
		InterlockedIncrement(&filesList.TotalFound);

		//std::cout << "Parrallel: " << item << std::endl;
	});
	std::cout << "Total time finding" << (double)(get_epoch_time() - start) / 1000 << "s." << std::endl;
	//std::for_each(files.begin(), files.end(), [](auto&& item)

	return filesList;
}

class CUnmanagedTestClass
{
public:
	CUnmanagedTestClass();
	virtual ~CUnmanagedTestClass();
	void PassInt(int nValue);
	void PassString(char* pchValue);
	char* ReturnString();
};

void main() {
	std::cout << "Initialise retrieval" << std::endl;

	uint64 start = get_epoch_time();
	Start("E:\\Project");
	//do_it(v, size, to_add);
	//SSD vs HDD
	// Read char  is 5x slower than line reading
	//cout << "Read Char in file:" << endl;
	//readCharInFile("C:\\Users\\Przemek\\Desktop\\Project\\30.json");

	std::cout << "Find files in Directory" << std::endl;

	std::vector<std::experimental::filesystem::v1::directory_entry> files = getFiles("E:\\Project");

	std::cout << "Finished reading directory" << std::endl;
	std::cout << "Total time reading:" << (double)(get_epoch_time() - start) / 1000 << "s." << std::endl;

	std::cout << "Read Line in File" << std::endl;

	std::vector<std::string> a = readFile("C:\\Users\\Przemek\\Desktop\\Project\\30.json");

	uint64 start_sequential = get_epoch_time();
	LONG matched = 0;

	std::cout << "Find files with .json extension" << std::endl;
	//for (int i = 0; i <= files.size() - 1; i++) {
	//	if (files[i].path().extension() == ".json" || files[i].path().extension() == ".bz2")
	//		matched++;
	//	//std::cout << "Sequential" << files[i].path() << std::endl;
	//}
	//std::cout << "Matched: " << matched << std::endl;
	//uint64 stop_sequential = get_epoch_time();

	uint64 start_parralel = get_epoch_time();
	parallel_for_each(files.begin(), files.end(), [&](auto item) {
		if (item.path().extension() == ".json" || item.path().extension() == ".bz2")
			InterlockedIncrement(&matched);
		//std::cout << "Parrallel: " << item << std::endl;
	});
	std::cout << "Matched: " << matched << std::endl;
	//std::for_each(files.begin(), files.end(), [](auto&& item)
	//{
	//	if (item.path().extension() == ".json")
	//		std::cout << "Parrallel: " << item << std::endl;
	//});
	std::cout << "Total time parallel" << (double)(get_epoch_time() - start_parralel) / 1000 << "s." << std::endl;

	//std::cout << "Total time sequential:" << (double)(stop_sequential - start_sequential) / 1000 << "s." << std::endl;

	std::cout << "Front:" << a.front() << std::endl;
	std::cout << "End:" << a.back() << std::endl;

	//readFileSlow("C:\\Users\\Przemek\\Desktop\\Project\\30.json");

	std::cout << "Finished reading files" << std::endl;

	//Find cpu string in lines
	std::vector<std::string> keywords = { "trump", "hillary" };
	parallel_for_each(a.begin(), a.end(), [&](auto item) {
		search(item, keywords);
		//std::cout << "Parrallel: " << item << std::endl;
	});
	uint64 stop = get_epoch_time();

	std::cout << "total time:" << (double)(stop - start) / 1000 << "s." << std::endl;
}


//C++ AMP shenanigans

void filterFile(int *v, int size, int to_add) {
	//alias for original data
	array_view<int, 1> av(size, v);

	parallel_for_each(av.extent, [=](index<1> idx) restrict(amp) {
		av[idx] += to_add;
	});
	av.synchronize();

	//Need to wait untill the above is completed due to parralel structure

	for (int i = 0; i < size; i++) {
		std::cout << static_cast<char>(v[i]);
	}
}

accelerator pickAccelerator() {
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
	accelerator ac = pickAccelerator();
	//alias for original data
	array_view<int, 1> av(size, v);

	parallel_for_each(av.extent, [=](index<1> idx) restrict(amp) {
		//readLineInFile("");
	});
	av.synchronize();

	//Need to wait untill the above is completed due to parralel structure

	for (int i = 0; i < size; i++) {
		std::cout << static_cast<char>(v[i]);
	}
}






//
//
//void readCharInFile(char* fileName) {
//	uint64 start = GetTimeMs64();
//
//	std::ifstream ifs;
//
//	ifs.open(fileName, std::ifstream::in);
//
//	char c = ifs.get();
//
//	while (ifs.good()) {
//		c = ifs.get();
//	}
//
//	ifs.close();
//
//	uint64 stop = GetTimeMs64();
//	std::cout << "Read " << fileName << " in " << (double)(stop - start) / 1000 << "s." << endl;
//}
//
//
//
//
//void readFileSlow(char* fileName) {
//	uint64 start = GetTimeMs64();
//
//	std::ifstream ifs(fileName);
//	std::vector<std::string> lines;
//
//	std::string line;
//	while (std::getline(ifs, line))
//	{
//		// skip empty lines:
//		if (line.empty())
//			continue;
//
//		lines.push_back(line);
//	}
//
//	ifs.close();
//	uint64 stop = GetTimeMs64();
//
//	cout << "First line" << lines.front() << endl;
//	cout << "Last line" << lines.back() << endl;
//	std::cout << "Read " << fileName << " in " << (double)(stop - start) / 1000 << "s." << endl;
//}
//



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

#ifdef _MSC_VER
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif //!_CRT_SECURE_NO_WARNINGS
#endif //_MSC_VER


#include <cstdlib>
#include <cmath>
#include <ctime>

#include <iostream>
#include <algorithm>
#include <fstream>
#include <string>
#include <vector>
#include <tuple>
#include <thread>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <highgui.h>

#include <hw2\tool.hpp>

// Start time
std::time_t startTime = std::time(NULL);


int main(int argc, char** argv) {

	using std::tuple;
	using std::get;
	using std::vector;
	using std::cout;
	using std::string;

	string path{ argv[1] };
	int n = std::stoi(argv[2]);



	// prepare files paths
	vector<string> file_names{ path + "sample.bmp" };
	for (auto i = 0; i < n; ++i) {
		file_names.push_back(path + "puzzle" + std::to_string(i + 1) + ".bmp");
	}
	file_names.push_back(path + "target.bmp");
	
	//
	// get kp and des
	//
	auto sift_res = hw2::sift_files(file_names);
	
	//
	// get knn 
	//
	vector<vector<vector<tuple<float, int, int>>>> knns(1);
	for (int i = 1; i < sift_res.size(); ++i) {
		knns.push_back(hw2::findKNN(get<1>(sift_res[i]), get<1>(sift_res[0]), 4, 350));
	}


	vector<std::thread> Ts;
	for (int i = 1; i < sift_res.size(); ++i) {


#define TRY_MANY_TIMES
#if defined(TRY_MANY_TIMES)
		Ts.push_back(std::thread([=]() { // thread
		//	for (int t = 0; t < 500; ++t) { // try many times
#endif


			// find knn from obj to target
			// using descriptor
			// knn with 4 ransac with 4				
				auto H = hw2::ransac(knns[i], get<0>(sift_res[i]), get<0>(sift_res[0]), 4, 500);
				hw2::backward_warping(H, file_names[i], file_names[0], path + std::to_string(i) + "res" + ".bmp");
				std::string mat_save{ path + std::to_string(i) + "res" + ".txt" };
				std::ofstream outfile(mat_save.c_str());
				for (auto i = 0; i < 3; ++i) {
					outfile << "\n\t";
					for (auto j = 0; j < 3; ++j) {
						outfile << H.at<float>(i, j) << '\t';
					}
				}
#if defined(TRY_MANY_TIMES)
			//} // for many times
		})); //thread
#endif

	}

	for (auto& T :Ts) {
		T.join();
	}
	// End time
	cout << "time: " << std::time(NULL) - startTime << " s\n";
	system("PAUSE");
}
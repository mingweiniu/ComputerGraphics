
#include <thread>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iostream>
#include <atomic>
#include <vector>
#include <memory>
#include <utility>

#include <cmath>
#include <cstdlib>
#include <ctime>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>
#include <opencv2/xfeatures2d.hpp>

#include <fin/main.hpp>
#include <fin/tool.hpp>
#include <fin/render.hpp>

auto DEBUG = false;
auto StartTime = std::time(nullptr);

int main(int argc, char **argv)
{
	using std::string;
	using std::to_string;
	using std::stoi;
	using std::vector;
	using std::tuple;
	using std::get;
	using std::thread;
	using std::cout;
	using std::min;

	using cv::Vec3f;
	using cv::Vec3i;

	//
	// read file_one.png and turn it into right and left image
	//
	string flag{ argv[1] };
	string file_one{ argv[2] };
	string file_two{ argv[3] };
	string higher{ argv[4] };
	string smooth{ argv[5] };

	auto Image = fin::LR3fImage(file_one);
	auto Edge = fin::LR1UEdge(file_two, Image);

	//if (flag == "-F") {
	//	fin::Cutting(Image, Edge);
	//}

	auto NormL = fin::norm3fImage(get<0>(Image));
	auto NormR = fin::norm3fImage(get<1>(Image));

	fin::save3fImage(get<0>(Image), "ImgL.bmp");
	fin::save3fImage(get<1>(Image), "ImgR.bmp");
	fin::save3fImage(NormL, "NormL.bmp");
	fin::save3fImage(NormR, "NormR.bmp");


	if (flag == "-C") {
		//
		// Calculate SSD with normalized vector
		//
		//fin::SSD_LR(NormL, NormR, 4);

		//
		// Calculate Correlation and save to file
		//
		cout << "Creating Correlation tables of" + file_one << '\n';
		fin::Correlation(NormL, NormR, get<0>(Edge), get<1>(Edge), 4);
		cout << '\n' << std::time(nullptr) - StartTime << " second is used \n";
		system("PAUSE");
	}

	if( (flag == "-D") || (flag == "-F") ){
		vector<GLfloat> colorVert;
		//
		// Dynamic Programming by Correlation Table
		//
		vector<vector<tuple<Vec3i, Vec3f>>> ImageDepth;
		for (auto r = 0; r < NormL.rows; ++r) {
			string file = "./Correlation/" + to_string(r) + ".txt";
			cout << "reading " << file << '\n';
			auto oneLine = fin::DynamicProgramming(file, r, get<0>(Image), get<1>(Image), 4);
			ImageDepth.push_back(oneLine);
		}

		vector<vector<float>> LeftDepth(NormL.rows, vector<float>(NormL.cols, -1.0f));
		for (auto Depth : ImageDepth) {
			for (auto D : Depth) {
				auto pos = get<0>(D);
				auto col = get<1>(D);
				auto height = min(static_cast<float>(pos[2]) * stoi(higher), 50.0f);
				if (LeftDepth[pos[0]][pos[1]] < height) {
					LeftDepth[pos[0]][pos[1]] = height;
				}
			}
		}
		
		//
		// Gaussian Smooth the result
		//
		int SmoothTimes = stoi(smooth);
		for (auto i = 0; i < SmoothTimes; ++i) {
			fin::GaussianSmooth(LeftDepth);
		}
		//fin::LURD(LeftDepth);

		string save_path = "./output.ply";
		//cout << "writing file to " << save_path << '\n';
		std::ofstream outfile(save_path);
		outfile <<
			"ply\n"
			"format ascii 1.0\n"
			"comment alpha=1.0\n";
		outfile << "element vertex " << LeftDepth.size()  * LeftDepth[0].size() << "\n";
		outfile <<
			"property float x\n"
			"property float y\n"
			"property float z\n"
			"property uchar red\n"
			"property uchar green\n"
			"property uchar blue z\n"
			"end_header\n"
			;

		auto LeftImg = get<0>(Image);
		for (auto h = 0; h < LeftDepth.size(); ++h) {
			for (auto w = 0; w < LeftDepth[0].size(); ++w) {
				if (get<0>(Edge).at<uchar>(h, w) > 200) {

					outfile << h << ' ';
					outfile << w << ' ';
					outfile << static_cast<int>(LeftDepth[h][w]) << ' ';
					outfile << static_cast<int>(LeftImg.at<Vec3f>(h, w)[2] * 255) << ' ';
					outfile << static_cast<int>(LeftImg.at<Vec3f>(h, w)[1] * 255) << ' ';
					outfile << static_cast<int>(LeftImg.at<Vec3f>(h, w)[0] * 255) << '\n';

					colorVert.push_back(static_cast<float>(h * 2) / NormL.cols);
					colorVert.push_back(static_cast<float>(w * 2) / NormL.cols);
					colorVert.push_back(static_cast<float>(LeftDepth[h][w]) / NormL.cols);
					colorVert.push_back(LeftImg.at<Vec3f>(h, w)[2]);
					colorVert.push_back(LeftImg.at<Vec3f>(h, w)[1]);
					colorVert.push_back(LeftImg.at<Vec3f>(h, w)[0]);
				}
			}
		}
		outfile.close();

		cout << '\n' << std::time(nullptr) - StartTime << " second is used \n";

		vector<GLfloat> testVert{
			0.5f, 0.5f, 0.0f,  1.0f, 0.0f, 0.0f,    // Test data a1
			0.5f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,    // Test data a2
			0.0f,  0.0f, 0.0f,  0.0f, 0.0f, 1.0f,   // Test data a3

			-5.0f,  -0.5f, 0.0f,  1.0f, 0.0f, 0.0f, // Test data b1
			-1.0f, 0.0f, 0.0f,  1.0f, 1.0f, 0.0f,   // Test data b2
			0.0f,  0.0f, 0.0f,  1.0f, 0.0f, 1.0f,   // Test data b3
		};

		fin::render show;
		show.vertices = colorVert;
		show.draw();
	}

	//system("pause");

	return 0;
}
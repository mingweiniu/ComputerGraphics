#include "fin/tool.hpp"

#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <thread>
#include <array>
#include <string>
#include <set>
#include <limits>
#include <deque>

#include <cstdlib>
#include <cmath>
#include <ctime>

#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>
#include <opencv2/xfeatures2d.hpp>

void fin::save3fImage(cv::Mat input, std::string path)
{
	using cv::Mat;
	using cv::Vec3f;
	using cv::Vec3b;

	auto Save = Mat(input.rows, input.cols, CV_8UC3);
	for (auto r = 0; r < input.rows; ++r) {
		for (auto c = 0; c < input.cols; ++c) {
			for (auto i = 0; i < 3; ++i) {
				Save.at<Vec3b>(r, c)[i] = static_cast<int>(input.at<Vec3f>(r, c)[i] * 255);
			}
		}
	}
	cv::imwrite(path, Save);
}

std::tuple<cv::Mat, cv::Mat> fin::LR3fImage(std::string imagepath)
{
	using std::make_tuple;

	using cv::Mat;
	using cv::Vec3f;
	using cv::Vec3b;

	Mat Image = cv::imread(imagepath, cv::IMREAD_COLOR);
	auto ImageWidth = Image.cols / 2;
	auto RightImage = Mat(Image.rows, ImageWidth, CV_32FC3);
	auto LeftImage = Mat(Image.rows, ImageWidth, CV_32FC3);

	for (auto r = 0; r < Image.rows; ++r) {
		for (auto c = 0; c < ImageWidth; ++c) {
			for (auto i = 0; i < 3; ++i) {
				LeftImage.at<Vec3f>(r, c)[i] = Image.at<Vec3b>(r, c)[i] / 255.0f;
				RightImage.at<Vec3f>(r, c)[i] = Image.at<Vec3b>(r, c + ImageWidth)[i] / 255.0f;
			}
		}
	}

	return make_tuple(LeftImage, RightImage);
}

std::tuple<cv::Mat, cv::Mat> fin::LR1UEdge(const std::string EdgePath, std::tuple<cv::Mat, cv::Mat>& Images)
{
	using std::make_tuple;
	using std::get;

	using cv::Mat;
	using cv::Vec3f;
	using cv::Vec3b;
	using cv::Size;
	using cv::resize;

	Mat Edge = cv::imread(EdgePath, cv::IMREAD_GRAYSCALE);
	int wid = Edge.cols / 2;
	auto EdgeL = Mat(Edge.rows, wid, CV_8UC1);
	auto EdgeR = Mat(Edge.rows, wid, CV_8UC1);
	for (auto r = 0; r < EdgeL.rows; ++r) {
		for (auto c = 0; c < wid; ++c) {
			EdgeL.at<uchar>(r, c) = Edge.at<uchar>(r, c);
			EdgeR.at<uchar>(r, c) = Edge.at<uchar>(r, c + wid);
		}
	}	
	resize(EdgeL, EdgeL, get<0>(Images).size());
	resize(EdgeR, EdgeR, get<0>(Images).size());

	return make_tuple(EdgeL, EdgeR);
}

void fin::Cutting(std::tuple<cv::Mat, cv::Mat>& Image, std::tuple<cv::Mat, cv::Mat>& Edge)
{
	using std::array;
	using std::get;

	using cv::Mat;
	using cv::Vec3f;

	// boundary
	array<int, 4> bound;

	// cutting up
	const auto EdgeL = get<0>(Edge);
	const auto EdgeR = get<1>(Edge);

	for (auto up = 0; up < EdgeL.rows; ++up) {
		bool is_white = false;
		for (auto c = 0; c < EdgeL.cols; ++c) {
			if (EdgeL.at<uchar>(up, c) > 100) {
				is_white = true;
				break;
			}
			if (EdgeR.at<uchar>(up, c) > 100) {
				is_white = true;
				break;
			}
		}
		if (is_white) {
			bound[0] = up;
			break;
		}
	}

	// cutting down
	for (auto down = EdgeL.rows; down > 0; --down) {
		bool is_white = false;
		for (auto c = 0; c < EdgeL.cols; ++c) {
			if (EdgeL.at<uchar>(down - 1, c) > 100) {
				is_white = true;
				break;
			}
			if (EdgeR.at<uchar>(down - 1, c) > 100) {
				is_white = true;
				break;
			}
		}
		if (is_white) {
			bound[1] = down;
			break;
		}
	}


	// cutting left
	for (auto left = 0; left < EdgeL.cols; ++left) {
		bool is_white = false;
		for (auto r = 0; r < EdgeL.rows; ++r) {
			if (EdgeL.at<uchar>(r, left) > 100) {
				is_white = true;
				break;
			}
			if (EdgeR.at<uchar>(r, left) > 100) {
				is_white = true;
				break;
			}
		}
		if (is_white) {
			bound[2] = left;
			break;
		}
	}


	// cutting left
	for (auto right = EdgeL.cols; right >0; --right) {
		bool is_white = false;
		for (auto r = 0; r < EdgeL.rows; ++r) {
			if (EdgeL.at<uchar>(r, right) > 100) {
				is_white = true;
				break;
			}
			if (EdgeR.at<uchar>(r, right) > 100) {
				is_white = true;
				break;
			}
		}
		if (is_white) {
			bound[3] = right;
			break;
		}
	}

	int Height = bound[1] - bound[0];
	int Width = bound[3] - bound[2];

	auto CutImgL = Mat(Height, Width, CV_32FC3);
	auto CutImgR = Mat(Height, Width, CV_32FC3);
	auto CutEdgeL = Mat(Height, Width, CV_8UC1);
	auto CutEdgeR = Mat(Height, Width, CV_8UC1);

	for (auto r = 0; r < Height; ++r) {
		for (auto c = 0; c < Width; ++c) {
			CutImgL.at<Vec3f>(r, c) = get<0>(Image).at<Vec3f>(r + bound[0], c + bound[2]);
			CutImgR.at<Vec3f>(r, c) = get<0>(Image).at<Vec3f>(r + bound[0], c + bound[2]);
			CutEdgeL.at<uchar>(r, c) = get<0>(Edge).at<uchar>(r + bound[0], c + bound[2]);
			CutEdgeR.at<uchar>(r, c) = get<1>(Edge).at<uchar>(r + bound[0], c + bound[2]);
		}
	}
		
	get<0>(Image) = CutImgL;
	get<1>(Image) = CutImgR;
	get<0>(Edge) = CutEdgeL;
	get<1>(Edge) = CutEdgeR;

}

cv::Mat fin::norm3fImage(cv::Mat image)
{
	using cv::normalize;
	using cv::Vec3f;

	auto output = image.clone();

	for (auto r = 0; r < image.rows; ++r) {
		for (auto c = 0; c < image.cols; ++c) {
			output.at<Vec3f>(r, c) = normalize(image.at<Vec3f>(r, c));
		}
	}
	return output;
}

cv::Mat fin::SSD_Norm(cv::Mat Obj, cv::Mat Tar, int halfWindow, bool DEBUG)
{
	using std::cout;
	using std::pow;
	using std::abs;

	using cv::Mat;
	using cv::Vec3f;

	auto Depth = Mat(Obj.rows * 2, Obj.cols * 2, CV_32FC3);
	if (DEBUG) cout << "image(r,c) = " << Obj.rows << ", " << Obj.cols << '\n';
	
	for (auto r = halfWindow; r < Obj.rows - halfWindow; ++r) {
		for (auto c = halfWindow * 2; c < Obj.cols - halfWindow * 2; ++c) {
			if (DEBUG) cout << "\n (r,c) = " << r << "," << c << '\t';
			int dis = 0;
			float sum = -1;
			for (auto d = -c + halfWindow; d < Obj.cols - c - halfWindow; ++d) {
				if (DEBUG) cout << "(d = " << d << " ),";
				float try_sum = 0;
				for (auto w = -halfWindow; w < halfWindow; ++w) {
					for (auto h = -halfWindow; h < halfWindow; ++h) {
						try_sum += Obj.at<Vec3f>(r + w, c + h).dot(Tar.at<Vec3f>(r + w, c + h + d));
					}
				}				
				if ((try_sum > sum) /*&& (try_sum > (0.6 * pow(halfWindow * 2, 2)))*/){
					dis = d;
					sum = try_sum;
					for (auto i = 0; i < 3; ++i) {
						const int max = 127;
						if (dis == 0) {
							Depth.at<Vec3f>(r, c)[i] = max;
						}
						else {
							Depth.at<Vec3f>(r, c)[i] = abs(max / dis);
						}
						Depth.at<Vec3f>(r + Obj.rows, c)[i] = sum * 200.0f / pow(halfWindow * 2, 2);
					}
				}
			}
		}
	}
	return Depth;
}

cv::Mat fin::SSD_LR(cv::Mat Left, cv::Mat Right, int halfWindow)
{
	using cv::Vec3f;
	using cv::Mat;

	auto L = SSD_Norm(Left, Right, halfWindow, false);
	auto R = SSD_Norm(Right, Left, halfWindow, false);
	auto Depth = Mat(Left.rows, Left.cols * 2, CV_32FC3);

	for (auto r = 0; r < Depth.rows; ++r) {
		for (auto c = 0; c < Depth.cols / 2; ++c) {
			Depth.at<Vec3f>(r, c) = L.at<Vec3f>(r, c);
			Depth.at<Vec3f>(r, c + L.cols / 2) = R.at<Vec3f>(r, c);
		}
	}
	fin::save3fImage(Depth, "SSD" + std::to_string(halfWindow) + ".bmp");

	return cv::Mat();


	//vector<std::thread> Ts;
	//for (auto thd = 1; thd < 10; thd++) {
	//	Ts.push_back(std::thread([=]() {
	//		// thread
	//auto thd = 4;
	//auto SSDL = fin::SSD_Norm(NormL, NormR, thd, false);
	//auto SSDR = fin::SSD_Norm(NormR, NormL, thd, false);
	//fin::save3fImage(SSDL, "SSDL" + std::to_string(thd) + ".bmp");
	//fin::save3fImage(SSDR, "SSDR" + std::to_string(thd) + ".bmp");
	//	})); //thread
	//}
	//for (auto& T : Ts) {
	//	T.join();
	//}
}

void fin::Correlation(cv::Mat Obj, cv::Mat Tar, cv::Mat EdgeL, cv::Mat EdgeR, int halfWindow)
{
	using std::vector;
	using std::string;
	using std::to_string;
	using std::cout;
	using std::abs;

	using cv::Vec3f;

	int size = Obj.cols;
	vector<float> line_empty(size, 0);
	vector<vector<float>> table(size, line_empty);	

	for (auto r = 0; r < Obj.rows; ++r) {
		string save = "./Correlation/" + to_string(r) + ".txt";
		cout << "saving " + save << '\n';
		std::ofstream outfile(save);
		if ((r >= halfWindow ) && (r < Obj.rows - halfWindow - 1)) {
			for (auto left = halfWindow; left < Obj.cols - halfWindow - 1; ++left) {
				for (auto right = halfWindow; right < Tar.cols - halfWindow - 1; ++right) {

					float sum = 0;
					//if ((EdgeL.at<uchar>(r, left) < 100) && (EdgeR.at<uchar>(r, right) < 100)) {
					// 
					//}
					//else
					//{
					for (auto w = -halfWindow; w < halfWindow + 1; ++w) {
						for (auto h = -halfWindow; h < halfWindow + 1; ++h) {
							auto vecL = Obj.at<Vec3f>(r + w, left + h);
							auto vecR = Tar.at<Vec3f>(r + w, right + h);
							sum += abs(vecL[0] - vecR[0]);
							sum += abs(vecL[1] - vecR[1]);
							sum += abs(vecL[2] - vecR[2]);
						}
					}
					table[left][right] = sum / std::pow(halfWindow * 2 + 1, 2);
					//}
				}
			}
		}
		
		for (const auto& p : table) {
			outfile << '\n';
			for (const auto& l : p) {
				outfile << to_string(l) << '\t';
			}
		}
		outfile.close();

	} // end of r

	//system("mkdir ./Correlation/");

	
	

}

std::vector<std::tuple<cv::Vec3i, cv::Vec3f>>  fin::DynamicProgramming(const std::string& FilePath, const int row,  const cv::Mat& Obj, const cv::Mat& Tar, int window)
{
	using std::vector;
	using std::tuple;
	using std::make_tuple;
	using std::get;
	using std::cout;
	using std::min;

	using cv::Vec3f;
	using cv::Vec3i;

	auto CorTable = tablefromfile(FilePath);

	using DP = tuple<float, fin::Dir>;
	vector<vector<DP>> DPtable(Obj.cols, vector<DP>(Obj.cols, DP()));

	int n = window + 1;

	//
	// Algorithm is
	// DP[left,right]<fitting,direction> = max{DP[h,k] + 1]+Correlation[h,k]}
	//
	

	for (auto i = 1; i < DPtable.size() - 1; ++i) {
		for (auto j = 1; j < n; ++j) {
			get<0>(DPtable[i][j]) = CorTable[i][j - 1];
			get<1>(DPtable[i][j]) = fin::Dir::Left;
		}
	}
	for (auto i = 1; i < n; ++i) {
		for (auto j = 1; j < DPtable.size() - 1; ++j) {
			get<0>(DPtable[i][j]) = CorTable[i - 1][j];
			get<1>(DPtable[i][j]) = fin::Dir::Up;
		}
	}

	for (auto i = n; i < DPtable.size() - 1; ++i) {
		for (auto j = n; j < DPtable.size() - 1; ++j) {

			auto up_C = CorTable[i - 1][j];
			auto left_C = CorTable[i][j - 1];
			auto obli_C = CorTable[i - 1][j - 1];

			float up = get<0>(DPtable[i - 1][j]) + min(1.0f, up_C);
			float left = get<0>(DPtable[i][j - 1]) + min(1.0f, left_C);
			float oblig = get<0>(DPtable[i - 1][j - 1]) + min(1.0f, obli_C);

			//cout << "(left,right,uC,lC,oC)= " << left << ", " << right << ", " << up_C << ", " << left_C << ", " << obli_C << "\t";
			//cout << "(left,right,up,left,o)= " << left << ", " << right << ", " << up << ", " << left << ", " << oblig << "\n";
			
			if ( (up < oblig) && (up < left) ){
				get<0>(DPtable[i][j]) = up;
				get<1>(DPtable[i][j]) = fin::Dir::Up;
			}
			else if ( (left < oblig) && (left < up)  ){
				get<0>(DPtable[i][j]) = left;
				get<1>(DPtable[i][j]) = fin::Dir::Left;
			}
			else {
				get<0>(DPtable[i][j]) = oblig;
				get<1>(DPtable[i][j]) = fin::Dir::Obl;
			}
		
		}		
	}


	using color = tuple<Vec3i, Vec3f>;
	vector<color> output;
	int O = DPtable.size() - 1;
	int T = DPtable.size() - 1;
	output.reserve(O + T);
	int height = 0;
	int index = DPtable.size() - 1;
	while ((O > 0) && (T > 0)) {		
		Vec3i position{};
		Vec3f color{};
		//cout << "(O,T,D)=" << O << ", " << T << ", " << static_cast<int>(get<1>(DPtable[O][T])) << '\n';
		if (get<1>(DPtable[O][T]) == fin::Dir::Obl) {
			color = Obj.at<Vec3f>(row, O) + Obj.at<Vec3f>(row, T);
			color /= 2;
			position[0] = row;
			position[1] = O;
			position[2] = height;
			output.push_back(make_tuple(position, color));
			index--;

			--T;
			--O;
		}
		else if (get<1>(DPtable[O][T]) == fin::Dir::Left) {
			--T;
			--height;
		}
		else { // case : get<1>(DPtable[O][T]) == fin::Dir::Up
			--O;
			++height;			
		}
		//cout << position <<", " << color << '\n';
	}

	return output;
}

std::vector<std::vector<float>> fin::tablefromfile(const std::string & FilePath)
{
	using std::cout;
	using std::vector;
	using std::move;

	//cout << FilePath << '\n';
	std::ifstream File_test;
	File_test.open(FilePath);

	std::stringstream read_streams;
	read_streams << File_test.rdbuf();
	File_test.close();

	std::vector<std::string> content;
	auto buffer_chars = read_streams.str();
	std::string temp_chars;
	for (auto i : buffer_chars) {
		if (i == '\n')
			content.push_back(move(temp_chars));
		else
			temp_chars += i;
	}
	content.push_back(move(temp_chars));
	content.erase(content.begin());

	vector<vector<float>> table;

	for (auto& c : content) {
		//cout << "(c size) = " << c.size();
		vector<float> one_line;
		for (auto& i : c) {
			if (i == '\t') {
				auto t = move(temp_chars);
				auto data = std::atof(t.c_str());
				one_line.push_back(move(data));
			}
			else {
				temp_chars += i;
			}
		}
		table.push_back(one_line);
	}

	//cout << table.size() << ", " << table[0].size() << '\n';

	return table;
}

void fin::GaussianSmooth(std::vector<std::vector<float>>& input)
{
	using std::array;
	using std::cout;
	const int filterSize = 5;
	array<array<float, filterSize>, filterSize> filter;
	array<float, filterSize> first{ 1, 4, 7, 4, 1 };
	array<float, filterSize> second{ 4, 16, 26 ,16, 4 };
	array<float, filterSize> third{ 7, 26, 41, 26, 7 };
	filter[0] = first;
	filter[1] = second;
	filter[2] = third;
	filter[3] = second;
	filter[4] = first;
	for (auto i = 0; i < filterSize; ++i) {
		for (auto j = 0; j < filterSize; ++j) {
			filter[i][j] /= 273;
		}
	}

	auto output{ input };

	for (auto h = filterSize; h < input.size() - filterSize; ++h) {
		for (auto w = filterSize; w < input[0].size() - filterSize; ++w) {
			output[h][w] = 0;
			for (auto i = 0; i < filterSize; ++i) {
				for (auto j = 0; j < filterSize; ++j) {
					output[h][w] += filter[i][j] * input[h - filterSize + i][w - filterSize + j];
				}
			}
		}
	}
	input = output;
}
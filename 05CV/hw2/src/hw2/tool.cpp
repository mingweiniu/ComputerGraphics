#include "hw2/tool.hpp"

#include <cstdlib>
#include <cmath>
#include <cstdlib>
#include <ctime>

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

#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>
#include <opencv2/xfeatures2d.hpp>


std::vector<std::tuple<std::vector<cv::KeyPoint>, cv::Mat>> hw2::sift_files(std::vector<std::string> inputs)
{
	using std::tuple;
	using std::vector;
	using std::cout;
	using std::string;

	using cv::Mat;
	using cv::imread;

	vector<tuple<vector<cv::KeyPoint>, Mat>> output;

	cv::Ptr<cv::Feature2D> feature = cv::xfeatures2d::SIFT::create();

	for (auto file : inputs) {
		vector<cv::KeyPoint> kps;
		Mat des;

		auto image = imread(file, cv::IMREAD_GRAYSCALE);
		feature->detect(image, kps);
		feature->compute(image, kps, des);
		kps.resize(des.rows);
		output.push_back(std::make_tuple(kps, des));
		cout << file << " des is: " << des.size() << '\n';
	}

	return output;
}

std::vector<std::vector<std::tuple<float, int, int>>> hw2::findKNN(cv::Mat object, cv::Mat target, int KNN_K, int distance)
{	
	using std::cout;
	using std::vector;
	using match_tuple = std::tuple<float, int, int>;

	vector<vector<match_tuple>> output{};
	
	// height fixed 128
	int t_size{ target.rows };
	int o_size{ object.rows };
	cout << "(obj size, target size)= " << o_size << ',' << t_size << '\n';

	for (int i = 0; i < o_size; ++i) {
		vector<match_tuple> matches{};
		for (int j = 0; j < t_size; ++j) {
			float sum{};
			for (int c = 0; c < 128; ++c) {
				sum += std::abs(object.at<float>(i, c) - target.at<float>(j, c));
			}
			//sum = std::pow(sum, 0.5);
			if (sum < distance) {
				matches.push_back(std::make_tuple(sum, i, j));
			}

			std::sort(matches.begin(), matches.end(),
				[](match_tuple const& n1, match_tuple const& n2) {
				return std::get<0>(n1) < std::get<0>(n2);
			});
		}
		if (matches.size() > 0) {
			if (matches.size() >= KNN_K) {
				matches.resize(KNN_K);
			}
			output.push_back(matches);
		}
	}


	std::cout << "matches size =" << output.size() << '\n';

	return output;
}

std::vector<int> hw2::random_id(int max, int sample_num)
{
	// sampling without same index
	std::set<int> rand_samples;

	while (rand_samples.size() < sample_num) {
		std::srand(std::time(nullptr));
		auto r = std::rand() % max;
		if (rand_samples.find(r) == rand_samples.end()) {
			rand_samples.insert(r);
		}
	}

	std::vector<int> rand_id(rand_samples.begin(), rand_samples.end());
	// finish sampling
	return rand_id;
}

std::vector<std::vector<int>> hw2::permutation(std::vector<std::vector<int>> input)
{
	std::deque<std::vector<int>> que;
	for (auto i = 0; i < input[0].size(); ++i) {
		std::vector<int> one_data{ input[0][i] };
		que.push_back(one_data);
	}

	bool doing = true;
	while (doing)
	{
		auto head = que.front();
		auto tail = que.back();
		doing = (head.size() != input.size()) || (tail.size() != input.size());

		//std::cout << "\n current quere = \n";
		//for (auto q : que) {
		//	std::cout << '\t';
		//	for (auto i : q) {
		//		std::cout << i << ' ';
		//	}
		//}

		if (head.size() < input.size()) {
			que.erase(que.begin());
			for (auto i = 0; i < input[head.size()].size(); ++i) {
				auto temp = head;
				auto append = input[head.size()][i];
				temp.push_back(append);
				que.push_back(temp);
			}
		}
	}

	std::vector<std::vector<int>> output(que.begin(), que.end());

	return output;
}

cv::Mat hw2::ransac(std::vector<std::vector<std::tuple<float, int, int>>> matches, std::vector<cv::KeyPoint> obj_kp, std::vector<cv::KeyPoint> tar_kp, int sample_num, int distance)
{
	using std::tuple;
	using std::get;
	using std::vector;
	using cv::Mat;

	using oXY_tUV = tuple<float, float, float, float>;

	auto score{ std::numeric_limits<float>::max() };
	
	Mat output;

	while (score > distance) {
		score = std::numeric_limits<float>::max();

		// sampling
		if (sample_num > matches.size()) sample_num = matches.size();
		auto ids = random_id(matches.size(), sample_num);

		std::vector<std::vector<std::tuple<float, int, int>>> rand_match;
		for (auto id : ids) {
			rand_match.push_back(matches[id]);
		}

		// permutation & combination
		std::vector<std::vector<int>> material;
		for (auto n = 0; n < rand_match.size(); ++n) {
			std::vector<int> one_set;
			for (auto i = 0; i < rand_match[n].size(); ++i) {
				one_set.push_back(i);
			}
			material.push_back(one_set);
		}
		auto combination = permutation(material);

		std::vector<std::vector<oXY_tUV>> enum_pairs;
		for (auto one_set : combination) {
			std::vector<oXY_tUV> e_pair;
			for (auto i = 0; i < one_set.size(); ++i) {
				auto one_match = rand_match[i][one_set[i]];
				auto obj = obj_kp[get<1>(one_match)];
				auto tar = tar_kp[get<2>(one_match)];
				oXY_tUV one_pair{ obj.pt.x, obj.pt.y, tar.pt.x, tar.pt.y };
				e_pair.push_back(one_pair);
			}
			enum_pairs.push_back(e_pair);
		}

		for (auto pair : enum_pairs) {
			auto H = get_H_mat(pair);
			float try_dis{};
			for (auto p : pair) {

				// tar u v
				cv::Mat tar_uv(3, 1, CV_32FC1);
				tar_uv.at<float>(0, 0) = get<2>(p);
				tar_uv.at<float>(1, 0) = get<3>(p);
				tar_uv.at<float>(2, 0) = 1;

				cv::Mat try_node = H.inv() * tar_uv;

				try_node.at<float>(0, 0) = try_node.at<float>(0, 0) / try_node.at<float>(2, 0);
				try_node.at<float>(1, 0) = try_node.at<float>(1, 0) / try_node.at<float>(2, 0);

				try_dis += std::abs(try_node.at<float>(0, 0) - get<0>(p));
				try_dis += std::abs(try_node.at<float>(1, 0) - get<1>(p));

			}
			std::cout << try_dis << '\n';
			if (try_dis < score) { 
				score = try_dis; 
				output = H;
			}
		}
		

	}// end of while

	return output;
}

cv::Mat hw2::get_H_mat(std::vector<std::tuple<float, float, float, float>> obj_tar)
{
	const int H_d = 9;
	using one_line = std::array<float, H_d>;

	// obj_xy_tar_uv
	auto A = cv::Mat(2 * obj_tar.size(), H_d, CV_32FC1);

	for (auto L = 0; L < obj_tar.size(); ++L) {
		auto ox = std::get<0>(obj_tar[L]);
		auto oy = std::get<1>(obj_tar[L]);
		auto tx = std::get<2>(obj_tar[L]);
		auto ty = std::get<3>(obj_tar[L]);

		one_line first{
			ox, oy, 1,
			0,0,0,
			-tx*ox, -tx*oy, -tx * 1,
		};
		one_line second{
			0,0,0,
			ox, oy, 1,
			-ty*ox, -ty*oy, -ty * 1,
		};

		for (int i = 0; i < 9; ++i) {
			A.at<float>(2 * L, i) = first[i];
			A.at<float>(2 * L + 1, i) = second[i];
		}
	}


	auto AT = cv::Mat(H_d, 2 * obj_tar.size(), CV_32FC1);
	cv::transpose(A, AT);

	cv::Mat EVal, EVec;
	cv::eigen(AT * A, EVal, EVec);

	//float EV = std::numeric_limits<float>::max();
	//int r = -1;
	//for (auto i = 0; i < EVal.cols; ++i) {
	//	//std::cout << EVal.at<float>(i, 0) << '\n';
	//	auto eigenvalue = EVal.at<float>(0, i);
	//	if (eigenvalue > -1e1) {
	//		if (EVal.at<float>(0, i) < EV) {
	//			r = i;
	//		}
	//	}
	//}

	float r = EVec.at<float>(0, 0);
	auto index{0};
	for (auto i = 0; i<EVal.rows; ++i) {
		if (EVal.at<float>(i, 0) < r) {
			r = EVal.at<float>(i, 0);
			index = i;
		}
	}
	

	cv::Mat output(3, 3, CV_32FC1);
	for (auto i = 0; i < 3; ++i) {
		for (auto j = 0; j < 3; ++j) {
			output.at<float>(i, j) = EVec.at<float>(index, 3 * i + j);
		}
	}
	return output;
}

void hw2::forward_warping(cv::Mat H, std::string obj_path, std::string tar_path, std::string save_path)
{
	cv::Mat obj_img = cv::imread(obj_path, cv::IMREAD_COLOR);
	cv::Mat tar_img = cv::imread(tar_path, cv::IMREAD_COLOR);
	cv::Mat check = cv::imread(tar_path, cv::IMREAD_COLOR);

	for (int r = 0; r < tar_img.rows; r++) {
		for (int c = 0; c < tar_img.cols; c++) {
			auto col = tar_img.at<cv::Vec3b>(r, c);
			decltype(col) black{};
			tar_img.at<cv::Vec3b>(r, c) = black;
		}
	}

	for (int r = 0; r < obj_img.rows; r++) {
		for (int c = 0; c < obj_img.cols; c++) {
			auto color = obj_img.at<cv::Vec3b>(r, c);

			if (color[0] > 1 && color[1] > 1 && color[2] > 1) {
				auto obj = cv::Mat(3, 1, CV_32FC1);

				obj.at<float>(0, 0) = r;
				obj.at<float>(1, 0) = c;
				obj.at<float>(2, 0) = 1;

				cv::Mat new_pos = H * obj;
				auto n = new_pos.at<float>(2, 0);
				new_pos.at<float>(0, 0) /= new_pos.at<float>(2, 0);
				new_pos.at<float>(1, 0) /= new_pos.at<float>(2, 0);
				new_pos.at<float>(2, 0) /= new_pos.at<float>(2, 0);

				//int p_r = tar_img.rows - new_pos.at<float>(0, 0);
				//int p_c = tar_img.cols - new_pos.at<float>(1, 0);
				auto p_r = static_cast<int>(new_pos.at<float>(0, 0));
				auto p_c = static_cast<int>(new_pos.at<float>(1, 0));

				if ((0 < p_r) && (p_r < tar_img.rows)) {
					if ((0 < p_c) && (p_c < tar_img.cols)) {
						tar_img.at<cv::Vec3b>(p_r, p_c) = color;
						//std::cout << "(r,c)" << r << ',' << c << '\n';
						//std::cout << "(nr,nc)" << new_pos(0, 0) << ',' << new_pos(1, 0) << '\n';
					}
				}
			}
		}
	}

	cv::imwrite(save_path, tar_img);
}

void hw2::backward_warping(cv::Mat H, std::string obj_path, std::string tar_path, std::string save_path)
{
	cv::Mat obj_img = cv::imread(obj_path, cv::IMREAD_COLOR);
	cv::Mat tar_img = cv::imread(tar_path, cv::IMREAD_COLOR);

	// flush
	for (int r = 0; r < tar_img.rows; r++) {
		for (int c = 0; c < tar_img.cols; c++) {
			auto col = tar_img.at<cv::Vec3b>(r, c);
			decltype(col) black{};
			tar_img.at<cv::Vec3b>(r, c) = black;
		}
	}

	//printH(H);

	for (int r = 0; r < tar_img.rows; r++) {
		for (int c = 0; c < tar_img.cols; c++) {

			auto tar = cv::Mat(3, 1, CV_32FC1);
			tar.at<float>(0, 0) = r;
			tar.at<float>(1, 0) = c;
			tar.at<float>(2, 0) = 1;

			cv::Mat obj_pos = H.inv() * tar;

			int p_r = obj_pos.at<float>(0, 0) / obj_pos.at<float>(2, 0);
			int p_c = obj_pos.at<float>(1, 0) / obj_pos.at<float>(2, 0);


			if ((0 < p_r) && (p_r < obj_img.rows)) {
				if ((0 < p_c) && (p_c < obj_img.cols)) {
					auto color = obj_img.at<cv::Vec3b>(p_r, p_c);
					if (color[0] > 1 && color[1] > 1 && color[2] > 1) {
						tar_img.at<cv::Vec3b>(r, c) = color;
					}
				}
			}
		}
	}

	cv::imwrite(save_path, tar_img);
}

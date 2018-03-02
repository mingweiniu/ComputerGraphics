#include "hw2/hw2_tool.hpp"

#include <cstdlib>
#include <cmath>
#include <cstdlib>

#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <thread>
#include <array>
#include <string>
#include <unordered_set>
#include <limits>

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>

#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>
#include <opencv2/xfeatures2d.hpp>

namespace hw2 {

	namespace bu = boost::numeric::ublas;
	using mat2d = bu::matrix<float>;
	using mat3d = boost::multi_array<float, 3>;

	bool InvertMatrix(const mat2d & input, mat2d & inverse)
	{
		//
		// this function is copied from net,
		// not writed by me.
		//
	
		using namespace boost::numeric::ublas;
		typedef permutation_matrix<std::size_t> pmatrix;
		// create a working copy of the input
		matrix<double> A(input);
		// create a permutation matrix for the LU-factorization
		pmatrix pm(A.size1());
	
		// perform LU-factorization
		auto res = lu_factorize(A, pm);
		if (res != 0) return false;
	
		// create identity matrix of "inverse"
		inverse.assign(boost::numeric::ublas::identity_matrix<double>(A.size1()));
	
		// backsubstitute to get the inverse
		lu_substitute(A, pm, inverse);
	
		return true;
	}

	void printH(mat2d H)
	{
		std::cout << "H is : \n";
		for (auto i = 0; i < 3; ++i) {
			for (auto j = 0; j < 3; ++j) {
				std::cout << H(i, j) << ' ';
			}
			std::cout << '\n';
		}
	}
	
	void check_is_dir(std::string directory_path) {
		std::cout << directory_path;
		boost::filesystem::path dir(directory_path);
		auto is_dir = boost::filesystem::is_directory(dir);
		if (!is_dir) {
			std::cout << " is not a directory path";
			std::exit(EXIT_SUCCESS);
		}
	}

	std::vector<std::tuple<std::string, cv::Mat>> read_img_from_dir(std::string directory_path) {
		check_is_dir(directory_path);
		using image = std::tuple<std::string, cv::Mat>;
		std::vector<std::tuple<std::string, cv::Mat>> output;
		boost::filesystem::path dir_path(directory_path);

		std::cout << directory_path << " has : \n";
		for (auto& entry : boost::make_iterator_range(boost::filesystem::directory_iterator(dir_path), {})) {
			auto temp_path = entry.path();
			auto file_name = temp_path.string();
			std::cout << '\t' << file_name << '\n';

			if (file_name.find("target") < file_name.size()) {
				auto img = cv::imread(file_name.c_str(), cv::IMREAD_GRAYSCALE);
				image res{ file_name, std::move(img)};
				output.emplace_back(std::move(res));
			}
		}

		for (auto& entry : boost::make_iterator_range(boost::filesystem::directory_iterator(dir_path), {})) {
			auto temp_path = entry.path();
			auto file_name = temp_path.string();
			if (file_name.find("puzzle") < file_name.size()) {
				auto img = cv::imread(file_name.c_str(), cv::IMREAD_GRAYSCALE);
				image res{ file_name, std::move(img) };
				output.emplace_back(std::move(res));
			}
		}
		std::cout << "images number = " << output.size() << '\n';
		return output;
	}

	std::tuple<std::vector<cv::KeyPoint>, cv::Mat> sift_img(cv::Mat image)
	{
		std::vector<cv::KeyPoint> keypoints;
		cv::Mat descriptors;
		cv::Ptr<cv::Feature2D> cv_sift = cv::xfeatures2d::SIFT::create();
		cv_sift->detect(image, keypoints);
		cv_sift->compute(image, keypoints, descriptors);

		auto s = descriptors.size();
		//std::cout << descriptors;
		
		// normalize descriptors
		float sum = 0;
		for (int c = 0; c < 128; ++c) {
			auto d = std::pow(descriptors.at<float>(0, c), 2);
			sum += std::move(d);
		}
		sum = std::pow(sum, 0.5);
		for (int c = 0; c < 128; ++c) {
			descriptors.at<float>(0, c) /= sum;
		}
		keypoints.resize(s.height);
		std::cout << " key size =" << keypoints.size() << '\n';
		std::tuple<std::vector<cv::KeyPoint>, cv::Mat> output{ std::move(keypoints), descriptors };

		return output;

	}

	std::vector<std::vector<std::tuple<float, int, int>>> findKNN(cv::Mat object, cv::Mat target, int KNN_K)
	{
		int distance = 400;
		std::vector<std::vector<std::tuple<float, int, int>>> output;
		//while (output.size() < length) {
		//	output.resize(0);
			distance += 200;
			// height fixed 128
			int t_size{ target.rows };
			int o_size{ object.rows };
			std::cout << "(obj size,target size)= " << o_size << ',' << t_size << '\n';

			for (int i = 0; i < o_size; ++i) {
				using match_tuple = std::tuple<float, int, int>;
				std::vector<match_tuple> matches{};
				for (int j = 0; j < t_size; ++j) {
					float sum{};
					for (int c = 0; c < 128; ++c) {
						auto d = std::pow(object.at<float>(i, c) - target.at<float>(j, c), 2);
						sum += std::move(d);
					}
					sum = std::pow(sum, 0.5);
					if (sum < distance) {
						matches.push_back(std::make_tuple(sum, i, j));
					}
				}
				if (matches.size() >= KNN_K) {
					std::sort(matches.begin(), matches.end(),
						[](match_tuple const& n1, match_tuple const& n2) {
						return std::get<0>(n1) < std::get<0>(n2);
					});
					matches.resize(KNN_K);
				}
				if(matches.size()) output.push_back(matches);
			}
		//}
		std::cout << "matches size ="  << output.size() << '\n';
		return output;

	}

	mat2d ransac(
		std::vector<cv::KeyPoint> obj_kp,
		cv::Mat obj_des,
		std::vector<cv::KeyPoint> tar_kp,
		cv::Mat tar_des,
		std::vector<std::vector<std::tuple<float, int, int>>> matches
	) {
		// sampling without same index
		int sample_num = 4;
		std::unordered_set<int> rand_samples;

		while (rand_samples.size() < sample_num + 1) {
			std::srand(std::time(nullptr));
			auto l = std::rand();
			auto r = matches.size();
			auto random = l % r;
			if (rand_samples.find(random) == rand_samples.end()) {
				rand_samples.insert(random);
			}
		}

		std::vector<int> rand_id(rand_samples.begin(), rand_samples.end());
		// finish sampling

		// object xy & target uv
		using oXY_tUV = std::tuple<float, float, float, float>;
		std::vector<std::vector<oXY_tUV>> match_pairs;
		for (auto id : rand_id) {
			std::vector<oXY_tUV> pair;
			for (auto m : matches[id]) {
				auto obj = obj_kp[std::get<1>(m)];
				auto tar = tar_kp[std::get<2>(m)];
				oXY_tUV one_pair{ obj.pt.x, obj.pt.y, tar.pt.x, tar.pt.y };
				pair.push_back(std::move(one_pair));
			}
			match_pairs.push_back(std::move(pair));
		}

		
		// permutation & combination
		std::vector<std::vector<int>> material;
		for (auto n = 0; n < sample_num; ++n) {
			std::vector<int> one_set;
			for (auto i = 0; i < matches[rand_id[n]].size(); ++i) {
				one_set.push_back(i);
			}
			material.push_back(one_set);
		}
		auto combination = permutation(material);
		
		std::vector<std::vector<oXY_tUV>> enum_pairs;
		for (auto one_set : combination) {
			std::vector<oXY_tUV> e_pair;
			for (auto i = 0; i < one_set.size(); ++i) {
				auto temp = match_pairs[i][one_set[i]];
				e_pair.push_back(temp);
			}
			enum_pairs.push_back(e_pair);
		}
		// finish permutation

		//[{'obj_no': 29, 'dis' : 350.0, 'tar_no' : 262},
		//{ 'obj_no': 0, 'dis' : 373.0, 'tar_no' : 277 },
		//{ 'obj_no': 10, 'dis' : 328.0, 'tar_no' : 288 },
		//{ 'obj_no': 99, 'dis' : 348.0, 'tar_no' : 615 }]
		enum_pairs.resize(0);
		oXY_tUV first{ obj_kp[29].pt.x, obj_kp[29].pt.y, tar_kp[262].pt.x, tar_kp[262].pt.y };
		oXY_tUV second{ obj_kp[0].pt.x, obj_kp[0].pt.y, tar_kp[277].pt.x, tar_kp[277].pt.y };
		oXY_tUV third{ obj_kp[10].pt.x, obj_kp[10].pt.y, tar_kp[288].pt.x, tar_kp[288].pt.y };
		oXY_tUV forth{ obj_kp[99].pt.x, obj_kp[99].pt.y, tar_kp[615].pt.x, tar_kp[615].pt.y };
		std::vector<oXY_tUV> testSet{first, second, third, forth};
		for (auto i : testSet) {
			std::cout << '\n';
			std::cout << std::get<0>(i) << ' ';
			std::cout << std::get<1>(i) << ' ';
			std::cout << std::get<2>(i) << ' ';
			std::cout << std::get<3>(i) << ' ';
			std::cout << '\n';
		}
		enum_pairs.push_back(testSet);


		// try all pair
		float score = std::numeric_limits<float>::max();
		mat2d H_mat(3, 3);		
		for (auto pair : enum_pairs) {
			float dis{};
			auto H_try = get_H_mat(pair);
			for (auto obj_tar : pair) {

				mat2d obj_xy(3, 1);
				obj_xy(0, 0) = std::get<0>(obj_tar);
				obj_xy(1, 0) = std::get<1>(obj_tar);
				obj_xy(2, 0) = 1;

				mat2d tar_uv(3, 1);
				tar_uv(0, 0) = std::get<2>(obj_tar);
				tar_uv(1, 0) = std::get<3>(obj_tar);
				tar_uv(2, 0) = 1;

				mat2d test(3, 1);
				test = prod(H_try, obj_xy);
				test(0, 0) /= test(2, 0);
				test(1, 0) /= test(2, 0);
				test(1, 0) /= test(2, 0);

				dis += (std::abs(test(0, 0) - tar_uv(0, 0)));
				dis += (std::abs(test(1, 0) - tar_uv(1, 0)));
			}

			if (dis < score) {
				score = dis;
				H_mat = H_try;
			}
		}
		return H_mat;
	}


	mat2d get_H_mat(std::vector<std::tuple<float, float, float, float>> obj_tar)
	{
		const int H_d = 9;
		using one_line = std::array<float, H_d>;
		// obj_xy_tar_uv

		//auto A = mat2d(2 * obj_tar.size(), H_d);

		auto A = cv::Mat(2 * obj_tar.size(), H_d, CV_32FC1);

		for (auto l = 0; l < obj_tar.size(); ++l) {
			auto ox = std::get<0>(obj_tar[l]);
			auto oy = std::get<1>(obj_tar[l]);
			auto tx = std::get<2>(obj_tar[l]);
			auto ty = std::get<3>(obj_tar[l]);

			one_line first{
				ox, oy, 1,
				0,0,0,
				-tx*ox, -tx*oy, -tx,
			};
			one_line second{
				0,0,0,
				ox, oy, 1,
				-ty*ox, -ty*oy, -ty,
			};

			for (int i = 0; i < 9; ++i) {
				//A(2 * l, i) = first[i];
				//A(2 * l + 1, i) = second[i];
				A.at<float>(2 * l, i) = first[i];
				A.at<float>(2 * l + 1, i) = second[i];
			}
		}		

		//std::cout << "here\n";
		//std::cout << A << '\n';

		auto AT = cv::Mat(H_d, 2 * obj_tar.size(), CV_32FC1);
		cv::transpose(A, AT);		

		/*mat2d ATA(H_d, H_d);*/
		//ATA = prod(AT, A);
		//auto AT = bu::trans(A);
		//auto ATA = cv::Mat(H_d, H_d, CV_32FC1);
		A = AT * A;
		std::cout << "here\n";
		//std::cout << ATA;

		//auto inputE = cv::Mat(H_d, H_d, CV_32FC1);
		cv::Mat EVal, EVec;
		//int heigh, low;
		//for (auto i = 0; i < H_d; ++i) {
		//	for (auto j = 0; j < H_d; ++j) {
		//		inputE.at<float>(i, j) = ATA(i, j);
		//	}
		//}
		//cv::eigen(inputE, EVal, EVec);

		cv::eigen(A, EVal, EVec);
	

		std::cout << A << '\n';
		float EV = std::numeric_limits<float>::max();

		int r;

		for (auto i = 0; i < EVal.rows; ++i) {
			//std::cout << EVal.at<float>(i, 0) << '\n';
			auto eigenvalue = EVal.at<float>(i, 0);
			if (eigenvalue > -1e1) {
				if (EVal.at<float>(i, 0) < EV) {
					r = i;
				}
			}
		}


		//for (auto i = 0; i < EVal.rows; ++i) {
		//	std::cout << "EVal " << i << ":";
		//	for (auto j = 0; j < EVal.cols; ++j) {
		//		std::cout << EVal.at<float>(i, j) << ' ';
		//	}
		//	std::cout << '\n';
		//}

		//for (auto i = 0; i < EVec.rows; ++i) {
		//	std::cout << "EVec " << i << ":";
		//	for (auto j = 0; j < EVec.cols; ++j) {
		//		std::cout << EVec.at<float>(i, j) << ' ';
		//	}
		//	std::cout << '\n';
		//}

		//EVec.at<float>(r, 0) = -2.47859701e-04;
		//EVec.at<float>(r, 1) = 2.45592151e-03;
		//EVec.at<float>(r, 2) = -2.17956954e-01;
		//EVec.at<float>(r, 3) = -2.38316603e-03;
		//EVec.at<float>(r, 4) = 1.92802967e-04;
		//EVec.at<float>(r, 5) = 9.75950913e-01;
		//EVec.at<float>(r, 6) = -8.23068137e-07;
		//EVec.at<float>(r, 7) = 1.33403802e-06;
		//EVec.at<float>(r, 8) = 1.66512482e-03;
		

		mat2d output(3, 3);
		for (auto i = 0; i < 3; ++i) {
			for (auto j = 0; j < 3; ++j) {
				output(i, j) = EVec.at<float>(r, 3 * i + j);
			}
		}
		//output = bu::trans(output);
		return output;
	}

	std::vector<std::vector<int>> permutation(std::vector<std::vector<int>> input)
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

	void forward_warping(mat2d H, std::string obj_path, std::string tar_path, std::string save_path)
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
					auto obj = mat2d(3, 1);
					
					obj(0, 0) = r;
					obj(1, 0) = c;
					obj(2, 0) = 1;

					mat2d new_pos = prod(H, obj);
					auto n = new_pos(2, 0);
					new_pos(0, 0) /= new_pos(2, 0);
					new_pos(1, 0) /= new_pos(2, 0);
					new_pos(2, 0) /= new_pos(2, 0) ;

					int p_r = tar_img.rows - new_pos(0, 0);
					int p_c = tar_img.cols - new_pos(1, 0);
					p_r = new_pos(0, 0);
					p_c = new_pos(1, 0);

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

		//float dis{};
		//for (int r = 0; r < tar_img.rows; r++) {
		//	for (int c = 0; c < tar_img.cols; c++) {
		//		auto tar_col = tar_img.at<cv::Vec3b>(r, c);
		//		auto check_col = check.at<cv::Vec3b>(r, c);
		//		for (auto i = 0; i < 3; ++i) {
		//			dis += std::abs(tar_col[i] - check_col[i]);
		//		}

		//	}
		//}
		cv::imwrite(save_path, tar_img);		
	}
	void backward_warping(mat2d H, std::string obj_path, std::string tar_path, std::string save_path)
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
		auto H_inv = mat2d(3, 3);
		InvertMatrix(H, H_inv);

		for (int r = 0; r < tar_img.rows; r++) {
			for (int c = 0; c < tar_img.cols; c++) {

				auto tar = mat2d(3, 1);
				tar(0, 0) = r;
				tar(1, 0) = c;
				tar(2, 0) = 1;

				mat2d obj_pos = prod(H_inv, tar);

				int p_r = obj_pos(0, 0) / obj_pos(2,0);
				int p_c = obj_pos(1, 0) / obj_pos(2, 0);


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
}
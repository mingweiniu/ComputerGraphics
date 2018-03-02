#include "hw1/hw1_tool.hpp"

#include <cstdlib>

#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <cmath>

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>

#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

namespace hw1 {

	namespace bu = boost::numeric::ublas;
	using mat2d = bu::matrix<double>;
	using mat3d = boost::multi_array<double, 3>;


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

	void check_is_dir(std::string directory_path) {
		std::cout << directory_path;
		boost::filesystem::path dir(directory_path);
		auto is_dir = boost::filesystem::is_directory(dir);
		if (!is_dir) {
			std::cout << " is not a directory path";
			std::exit(EXIT_SUCCESS);
		}
	}

	std::vector<std::string> txtfile(std::string file_path) {
		std::vector<std::string> content;
		std::ifstream file;
		file.open(file_path.c_str());

		//
		// read file as stream
		//
		std::stringstream read_streams;
		read_streams << file.rdbuf();
		file.close();

		auto buffer_chars = read_streams.str();
		std::string temp_chars;
		for (auto letter : buffer_chars) {
			if (letter == '\n')
				content.push_back(std::move(temp_chars));
			else
				temp_chars += letter;
		}
		return content;
	}

	void cvMatPrint(cv::Mat matrix) {
		for (int rowIndex = 0; rowIndex < matrix.rows; rowIndex++) {
			std::cout << '\t';
			for (int colIndex = 0; colIndex < matrix.cols; colIndex++) {
				std::cout << matrix.at<float>(rowIndex, colIndex) << '\t';
			}
			std::cout << '\n';
		}

	}

	void getMaxMin(double& Max, double& Min, double& input)
	{
		Max = std::max(Max, input);
		Min = std::min(Min, input);
	}


	boost::numeric::ublas::matrix<double> read_light(std::string file_path) {
		hw1::check_is_dir(file_path);
		std::cout << "LightSource.txt :\n";
		auto datas = hw1::txtfile(file_path + "LightSource.txt");
		boost::numeric::ublas::matrix<double> lights(datas.size(), 3);

		//
		// parsing three value (X, Y, Z)
		//
		for (std::size_t i = 0; i < datas.size(); ++i) {
			auto line = datas[i];
			std::vector<double> temp;
			if (line.find("(") < line.size()) {
				line.erase(line.begin(), line.begin() + line.find("(") + 1);
				temp.reserve(3);
				while (line.find(",") < line.size()) {
					std::string number(line, 0, line.find(","));
					temp.push_back(static_cast<double>(std::stof(number)));
					line.erase(line.begin(), line.begin() + line.find(",") + 1);
				}
				if (line.find(")") < line.size()) {
					line.erase(line.find(")"));
					temp.push_back(static_cast<double>(std::stof(line)));
				}

				std::cout << '\t' << temp[0] << '\t' << temp[1] << '\t' << temp[2] << '\n';
				for (std::size_t j = 0; j < 3; ++j) {
					lights(i, j) = temp[j];
				}
			}
		}
		return lights;
	}
		
	mat2d pseudo_inverse(mat2d input) {
		std::cout << "input: \n" << input << '\n';
		auto row = input.size1();

		bool matrix_is_wide = input.size1() < input.size2();

		auto small_side = std::min(input.size1(), input.size2());
		auto large_side = std::max(input.size1(), input.size2());

		auto wide_mat = mat2d(large_side, small_side);
		auto long_mat = mat2d(small_side, large_side);

		if (matrix_is_wide) {
			wide_mat = input;
			long_mat = bu::trans(input);
		}
		else {
			wide_mat = bu::trans(input);
			long_mat = input;
		}


		mat2d AAT(small_side, small_side);
		AAT = prod(wide_mat, long_mat);
		std::cout << "AAT : \n" << AAT << '\n';

		mat2d inv_AAT(small_side, small_side);
		InvertMatrix(AAT, inv_AAT);
		std::cout << "AAT_inv : \n" << inv_AAT << '\n';

		auto pseudo_inv = mat2d(input.size2(), input.size1());
		if (matrix_is_wide) {
			pseudo_inv = bu::prod(long_mat, inv_AAT);
		}
		else {
			pseudo_inv = bu::prod(inv_AAT, wide_mat);
		}
		std::cout << "pseudo_inv : \n" << pseudo_inv << '\n';

		return pseudo_inv;
	}

	std::vector<mat2d> read_img_from_dir(std::string directory_path) {
		hw1::check_is_dir(directory_path);
		std::vector<mat2d> images;
		boost::filesystem::path dir_path(directory_path);

		auto max = -999.0;
		auto min = 999.0;
		std::cout << directory_path << " has : \n";
		for (auto& entry : boost::make_iterator_range(boost::filesystem::directory_iterator(dir_path), {})) {
			auto temp_path = entry.path();
			auto file_name = temp_path.string();
			std::cout << '\t' << file_name << '\n';

			if (file_name.find(".bmp") < file_name.size()) {
				auto img = cv::imread(file_name.c_str(), cv::IMREAD_GRAYSCALE);
				auto temp = mat2d(img.rows, img.cols);
				for (auto h = 0; h < img.rows; ++h) {
					for (auto w = 0; w < img.cols; ++w) {
						auto res = static_cast<double>(img.at<uchar>(h, w));
						getMaxMin(max, min, res);
						temp(h, w) = std::move(res);
					}
				}
				images.emplace_back(std::move(temp));
			}
		}
		std::cout << "images range is [ " << min << ", " << max << " ]\n";
		return images;
	}
	
	void repair_img(mat2d& input)
	{

		auto height = input.size1();
		auto width = input.size2();
		for (auto h = 0; h < height; ++h) {
			for (auto w = 0; w < width; ++w) {
				if ((0 < h) && (h < height - 1) && (0 < w) && (w < width - 1)) {
					auto diff_nx = std::abs(input(h, w) - input(h - 1, w));
					auto diff_px = std::abs(input(h, w) - input(h + 1, w));
					auto diff_ny = std::abs(input(h, w) - input(h, w - 1));
					auto diff_py = std::abs(input(h, w) - input(h, w + 1));
					if (diff_nx + diff_px + diff_ny + diff_py > 40) {
						if (std::abs(input(h + 1, w) - input(h - 1, w)) < 15) {
							input(h, w) = (input(h + 1, w) + input(h - 1, w)) / 2;
						}
						if (std::abs(input(h, w + 1) - input(h, w - 1)) < 15) {
							input(h, w) = (input(h, w + 1) + input(h, w - 1)) / 2;
						}
					}
				}
			}
		}
	}

	mat3d lightInv_mul_images(mat2d light_inv, std::vector<mat2d> images)
	{
		//
		// make all data into one big matrix 
		//
		auto obj_mat = mat2d(images.size(), images[0].size1() * images[0].size2());
		for (std::size_t i = 0; i < images.size();++i) {
			for (auto h = 0; h < images[0].size1(); ++h) {
				for (auto w = 0; w < images[0].size2(); ++w) {
					obj_mat(i, images[0].size2() * h + w) = images[i](h, w);
				}
			}
		}
		std::cout << "obj_mat(" << obj_mat.size1() << ", " << obj_mat.size2() << ")\n";

		//
		// calculate normal vectors by inverse * input 
		//
		auto normals = mat2d(light_inv.size1(), obj_mat.size2());
		normals = bu::prod(light_inv, obj_mat);
		std::cout << "normals(" << normals.size1() << ", " << normals.size2() << ")\n";

		//
		// normalize normal vectors
		//
		auto vec = bu::vector<double>(normals.size1());
		auto norm = bu::norm_2(vec);
		for (std::size_t j = 0; j < normals.size2(); ++j) {
			for (std::size_t i = 0; i < normals.size1(); ++i) {
				vec(i) = normals(i, j);
			}
			norm = bu::norm_2(vec);
			if (norm > 1e-8) {
				for (std::size_t i = 0; i < normals.size1(); ++i) {
					normals(i, j) = normals(i, j) / norm;
				}
			}
		}

		//
		// put normals into three dimential matrix
		//
		using mat3d = boost::multi_array<double, 3>;
		mat3d img_normals(boost::extents[images[0].size1()][images[0].size2()][3]);
		for (auto h = 0; h < images[0].size1(); ++h) {
			for (auto w = 0; w < images[0].size2(); ++w) {
				for (auto i = 0; i < 3; ++i) {
					auto res = normals(i, h*images[0].size2() + w);
					img_normals[h][w][i] = std::move(res);
				}
			}
		}
		std::cout << "img_normals (" << img_normals.size() << ", " << img_normals[0].size() << ")\n";
		return img_normals;
	}

	void print_as_RGB(mat3d input)
	{
		const bool print_xyz_separate = true;
		auto height = input.size();
		auto width = input[0].size();
		
		auto max = -999.0;
		auto min = 999.0;
		if (!print_xyz_separate) {
			auto output = cv::Mat(height, width, CV_32FC3);
			for (auto h = 0; h < height; ++h) {
				for (auto w = 0; w < width; ++w) {
					for (auto i = 0; i < 3; ++i) {
						auto res = input[h][w][i];
						getMaxMin(max, min, res);
						output.at<cv::Vec3f>(h, w)[i] = std::move(res);
					}
				}
			}
			cv::imshow("output", output);
			cv::waitKey();
		}
		else
		{
			auto output = cv::Mat(input.size() * 3, input[0].size() * 3, CV_32FC3);
			for (std::size_t h = 0; h < height; ++h) {
				for (std::size_t w = 0; w < input[0].size(); ++w) {
					for (std::size_t i = 0; i < 3; ++i) {
						auto res = input[h][w][i];
						getMaxMin(max, min, res);
						output.at<cv::Vec3f>(h + height * i, w)[i] = std::move(res);
					}
				}
			}
			for (std::size_t i = 0; i < 3; ++i) {
				for (std::size_t h = 1; h < height - 1; ++h) {
					for (std::size_t w = 1; w < width - 1; ++w) {
						auto sub_x = static_cast<float>(input[h][w][i] - input[h - 1][w][i]);
						auto sub_y = static_cast<float>(input[h][w][i] - input[h][w - 1][i]);

						output.at<cv::Vec3f>(h + height * i, width * 1 + w)[i] = std::move(sub_x);
						output.at<cv::Vec3f>(h + height * i, width * 2 + w)[i] = std::move(sub_y);
					}
				}
			}
			std::cout << "data range is [ " << min << ", " << max << " ]\n";
			cv::imshow("output", output);
			cv::waitKey();
		}
	}

	mat2d calculate_depth(mat3d input)
	{
		auto height = input.size();
		auto width = input[0].size();
		std::vector<mat2d> sums;
		auto res_px_py = mat2d(height, width, 0);
		auto res_px_ny = mat2d(height, width, 0);
		auto res_nx_py = mat2d(height, width, 0);
		auto res_nx_ny = mat2d(height, width, 0);
		auto max = -999.0;
		auto min = 999.0;
		const auto pi = std::acos(-1);

		for (std::size_t h = 0; h < height; ++h) {
			for (std::size_t w = 0; w < width; ++w) {
				auto x_way = -std::sin(std::acos(input[h][w][0]));
				auto y_way = std::sin(std::acos(input[h][w][1]));
				if ((0 < h) && (h < height - 1) && (0 < w) && (w < width - 1))
					res_px_py(h, w) = (res_px_py(h - 1, w) + std::move(x_way) + res_px_py(h, w - 1) + std::move(y_way)) / 2;
			}
		}
		
		
		for (std::size_t h = 0; h < height; ++h) {
			for (std::size_t w = width - 1; w > 0; --w) {
				auto x_way = -std::sin(std::acos(input[h][w][0]));
				auto y_way = std::sin(std::acos(input[h][w][1]));
				if ((0 < h) && (h < height - 1) && (0 < w) && (w < width - 1))
					res_px_ny(h, w) = (res_px_ny(h - 1, w) + std::move(x_way) + res_px_ny(h, w + 1) + std::move(y_way)) / 2;
			}
		}
		
		for (std::size_t h = height - 1; h < height; --h) {
			for (std::size_t w = 0; w < width; ++w) {
				auto x_way = -std::sin(std::acos(input[h][w][0]));
				auto y_way = std::sin(std::acos(input[h][w][1]));
				if ((0 < h) && (h < height - 1) && (0 < w) && (w < width - 1))
					res_nx_py(h, w) = (res_nx_py(h + 1, w) + std::move(x_way) + res_nx_py(h, w - 1) + std::move(y_way)) / 2;


			}
		}
		
		for (std::size_t h = height - 1; h < height; --h) {
			for (std::size_t w = width - 1; w > 0; --w) {
				auto x_way = -std::sin(std::acos(input[h][w][0]));
				auto y_way = std::sin(std::acos(input[h][w][1]));
				auto z_way = input[h][w][2];
				if (z_way > 0.9 || z_way < -0.9) z_way = {};
				if ((0 < h) && (h < height - 1) && (0 < w) && (w < width - 1))
					res_nx_ny(h, w) = (res_nx_ny(h + 1, w) + std::move(x_way) + res_nx_ny(h, w - 1) + std::move(y_way) + std::move(z_way)) / 2;

			}
		}
		

		//sums.emplace_back(res_px_py);
		//sums.emplace_back(res_px_ny);
		//sums.emplace_back(res_nx_py);
		sums.emplace_back(res_nx_ny); // this way is better

		auto output = mat2d(height, width, 0);
		for (std::size_t i = 0; i < sums.size(); ++i) {
			for (std::size_t h = 0; h < height; ++h) {
				for (std::size_t w = 0; w < width; ++w) {
					//auto add_x = std::abs(height - static_cast<double>(h - height) / 2);
					//auto add_y = std::abs(width - static_cast<double>(w - width) / 2);
					output(h, w) = output(h, w) + sums[i](h, w);
					getMaxMin(max, min, output(h, w));

				}
			}
		}
		
		
		for (std::size_t h = 0; h < height; ++h) {
			for (std::size_t w = 0; w < width; ++w) {
				output(h, w) = output(h, w) * 50 / (max - min);
			}
		}
		
		std::cout << "depth range is [ " << min << ", " << max << "] \t";
		
		return output;
	}

	void print_as_gray(mat2d input)
	{
		auto height = input.size1();
		auto width = input.size2();

		auto max = -999.0;
		auto min = 999.0;

		auto output = cv::Mat(height, width, CV_8U);
		for (auto h = 0; h < height; ++h) {
			for (auto w = 0; w < width; ++w) {
				auto res = input(h, w);
				getMaxMin(max, min, res);
				auto data = static_cast<char>(res);
				output.at<uchar>(h, w) = std::move(data);

			}
		}
		std::cout << "gray range is [ " << min << ", " << max << " ]\n";

		cv::imshow("output", output);
		cv::waitKey();
	}


	void output_pcl_to_dir(mat2d input, std::string dir_path)
	{
		auto save_path = dir_path + "output.ply";
		std::cout << "writing file to << " << save_path << '\n';

		std::ofstream outfile(save_path.c_str());
		outfile <<
			"ply\n"
			"format ascii 1.0\n"
			"comment alpha=1.0\n";
		outfile << "element vertex " << input.size1() * input.size2() << "\n";
		outfile <<
			"property float x\n"
			"property float y\n"
			"property float z\n"
			"property uchar red\n"
			"property uchar green\n"
			"property uchar blue z\n"
			"end_header\n"
			;

		for (std::size_t h = 0; h < input.size1(); ++h) {
			for (std::size_t w = 0; w < input.size2(); ++w) {
				auto depth = input(h, w);
				if (depth < -1 || depth > 1){
					outfile << h << ' ' << w << ' ' << input(h, w) << " 255 255 255\n";
				}
				else{
					outfile << h << ' ' << w << ' ' << input(h, w) << " 128 128 000\n";

				}
			}
		}
		
		outfile.close();
	}

}
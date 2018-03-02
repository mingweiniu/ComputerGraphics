#ifndef HW2_TOOL_HPP
#define HW2_TOOL_HPP

#include <string>
#include <vector>
#include <tuple>

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>
#include <boost/numeric/ublas/triangular.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/multi_array.hpp>

#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

namespace hw2{

	namespace bu = boost::numeric::ublas;
	using mat2d = bu::matrix<float>;
	using mat3d = boost::multi_array<float, 3>;
	// mytools
	bool InvertMatrix(const mat2d& input, mat2d& inverse);
	void printH(mat2d H);

	// homework processes
	void check_is_dir(std::string directory_path);
	std::vector<std::tuple<std::string, cv::Mat>> read_img_from_dir(std::string directory_path);
	std::tuple<std::vector<cv::KeyPoint>, cv::Mat> sift_img(cv::Mat image);
	std::vector<std::vector<std::tuple<float, int, int>>> findKNN(cv::Mat object, cv::Mat target, int KNN_K);
	mat2d ransac(
		std::vector<cv::KeyPoint> obj_kp,
		cv::Mat obj_des,
		std::vector<cv::KeyPoint> tar_kp,
		cv::Mat tar_des, 
		std::vector<std::vector<std::tuple<float, int, int>>> matches);
	// obj_xy_tar_uv
	mat2d get_H_mat(std::vector<std::tuple<float, float, float, float>> obj_tar);
	std::vector<std::vector<int>> permutation(std::vector<std::vector<int>> input);
	void forward_warping(mat2d H, std::string obj_path, std::string tar_path, std::string save_path);
	void backward_warping(mat2d H, std::string obj_path, std::string tar_path, std::string save_path);

}

#endif // !HW2_TOOL_HPP





//void cvMatPrint(cv::Mat matrix);
//void getMaxMin(double& Max, double& Min, double& input);
//mat2d pseudo_inverse(mat2d input);
//void print_as_RGB(mat3d input);
//void print_as_gray(mat2d input);
//double mat3_det(mat2d input);

//double mat3_det(mat2d input)
//{
//	auto a1 = input(0, 0) * input(1, 1) *input(2, 2);
//	auto a2 = input(0, 1) * input(1, 2) *input(2, 0);
//	auto a3 = input(1, 0) * input(0, 2) *input(2, 1);
//	auto a4 = - input(0, 2) * input(1, 1) *input(2, 0);
//	auto a5 = - input(0, 1) * input(1, 0) *input(2, 2);
//	auto a6 = - input(0, 0) * input(1, 2) *input(2, 1);

//	return a1+a2+a3+a4+a5+a6;
//}






//void cvMatPrint(cv::Mat matrix) {
//	for (int rowIndex = 0; rowIndex < matrix.rows; rowIndex++) {
//		std::cout << '\t';
//		for (int colIndex = 0; colIndex < matrix.cols; colIndex++) {
//			std::cout << matrix.at<float>(rowIndex, colIndex) << '\t';
//		}
//		std::cout << '\n';
//	}
//
//}
//
//void getMaxMin(double& Max, double& Min, double& input)
//{
//	Max = std::max(Max, input);
//	Min = std::min(Min, input);
//}
//
//mat2d pseudo_inverse(mat2d input) {
//	std::cout << "input: \n" << input << '\n';
//	auto row = input.size1();
//
//	bool matrix_is_wide = input.size1() < input.size2();
//
//	auto small_side = std::min(input.size1(), input.size2());
//	auto large_side = std::max(input.size1(), input.size2());
//
//	auto wide_mat = mat2d(large_side, small_side);
//	auto long_mat = mat2d(small_side, large_side);
//
//	if (matrix_is_wide) {
//		wide_mat = input;
//		long_mat = bu::trans(input);
//	}
//	else {
//		wide_mat = bu::trans(input);
//		long_mat = input;
//	}
//
//
//	mat2d AAT(small_side, small_side);
//	AAT = prod(wide_mat, long_mat);
//	std::cout << "AAT : \n" << AAT << '\n';
//
//	mat2d inv_AAT(small_side, small_side);
//	InvertMatrix(AAT, inv_AAT);
//	std::cout << "AAT_inv : \n" << inv_AAT << '\n';
//
//	auto pseudo_inv = mat2d(input.size2(), input.size1());
//	if (matrix_is_wide) {
//		pseudo_inv = bu::prod(long_mat, inv_AAT);
//	}
//	else {
//		pseudo_inv = bu::prod(inv_AAT, wide_mat);
//	}
//	std::cout << "pseudo_inv : \n" << pseudo_inv << '\n';
//
//	return pseudo_inv;
//}
//
//void print_as_RGB(mat3d input)
//{
//	const bool print_xyz_separate = true;
//	auto height = input.size();
//	auto width = input[0].size();
//
//	auto max = -999.0;
//	auto min = 999.0;
//	if (!print_xyz_separate) {
//		auto output = cv::Mat(height, width, CV_32FC3);
//		for (auto h = 0; h < height; ++h) {
//			for (auto w = 0; w < width; ++w) {
//				for (auto i = 0; i < 3; ++i) {
//					auto res = input[h][w][i];
//					getMaxMin(max, min, res);
//					output.at<cv::Vec3f>(h, w)[i] = std::move(res);
//				}
//			}
//		}
//		cv::imshow("output", output);
//		cv::waitKey();
//	}
//	else
//	{
//		auto output = cv::Mat(input.size() * 3, input[0].size() * 3, CV_32FC3);
//		for (std::size_t h = 0; h < height; ++h) {
//			for (std::size_t w = 0; w < input[0].size(); ++w) {
//				for (std::size_t i = 0; i < 3; ++i) {
//					auto res = input[h][w][i];
//					getMaxMin(max, min, res);
//					output.at<cv::Vec3f>(h + height * i, w)[i] = std::move(res);
//				}
//			}
//		}
//		for (std::size_t i = 0; i < 3; ++i) {
//			for (std::size_t h = 1; h < height - 1; ++h) {
//				for (std::size_t w = 1; w < width - 1; ++w) {
//					auto sub_x = static_cast<float>(input[h][w][i] - input[h - 1][w][i]);
//					auto sub_y = static_cast<float>(input[h][w][i] - input[h][w - 1][i]);
//
//					output.at<cv::Vec3f>(h + height * i, width * 1 + w)[i] = std::move(sub_x);
//					output.at<cv::Vec3f>(h + height * i, width * 2 + w)[i] = std::move(sub_y);
//				}
//			}
//		}
//		std::cout << "data range is [ " << min << ", " << max << " ]\n";
//		cv::imshow("output", output);
//		cv::waitKey();
//	}
//}
//
//void print_as_gray(mat2d input)
//{
//	auto height = input.size1();
//	auto width = input.size2();
//
//	auto max = -999.0;
//	auto min = 999.0;
//
//	auto output = cv::Mat(height, width, CV_8U);
//	for (auto h = 0; h < height; ++h) {
//		for (auto w = 0; w < width; ++w) {
//			auto res = input(h, w);
//			getMaxMin(max, min, res);
//			auto data = static_cast<char>(res);
//			output.at<uchar>(h, w) = std::move(data);
//
//		}
//	}
//	std::cout << "gray range is [ " << min << ", " << max << " ]\n";
//
//	cv::imshow("output", output);
//	cv::waitKey();
//}
#ifndef HW1_TOOL_HPP
#define HW1_TOOL_HPP

#include <string>
#include <vector>

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

namespace hw1{

	namespace bu = boost::numeric::ublas;
	using mat2d = bu::matrix<double>;
	using mat3d = boost::multi_array<double, 3>;

	// mytools
	bool InvertMatrix(const mat2d& input, mat2d& inverse);
	void check_is_dir(std::string directory_path);
	std::vector<std::string> txtfile(std::string file_path);	
	void cvMatPrint(cv::Mat matrix);
	void getMaxMin(double& Max, double& Min, double& input);
	
	// homework processes
	mat2d read_light(std::string file_path);
	mat2d pseudo_inverse(mat2d input);
	std::vector<mat2d> read_img_from_dir(std::string directory_path);
	void repair_img(mat2d& input);
	mat3d lightInv_mul_images(mat2d light_inv, std::vector<mat2d>images);
	void print_as_RGB(mat3d input);
	mat2d calculate_depth(mat3d input);
	void print_as_gray(mat2d input);
	void output_pcl_to_dir(mat2d input, std::string dir_path);

}

#endif // !HW1_TOOL_HPP
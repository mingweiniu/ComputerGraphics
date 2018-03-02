#ifdef _MSC_VER
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif //!_CRT_SECURE_NO_WARNINGS
#endif //_MSC_VER

#include <cstdlib>

#include <utility>
#include <algorithm>
#include <iostream>

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/multi_array.hpp>

#include "hw1/hw1_tool.hpp"

std::vector<std::string> argvs(int argc, char** argv);
void help();

// debug in VS14
//#define DEBUG_IN_VS14

int main(int argc, char** argv)
{
#if defined(DEBUG_IN_VS14)
	bool using_debug_mode = true;
	std::string target_path{ "..\\test\\special\\bunny\\" };
#else	
	auto inputs = argvs(argc, argv);
	std::string target_path{};
	static bool using_debug_mode = false;
	if (inputs[1] == "-R") {
		using_debug_mode = false;
		target_path = target_path + inputs[2];
	}
	if (inputs[1] == "-D") {
		using_debug_mode = true;
		target_path = target_path + inputs[2];
	}
#endif

	namespace bu = boost::numeric::ublas;
	using mat2d = bu::matrix<double>;
	using mat3d = boost::multi_array<double, 3>;

	//
	// read light matrix from directory
	//
	auto light_vec = hw1::read_light(target_path);

	//
	// calculate inverse matrix of light vectors	
	//
	auto light_inv = mat2d(light_vec.size2(), light_vec.size1());
	light_inv = hw1::pseudo_inverse(light_vec);

	//
	// read *.bmp pictures
	//
	auto images = hw1::read_img_from_dir(target_path);

	//
	// repair the images
	//
	for (auto& image : images) {
		hw1::repair_img(image);
	}

	//
	// calculate all images with light inverse into normals
	//
	auto img_normals = hw1::lightInv_mul_images(light_inv, images);

	//
	// output normals, partial x , partial y as RGB
	// not necessary
	//
	if (using_debug_mode)
		hw1::print_as_RGB(img_normals);


	//
	// calculate depth by summation  different direction and average
	//
	std::cout << "img_depth" << '\n';
	auto img_depth = hw1::calculate_depth(img_normals);

	//
	// output depth as grayscale
	// not necessary
	//
	if (using_debug_mode)
		hw1::print_as_gray(img_depth);


	//
	// output as pcl
	//
	hw1::output_pcl_to_dir(img_depth, target_path);

	if (using_debug_mode)
		system("PAUSE");


}


std::vector<std::string> argvs(int argc, char** argv){
	std::vector<std::string> inputs;
	inputs.reserve(argc);
	if (argc == 1) {
		std::cout << " please use --help \n";
		std::exit(EXIT_SUCCESS);
	}
	for (int i = 0; i < argc; ++i) {
		inputs.push_back(argv[i]);
	}
	if (inputs[1] == "--help" || inputs[1] == "--h" || inputs[1] == "-h") {
		help();
	}

	return inputs;
}

void help()
{
	const char* helpinfo =
		"Usage :\n"
		"\topencv_hw1 [options] <path to model>\n"
		"Options :\n"
		"\t --help, --h, -h         = Print this info and exit\n"
		"\t -R                      = Read Directory and generate \"output.ply\" \n"
		"\t -D                      = use debug mode \n"
		"\n"
		"Example : \n"
		"\tLinux : \n"
		"\t\t./opencv_hw1 -R ../../test/bunny/ \n"
		"\tWin10 : \n"
		"\t\t.\\opencv_hw1.exe -R ..\\..\\test\\bunny\\ \n"
		"\n"
		;
	
	std::cout << helpinfo;
	std::exit(EXIT_SUCCESS);
}


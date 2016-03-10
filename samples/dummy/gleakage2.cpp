#include <iostream>
#include <string>
#include "GLeakage.h"

void gleakage2() {
	std::cout << "Using LIBQIF Library Example" << std::endl;

	std::string channel_elements = "0.3 0.7; 0.7 0.3; 0.3 0.7";
	chan C = chan(channel_elements);

	std::string function_elements = "1 0 0; 0 1 0; 0 0 1";
	mat g = mat(function_elements);

	//Creating the probability distribution vectors
	std::string vector1_elements = "0.33333 0.33333 0.33334";
	std::string vector2_elements = "0 0.5 0.5";
	std::string vector3_elements = "0.5 0.5 0";
	std::string vector4_elements = "0.25 0.5 0.25";

	prob p1 = prob(vector1_elements);
	prob p2 = prob(vector2_elements);
	prob p3 = prob(vector3_elements);
	prob p4 = prob(vector4_elements);

	//GLeakage<double>
	GLeakage<double> gl = GLeakage<double>(C, g);

	//calculating measures
	double Lg1 = gl.leakage(p1);
	double Lg2 = gl.leakage(p2);
	double Lg3 = gl.leakage(p3);
	double Lg4 = gl.leakage(p4);

	std::cout << "Lg p1" << Lg1 << std::endl;
	std::cout << "Lg p2" << Lg2 << std::endl;
	std::cout << "Lg p3" << Lg3 << std::endl;
	std::cout << "Lg p4" << Lg4 << std::endl;
}

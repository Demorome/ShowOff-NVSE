#pragma once
#include <armadillo>
#include <cfloat>


/*When comparing two float values for equality, due to internal conversions between singleand double precision,
 *it's better to check if the absolute difference is less than epsilon (0.0001)
 *-Jazzisparis
 *May not be needed if the Double Precision Fix from JIP LN is enabled. */
DEFINE_COMMAND_ALT_PLUGIN(Flt_Equals, Float_Equals, "Returns true if the absolute difference between two floats is less than epsilon (0.0001)", false, kParams_TwoDoubles);
bool Cmd_Flt_Equals_Execute(COMMAND_ARGS)
{
	double a, b;
	*result = false;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &a, &b)) 
		return true;
	if (abs(a - b) < FLT_EPSILON)
		*result = true;
	return true;
}


#if _DEBUG

DEFINE_COMMAND_PLUGIN(TestMatrix, "debug matrix function", false, NULL);

bool Cmd_TestMatrix_Execute(COMMAND_ARGS)
{
	// Initialize the random generator
	arma::arma_rng::set_seed_random();

	std::stringstream stream;

	// Create a 4x4 random matrix and print it on the screen
	arma::Mat<double> A = arma::randu(4, 4);
	stream << "A:\n" << A << "\n";

	// Multiply A with his transpose:
	stream << "A * A.t() =\n";
	stream << A * A.t() << "\n";

	// Access/Modify rows and columns from the array:
	A.row(0) = A.row(1) + A.row(3);
	A.col(3).zeros();
	stream << "add rows 1 and 3, store result in row 0, also fill 4th column with zeros:\n";
	stream << "A:\n" << A << "\n";

	arma::Mat<double>B = arma::diagmat(A);
	stream << "B:\n" << B << "\n";

	Console_Print_Long(stream.str());

	// Save matrices A and B:
	A.save("A_mat.txt", arma::arma_ascii);
	B.save("B_mat.txt", arma::arma_ascii);

	return true;
}


#endif

#pragma once
#include <armadillo>
#include <cfloat>
#include <optional>
#include "jip_nvse.h"


#if _DEBUG

/*
std::optional<arma::Mat<double>> GetMatrixFromArray(ArrayData_JIP &arrData)
{
	if (arrData.size <= 0)
		return {};
	std::vector<double*> values;
	size_t n_cols = 0, n_rows = 0;
	bool is2D = false;
	for (int i = arrData.size - 1; i >= 0; i--)
	{
		if (arrData.vals[i].GetType() == NVSEArrayVarInterface::kType_Array)
		{
			is2D = true;
			for (int i = arrData.size - 1; i >= 0; i--)
			{
				
			}
		}
		else
			values.emplace_front(&arrData.vals[i].num);
	}

	if (is2D)
		n_rows = arrData.size;
	else
	{
		n_rows = 1;
		n_cols = arrData.size;
	}

	if (n_cols > 0 && n_rows > 0)
	{
		arma::mat matrix(*values.data(), n_rows, n_cols, false, true);
		return matrix;
	}
	return {};
}
*/

std::optional<arma::Mat<double>> GetMatrixFromArray(NVSEArrayVar* arr)
{
	ArrayData const arrData(arr, true);	//assume isPacked
	if (arrData.size <= 0)
		return {};
	
	arma::Mat<double> matrix;
	if (bool const is2D = arrData.vals[0].GetType() == NVSEArrayVarInterface::kType_Array;
		is2D == true)
	{
		int n_cols = -1;
		for (int i = 0; i < arrData.size; i++)	//assume each elem is an array.
		{
			ArrayData const innerArrData(arrData.vals[i].Array(), true);
			if (innerArrData.size <= 0)
				return {};
			
			if (n_cols == -1)
				n_cols = innerArrData.size;
			else if (n_cols != innerArrData.size)	// if inner arrays don't have matching sizes.
				return {};
			
			//Populate current row with column data contained in internal (row) array.
			arma::Row<double> NthRow(innerArrData.size);
			for (int k = 0; k < innerArrData.size; k++)	//assume each elem is a number.
			{
				NthRow(k) = innerArrData.vals[k].Number();
			}
			matrix.insert_rows(i, NthRow);
		}
	}
	else //1D array, which is only a single row (each element is a column).
	{
		arma::Row<double> row(arrData.size);
		for (int i = 0; i < arrData.size; i++)
		{
			row(i) = arrData.vals[i].Number();
		}
		matrix = row;
	}
	return matrix;
}

NVSEArrayVar* GetMatrixAsArray(arma::Mat<double>& matrix, Script* callingScript)
{
	if (matrix.empty())
		return nullptr;
	
	NVSEArrayVar* resArr = g_arrInterface->CreateArray(nullptr, 0, callingScript);
	if (matrix.is_rowvec())	// create 1D array
	{
		auto const iter_end = matrix.end_row(0);
		for (auto iter = matrix.begin_row(0); iter != iter_end; iter++)
		{
			g_arrInterface->AppendElement(resArr, ArrayElementR(*iter));
		}
	}
	else //create 2D array (starting with rows as internal arrays)
	{
		auto CreateInternalArray = [&](arma::rowvec& row) {
			NVSEArrayVar* innerArr = g_arrInterface->CreateArray(nullptr, 0, callingScript);
			row.for_each([&innerArr](arma::mat::elem_type& val) { g_arrInterface->AppendElement(innerArr, ArrayElementR(val)); });
			g_arrInterface->AppendElement(resArr, ArrayElementR(innerArr));
		};
		matrix.each_row(CreateInternalArray);
	}
	return resArr;
}

DEFINE_COMMAND_PLUGIN(Matrix_Multiply, "Returns the matrix multiplication result of two matrix arrays.", false, kParams_TwoArrayIDs);
bool Cmd_Matrix_Multiply_Execute(COMMAND_ARGS)
{
	*result = 0;	// resulting matrix
	UInt32 arrA_ID, arrB_ID;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &arrA_ID, &arrB_ID))
		return true;
	auto const arrA = g_arrInterface->LookupArrayByID(arrA_ID);
	auto const arrB = g_arrInterface->LookupArrayByID(arrB_ID);
	if (arrA && arrB)
	{
		if (int const typeA = g_arrInterface->GetContainerType(arrA); 
			typeA == NVSEArrayVarInterface::kArrType_Array && typeA == g_arrInterface->GetContainerType(arrB))
		{
			auto matrixA = GetMatrixFromArray(arrA);
			auto matrixB = GetMatrixFromArray(arrB);
			if (matrixA && matrixB)
			{
				try{
					arma::Mat<double> resMatrix = (*matrixA) * (*matrixB);
					if (auto const matrixAsArray = GetMatrixAsArray(resMatrix, scriptObj))
						g_arrInterface->AssignCommandResult(matrixAsArray, result);
				}
				catch (std::logic_error&err){	//invalid matrix sizes for multiplication
					return true;
				}
			}
		}
	}
	return true;
}

DEFINE_COMMAND_PLUGIN(Matrix_IsMatrix, "Checks if an array is convertible to a matrix.", false, kParams_OneArrayID);
bool Cmd_Matrix_IsMatrix_Execute(COMMAND_ARGS)
{
	*result = false;	// isMatrix (bool)
	UInt32 arrID;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &arrID))
		return true;
	auto const arrA = g_arrInterface->LookupArrayByID(arrID);
	if (auto const matrix = GetMatrixFromArray(arrA))
		*result = true;
	return true;
}

DEFINE_COMMAND_PLUGIN(Matrix_Dump, "Dumps the matrix array in console, in matrix notation.", false, kParams_OneArrayID);
bool Cmd_Matrix_Dump_Execute(COMMAND_ARGS)
{
	UInt32 arrID;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &arrID))
		return true;
	auto const arrA = g_arrInterface->LookupArrayByID(arrID);
	if (auto const matrix = GetMatrixFromArray(arrA))
	{
		Console_Print("** Dumping Array %u as Matrix **", arrID);
		std::stringstream stream;
		stream << (*matrix);
		Console_Print_Long(stream.str());
	}
	else
	{
		Console_Print("Matrix_Dump >> Array %u is not a valid Matrix.", arrID);
	}
	return true;
}

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

/*When comparing two float values for equality, due to internal conversions between singleand double precision,
 *it's better to check if the absolute difference is less than epsilon (0.0001)
 *
 *(The Double Precision Fix from JIP LN) alleviates the problem with operator ==,
 *but doesn't eliminate it entirely. Checking absolute difference against an epsilon is far more computationally expensive than a simple comparison, obviously
 *-Jazzisparis */
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

#endif

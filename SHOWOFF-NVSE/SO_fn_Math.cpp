#include "SO_fn_Math.h"
#include <cfloat>
#include "easing.h"





bool Cmd_ApplyEasing_Execute(COMMAND_ARGS)
{
	*result = -1;
	double input;	//should be between 0-1
	char funcName[0x25];	//choose easing function, ex: "sine", "Quad". Non-case sensitive.
	EasingMode mode;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &input, &funcName, &mode) || mode > kModeMax)
		return true;
	std::string funcNameStr{ funcName };
	std::ranges::for_each(funcNameStr, [](char& c) { c = tolower(c); });
	if (auto const funcs = GetEasingFuncsFromStr<double>(funcNameStr);
		funcs[mode])
	{
		*result = funcs[mode](input);
	}
	return true;
}

bool Cmd_ApplyEasingAlt_Execute(COMMAND_ARGS)
{
	*result = -1;
	double input;
	EasingFuncs funcNum;
	EasingMode mode;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &input, &funcNum, &mode) || mode > kModeMax)
		return true;
	if (auto const funcs = GetEasingFuncsFromNum<double>(funcNum);
		funcs[mode])
	{
		*result = funcs[mode](input);
	}
	return true;
}


template <typename T>
std::pair<size_t, size_t> array_2d<T>::GetDimensions() const
{
	auto const num_rows = numbers.size();
	size_t const num_cols = 0;
	if (num_rows > 0)
	{
		num_cols = numbers[0].size();
	}
	return { num_rows, num_cols };
}

template <typename T>
NVSEArrayVar* array_2d<T>::CreateArray(Script* callingScript)
{
	if (IsEmpty())
		return nullptr;

	std::vector<ArrayElementR> elems;

	//Make a sub-array for each row.
	//Inspired by https://thispointer.com/how-to-print-two-dimensional-2d-vector-in-c/
	std::for_each(numbers.begin(), numbers.end(), [&](const auto& row) {
		std::vector<ArrayElementR> rowElems;
		std::for_each(row.begin(), row.end(), [&rowElems](const auto& elem) {
			rowElems.emplace_back(elem);
			});
		elems.emplace_back(g_arrInterface->CreateArray(rowElems.data(), rowElems.size(), callingScript));
		});
	return g_arrInterface->CreateArray(elems.data(), elems.size(), callingScript);
}

template <typename T, size_t rows, size_t cols>
array_2d<T> process_2d_array(const T(&array)[rows][cols])
{
	array_2d<T> ret;
	ret.numbers.resize(rows);
	for (size_t i = 0; i < rows; ++i)
	{
		for (size_t j = 0; j < cols; ++j)
		{
			ret.numbers[i].emplace_back(array[i][j]);
		}
	}
	return ret;
}

template <typename T>
std::optional<array_2d<T>> TryGetArrayNumbers(NVSEArrayVar* arr, size_t const maxCols, size_t const minCols,
	size_t const maxRows, size_t const minRows)
{
	if (!arr) return {};
	ArrayData const arrData(arr, true); //assume isPacked
	if (arrData.size <= 0)
		return {};

	array_2d<T> matData;
	size_t num_rows, num_cols = 0;
	if (arrData.vals[0].GetType() == NVSEArrayVarInterface::kType_Array) //is2D array
	{
		if (maxRows != 0 && arrData.size > maxRows)
			return {};
		num_rows = arrData.size;
		matData.numbers.resize(num_rows);
		for (int i = 0; i < num_rows; i++)
		{
			ArrayData const innerArrData(arrData.vals[i].Array(), true);
			if (innerArrData.size <= 0 || (maxCols && innerArrData.size > maxCols))
				return {};

			if (!num_cols)
				num_cols = innerArrData.size;
			else if (num_cols != innerArrData.size) // inner arrays must have matching sizes.
				return {};

			//Get row data from internal array.
			for (int k = 0; k < num_cols; k++)
			{
				matData.numbers[k].emplace_back(innerArrData.vals[k].num);
			}
		}
	}
	else //1D array, only a single row (each element is a column).
	{
		num_rows = 1;
		if (maxCols && arrData.size > maxCols)
			return {};
		num_cols = arrData.size;
		matData.numbers.resize(num_rows);
		for (int i = 0; i < num_cols; i++)
		{
			matData.numbers[0].emplace_back(arrData.vals[i].num);
		}
	}
	if ((minRows && num_rows > minRows) || (minCols && num_cols > minCols))
		return {};
	return matData;
}


std::optional<NiMatrix33> Get3x3MatrixFromArray(NVSEArrayVar* arr)
{
	if (auto matData = TryGetArrayNumbers<float>(arr, 3, 3, 3, 3))
	{
		NiMatrix33 mat3x3((*matData).numbers[0].data(), (*matData).numbers[1].data(), (*matData).numbers[2].data());
		return mat3x3;
	}
	return {};
}

std::optional<NiQuaternion> GetQuatFromArray(NVSEArrayVar* arr)
{
	if (auto quatData = TryGetArrayNumbers<float>(arr, 4, 4, 1, 1))
	{
		NiQuaternion quat(quatData.value().numbers[0].data());
		return quat;
	}
	return {};
}

//assume non-nullptr
bool ArrayIsMatrix(NVSEArrayVar* arr)
{
	if (g_arrInterface->GetContainerType(arr) != NVSEArrayVarInterface::kArrType_Array)
		return false;
	ArrayData const arrData(arr, true);	//assume isPacked
	if (arrData.size <= 0)
		return false;

	//check if each element in the (potentially 2D) array is number-type.
	if (bool const is2D = arrData.vals[0].GetType() == NVSEArrayVarInterface::kType_Array;
		is2D == true)
	{
		size_t numElemsInRows = 0;
		for (int iRow = 0; iRow < arrData.size; iRow++)
		{
			ArrayData const rowData(arrData.vals[iRow].Array(), true);
			if (rowData.size <= 0)
				return false;

			//ensure each row has the same number of elements.
			if (numElemsInRows) {
				if (rowData.size != numElemsInRows)
					return false;
			}
			else
				numElemsInRows = rowData.size;

			for (int iCol = 0; iCol < rowData.size; iCol++)
			{
				if (rowData.vals[iCol].GetType() != NVSEArrayVarInterface::kType_Numeric)
					return false;
			}
		}
	}
	else //1D
	{
		for (int iCol = 0; iCol < arrData.size; iCol++)
		{
			if (arrData.vals[iCol].GetType() != NVSEArrayVarInterface::kType_Numeric)
				return false;
		}
	}
	return true;
}

std::optional<arma::Mat<double>> GetMatrixFromArray(NVSEArrayVar* arr)
{
	if (!arr) return {};
	ArrayData const arrData(arr, true);	//assume isPacked
	if (arrData.size <= 0)
		return {};

	arma::Mat<double> matrix;
	if (bool const is2D = arrData.vals[0].GetType() == NVSEArrayVarInterface::kType_Array;
		is2D == true)
	{
		int n_cols = -1;
		for (int iRow = 0; iRow < arrData.size; iRow++)	//assume each elem is an array.
		{
			ArrayData const innerArrData(arrData.vals[iRow].Array(), true);
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
			matrix.insert_rows(iRow, NthRow);
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

NVSEArrayVar* QuatToArray(NiQuaternion const& quat, Script* callingScript)
{
	ArrayElementR elems[] = { {quat.w}, {quat.x}, {quat.y}, {quat.z} };
	return g_arrInterface->CreateArray(elems, 4, callingScript);
}




arma::Mat<double> Mat_ApplyOpWithMat(arma::Mat<double>& matA, const char op, arma::Mat<double>& matB)
{
	switch (op)
	{
	case '+':
		return matA + matB;
	case '-':
		return matA - matB;
	case '%':
		return matA % matB;
	case '/':
		return matA / matB;
	case '*':
		return matA * matB;
	}
	return {};
}

bool Cmd_Matrix_ApplyOperationWithMatrix_OLD_Execute(COMMAND_ARGS)
{
	*result = 0;	// resulting matrix
	UInt32 arrA_ID, arrB_ID;
	char op[3];
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &arrA_ID, op, &arrB_ID))
		return true;
	auto matrixA = GetMatrixFromArray(g_arrInterface->LookupArrayByID(arrA_ID));
	auto matrixB = GetMatrixFromArray(g_arrInterface->LookupArrayByID(arrB_ID));
	if (matrixA && matrixB)
	{
		try {
			auto resMatrix = Mat_ApplyOpWithMat(matrixA.value(), op[0], matrixB.value());
			if (auto const matrixAsArray = GetMatrixAsArray(resMatrix, scriptObj))
				g_arrInterface->AssignCommandResult(matrixAsArray, result);
		}
		catch (std::logic_error&) {	//invalid matrix sizes for multiplication
			return true;
		}
	}
	return true;
}
bool Cmd_Matrix_ApplyOperationWithMatrix_Execute(COMMAND_ARGS)
{
	*result = 0;	// resulting matrix
	if (PluginExpressionEvaluator eval(PASS_COMMAND_ARGS);
		eval.ExtractArgs())
	{
		auto matrixA = GetMatrixFromArray(eval.GetNthArg(0)->GetArrayVar());
		std::string const sOp{ eval.GetNthArg(1)->GetString() };
		auto matrixB = GetMatrixFromArray(eval.GetNthArg(2)->GetArrayVar());
		if (matrixA && matrixB)
		{
			try {
				auto resMatrix = Mat_ApplyOpWithMat(matrixA.value(), sOp[0], matrixB.value());
				if (auto const matrixAsArray = GetMatrixAsArray(resMatrix, scriptObj))
					g_arrInterface->AssignCommandResult(matrixAsArray, result);
			}
			catch (std::logic_error&) {	//invalid matrix sizes for multiplication
				return true;
			}
		}
	}
	return true;
}

arma::Mat<double> Mat_ApplyOpWithScal(arma::Mat<double>& mat, const char op, double scalar)
{
	switch (op)
	{
	case '+':
		return mat + scalar;
	case '-':
		return mat - scalar;
	case '/':
		return mat / scalar;
	case '*':
		return mat * scalar;
	}
	return {};
}

bool Cmd_Matrix_ApplyOperationWithScalar_OLD_Execute(COMMAND_ARGS)
{
	*result = 0;	// resulting matrix
	UInt32 arrID;
	char op[3];
	double scalar;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &arrID, op, &scalar))
		return true;
	if (auto matrix = GetMatrixFromArray(g_arrInterface->LookupArrayByID(arrID)))
	{
		auto resMatrix = Mat_ApplyOpWithScal(matrix.value(), op[0], scalar);
		if (auto const matrixAsArray = GetMatrixAsArray(resMatrix, scriptObj))
			g_arrInterface->AssignCommandResult(matrixAsArray, result);
	}
	return true;
}
bool Cmd_Matrix_ApplyOperationWithScalar_Execute(COMMAND_ARGS)
{
	*result = 0;	// resulting matrix
	if (PluginExpressionEvaluator eval(PASS_COMMAND_ARGS);
		eval.ExtractArgs())
	{
		if (auto matrix = GetMatrixFromArray(eval.GetNthArg(0)->GetArrayVar()))
		{
			std::string const sOp{ eval.GetNthArg(1)->GetString() };
			auto const scalar = eval.GetNthArg(2)->GetFloat();

			auto resMatrix = Mat_ApplyOpWithScal(matrix.value(), sOp[0], scalar);
			if (auto const matrixAsArray = GetMatrixAsArray(resMatrix, scriptObj))
				g_arrInterface->AssignCommandResult(matrixAsArray, result);
		}
	}
	return true;
}

bool Cmd_Matrix_Transpose_OLD_Execute(COMMAND_ARGS)
{
	*result = 0;	// transposeArr
	UInt32 arrID;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &arrID))
		return true;
	if (auto const matrix = GetMatrixFromArray(g_arrInterface->LookupArrayByID(arrID)))
	{
		arma::Mat<double> transpose = matrix.value().t();
		g_arrInterface->AssignCommandResult(GetMatrixAsArray(transpose, scriptObj), result);
	}
	return true;
}
bool Cmd_Matrix_Transpose_Execute(COMMAND_ARGS)
{
	*result = 0;	// transposeArr
	if (PluginExpressionEvaluator eval(PASS_COMMAND_ARGS);
		eval.ExtractArgs())
	{
		if (auto const matrix = GetMatrixFromArray(eval.GetNthArg(0)->GetArrayVar()))
		{
			arma::Mat<double> transpose = matrix.value().t();
			g_arrInterface->AssignCommandResult(GetMatrixAsArray(transpose, scriptObj), result);
		}
	}
	return true;
}

bool Cmd_Matrix_IsMatrix_OLD_Execute(COMMAND_ARGS)
{
	*result = false;	// isMatrix (bool)
	UInt32 arrID;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &arrID))
		return true;
	if (auto const arr = g_arrInterface->LookupArrayByID(arrID))
	{
		*result = ArrayIsMatrix(arr);
	}
	return true;
}
bool Cmd_Matrix_IsMatrix_Execute(COMMAND_ARGS)
{
	*result = false;	// isMatrix (bool)
	if (PluginExpressionEvaluator eval(PASS_COMMAND_ARGS);
		eval.ExtractArgs())
	{
		if (auto const arr = eval.GetNthArg(0)->GetArrayVar())
		{
			*result = ArrayIsMatrix(arr);
		}
	}
	return true;
}

bool Cmd_Matrix3x3_GetQuaternion_OLD_Execute(COMMAND_ARGS)
{
	*result = 0;	// resulting matrix
	UInt32 arrID;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &arrID))
		return true;
	if (auto matrix = Get3x3MatrixFromArray(g_arrInterface->LookupArrayByID(arrID)))
	{
		const NiQuaternion quat{ *matrix };
		g_arrInterface->AssignCommandResult(QuatToArray(quat, scriptObj), result);
	}
	return true;
}
bool Cmd_Matrix3x3_GetQuaternion_Execute(COMMAND_ARGS)
{
	*result = 0;	// resulting matrix
	if (PluginExpressionEvaluator eval(PASS_COMMAND_ARGS);
		eval.ExtractArgs())
	{
		if (auto matrix = Get3x3MatrixFromArray(eval.GetNthArg(0)->GetArrayVar()))
		{
			const NiQuaternion quat{ *matrix };
			g_arrInterface->AssignCommandResult(QuatToArray(quat, scriptObj), result);
		}
	}
	return true;
}

bool Cmd_Quaternion_GetMatrix_OLD_Execute(COMMAND_ARGS)
{
	*result = 0;	// resulting matrix
	UInt32 arrID;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &arrID))
		return true;
	if (auto quat = GetQuatFromArray(g_arrInterface->LookupArrayByID(arrID)))
	{
		const NiMatrix33 mat{ *quat };
		auto arr = process_2d_array<float, 3, 3>(mat.cr);
		g_arrInterface->AssignCommandResult(arr.CreateArray(scriptObj), result);
	}
	return true;
}
bool Cmd_Quaternion_GetMatrix_Execute(COMMAND_ARGS)
{
	*result = 0;	// resulting matrix
	if (PluginExpressionEvaluator eval(PASS_COMMAND_ARGS);
		eval.ExtractArgs())
	{
		if (auto quat = GetQuatFromArray(eval.GetNthArg(0)->GetArrayVar()))
		{
			const NiMatrix33 mat{ *quat };
			auto arr = process_2d_array<float, 3, 3>(mat.cr);
			g_arrInterface->AssignCommandResult(arr.CreateArray(scriptObj), result);
		}
	}
	return true;
}

void Matrix_Dump_Call(const std::optional<arma::Mat<double>> &matrix, UInt32 arrID)
{
	if (matrix)
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
}

bool Cmd_Matrix_Dump_OLD_Execute(COMMAND_ARGS)
{
	UInt32 arrID;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &arrID))
	{
		Console_Print("Matrix_Dump >> ERROR, unable to extract arguments");
		return true;
	}
	auto const arrA = g_arrInterface->LookupArrayByID(arrID);
	Matrix_Dump_Call(GetMatrixFromArray(arrA), reinterpret_cast<UInt32>(arrA));
	return true;
}
bool Cmd_Matrix_Dump_Execute(COMMAND_ARGS)
{
	if (PluginExpressionEvaluator eval(PASS_COMMAND_ARGS);
		eval.ExtractArgs())
	{
		auto const arrA = eval.GetNthArg(0)->GetArrayVar();
		Matrix_Dump_Call(GetMatrixFromArray(arrA), reinterpret_cast<UInt32>(arrA));
	}
	else
	{
		Console_Print("Matrix_Dump >> ERROR, unable to extract arguments");
	}
	return true;
}












#if _DEBUG


bool Cmd_TestMatrix_Execute(COMMAND_ARGS)
{
	NiMatrix33 mat = 
	{	1, 2, 3,
		1, 2, 3,
		1, 2, 3 };
	mat.Dump();

	NiQuaternion quat = { mat };
	quat.Dump();

	// should be the same as the old Mat...
	NiMatrix33 newMat = { quat };
	newMat.Dump();

	//should be the same as the old Quat...
	NiQuaternion newQuat = { newMat };
	newQuat.Dump();

	newMat = { newQuat };
	newMat.Dump();

	newQuat = { newMat };
	newQuat.Dump();
	
	/*
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
	*/

	return true;
}



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

#endif _DEBUG
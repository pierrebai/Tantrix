#pragma once

#ifndef DAK_TANTRIX_TESTS_HELPERS_H
#define DAK_TANTRIX_TESTS_HELPERS_H

#include "tantrix.h"
#include "tantrix_stream.h"

#include "CppUnitTest.h"

namespace Microsoft::VisualStudio::CppUnitTestFramework
{
	template<>
	inline std::wstring ToString<dak::tantrix::color_t>(const dak::tantrix::color_t& t)
	{
		using namespace dak::tantrix;
		RETURN_WIDE_STRING(t);
	}

	template<>
	inline std::wstring ToString<dak::tantrix::direction_t>(const dak::tantrix::direction_t& t)
	{
		using namespace dak::tantrix;
		RETURN_WIDE_STRING(t);
	}

	template<>
	inline std::wstring ToString<dak::tantrix::position_t>(const dak::tantrix::position_t& t)
	{
		using namespace dak::tantrix;
		RETURN_WIDE_STRING(t);
	}

	template<>
	inline std::wstring ToString<dak::tantrix::solution_t>(const dak::tantrix::solution_t& t)
	{
		using namespace dak::tantrix;
		RETURN_WIDE_STRING(t);
	}

	template<>
	inline std::wstring ToString<dak::tantrix::tile_t>(const dak::tantrix::tile_t& t)
	{
		using namespace dak::tantrix;
		RETURN_WIDE_STRING(t);
	}

}

#endif /* DAK_TANTRIX_TESTS_HELPERS_H */

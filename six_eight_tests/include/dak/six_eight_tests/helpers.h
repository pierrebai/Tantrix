#pragma once

#include "dak/six_eight/six_eight.h"
#include "dak/six_eight/stream.h"

#include "CppUnitTest.h"

namespace Microsoft::VisualStudio::CppUnitTestFramework
{
	template<>
	inline std::wstring ToString<dak::six_eight::direction_t>(const dak::six_eight::direction_t& t)
	{
		using namespace dak::six_eight;
		RETURN_WIDE_STRING(t);
	}

	template<>
	inline std::wstring ToString<dak::six_eight::position_t>(const dak::six_eight::position_t& t)
	{
		using namespace dak::six_eight;
		RETURN_WIDE_STRING(t);
	}

	template<>
	inline std::wstring ToString<dak::six_eight::solution_t>(const dak::six_eight::solution_t& t)
	{
		using namespace dak::six_eight;
		RETURN_WIDE_STRING(t);
	}

	template<>
	inline std::wstring ToString<dak::six_eight::tile_t>(const dak::six_eight::tile_t& t)
	{
		using namespace dak::six_eight;
		RETURN_WIDE_STRING(t);
	}

}


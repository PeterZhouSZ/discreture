#pragma once

#include "Permutations.hpp"
#include "do_not_optimize.hpp"
#include "Probability.hpp"

inline void BM_PermutationsConstruct(int n, int numtimes)
{
	dscr::permutations X(n);

	for (int i = 0; i < numtimes; ++i)
	{
		auto t = dscr::random::random_int<long>(0,X.size());
		DoNotOptimize(X[t]);
	}
}

inline void BM_PermutationsRandom(int n, int numtimes)
{
	dscr::permutations X(n);

	for (int i = 0; i < numtimes; ++i)
	{
		DoNotOptimize(X.random());
	}
}


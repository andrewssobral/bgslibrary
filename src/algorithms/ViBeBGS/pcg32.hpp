#pragma once

#include <stdint.h>

namespace bgslibrary
{
	namespace algorithms
	{

		class Pcg32 {
		public:

			static inline uint32_t fast()
			{
				static uint64_t mcg_state{0xcafef00dd15ea5e5u};	// Must be odd
				uint64_t x = mcg_state;
				unsigned count = (unsigned)(x >> 61);	// 61 = 64 - 3

				mcg_state = x * multiplier;
				x ^= x >> 22;
				return (uint32_t)(x >> (22 + count));	// 22 = 32 - 3 - 7
			}

		private:
			static uint64_t const multiplier = 6364136223846793005u;
		};

	}
}
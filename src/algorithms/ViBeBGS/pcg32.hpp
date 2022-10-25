#pragma once

#include <stdint.h>

namespace bgslibrary
{
	namespace algorithms
	{
		namespace sky360 {
			class Pcg32 {
			public:

				Pcg32() {
					for (size_t i = 0; i < tableSize; ++i)
						fixedTable[i] = fast2() % tableSize;
				}

				// The actual algorithm
				inline uint32_t fast2()
				{
					uint64_t x = mcg_state;
					unsigned count = (unsigned)(x >> 61);	// 61 = 64 - 3

					mcg_state = x * multiplier;
					x ^= x >> 22;
					return (uint32_t)(x >> (22 + count));	// 22 = 32 - 3 - 7
				}

				inline uint32_t fast()
				{
					const uint32_t result = fixedTable[currentPos];
					if (++currentPos >= tableSize)
						currentPos = 0;
					return result;
				}

			private:
				static uint32_t const tableSize = 65536; 
				static uint64_t const multiplier = 6364136223846793005u;		

				uint64_t mcg_state{0xcafef00dd15ea5e5u};	// Must be odd
				uint32_t fixedTable[tableSize];
				size_t currentPos = 0;
			};
		}
	}
}
//AS header file
//Author: Tye Kukucka
//Date: 4/12/2022

#ifndef AS_H
#define AS_H

#include <cstdint>
#include <vector>

class AS {
	public:
		uint32_t asNumber;
		std::vector<uint32_t> neighbors;
		int propagation_rank = -1;
		//constructor takes asn and assigns propagation rank to -1
		AS(uint32_t asn) {
			asNumber = asn;
			propagation_rank = -1;
		}
		
		
		//destructor
		~AS() {}
};

#endif

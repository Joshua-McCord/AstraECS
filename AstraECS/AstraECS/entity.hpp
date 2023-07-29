#pragma once
#include <set>
#include <string>

namespace astra
{ 
	struct entity
	{
		entity(size_t id, std::set<std::string> type, uint32_t generation) :
			id(id), type(type), generation(generation), is_alive(true) { }


		size_t					 id;
		std::set<std::string>	 type;
		uint32_t				 generation;
		bool					 is_alive;
	};
}

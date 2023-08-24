#pragma once
#include <set>
#include <string>

namespace astra
{
    struct entity
    {
        entity(size_t id, std::set<std::string> type, uint32_t generation) :
            id(id), type(type), generation(generation), is_alive(true) { }

        entity(const astra::entity& other)
        {
            id = other.id;
            type = other.type;
            generation = other.generation;
            is_alive = other.is_alive;
        }

        size_t                  id;
        std::set<std::string>   type;
        uint32_t                generation;
        bool                    is_alive;
    };
}

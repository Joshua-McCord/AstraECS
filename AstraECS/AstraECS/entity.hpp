#pragma once
#include <set>
#include <string>

namespace astra
{
    struct entity
    {
        entity(size_t id = 0, std::set<std::string> type = {}, uint32_t generation = 0) :
            id(id), type(type), generation(generation), is_alive(true) { }

        entity(const astra::entity& other)
        {
            id = other.id;
            type = other.type;
            generation = other.generation;
            is_alive = other.is_alive;
        }

        bool operator==(const entity& rhs) const
        {
        
            return id == rhs.id && 
                   type == rhs.type && 
                   generation == rhs.generation && 
                   is_alive == rhs.is_alive;
        }

        size_t                  id;
        std::set<std::string>   type;
        uint32_t                generation;
        bool                    is_alive;

        struct HashFunction
        {
            size_t operator()(const entity& entity) const
            {
                size_t cnt = 0;
                size_t idHash = std::hash<size_t>()(entity.id) << cnt++;
                size_t genHash = std::hash<uint32_t>()(entity.generation) << cnt++;
                size_t aliveHash = std::hash<bool>()(entity.is_alive) << cnt++;

                size_t typeHash = 0;
                for (std::string s : entity.type)
                    typeHash ^= std::hash<std::string>()(s) << cnt++;

                return idHash ^ genHash ^ aliveHash ^ typeHash;
            }
        };
    };
}

#pragma once
#include <unordered_map>
#include <set>
#include <tuple>
#include <range/v3/all.hpp>
#include <queue>
#include <any>

#include "component_vector.hpp"
#include "entity.hpp"

namespace astra {

    using component_map = std::unordered_map<std::string, astra::IComponentVector*>;

    class IArchetype {
    public:

        IArchetype() { }

        ~IArchetype()
        {
            for (auto [key, ivec] : this->data)
                delete ivec;
        }

        /**
        * Erases the entity from all Component Vectors
        *
        * @param   the Entity to Delete
        * @return  void
        */
        void erase(astra::entity& entity)
        {
            entity.is_alive = false;
            entities[entity.id].is_alive = false;
            free_list.push(entity);
            free_set.insert(entity.id);
        }

        virtual IArchetype* copy(astra::entity& entity) = 0;

        template <typename ComponentType>
        astra::component_vector<ComponentType>* get_component_vector()
        {
            return static_cast<astra::component_vector<ComponentType>*>(
                data.at(typeid(ComponentType).name())
                );
        }

        template<class ComponentType>
        void register_component_type()
        {
            std::string name = typeid(ComponentType).name();

            types.insert(name);

            astra::component_vector<ComponentType>* components = new astra::component_vector<ComponentType>();
            data[name] = components;
        }

        template<class ComponentType>
        void add_component(ComponentType component)
        {
            std::string name = typeid(ComponentType).name();
            astra::component_vector<ComponentType>* components =
                static_cast<astra::component_vector<ComponentType>*>(data.at(name));
            components->data.push_back(component);
        }

        template<class ComponentType>
        void add_component(size_t index, ComponentType component)
        {
            std::string name = typeid(ComponentType).name();
            astra::component_vector<ComponentType>* components =
                static_cast<astra::component_vector<ComponentType>*>(data.at(name));
            components->data[index] = component;
        }

        template<class ComponentType>
        size_t transfer_components(IArchetype* old_archetype, astra::entity& entity, ComponentType component)
        {
            if (free_list.empty())
            {
                for (auto [key, val] : old_archetype->data)
                {
                    auto new_vector = data[key];
                    auto old_vector = old_archetype->data[key];
                    new_vector->transfer_component(old_vector, entity);
                }
                // It should just be added to the back since we just got done 
                add_component(component);
                entities.push_back(entity);
                return entities.size() - 1;
            }

            astra::entity dead_entity = free_list.front();
            free_list.pop();

            free_set.erase(dead_entity.id);

            size_t insertion_point = dead_entity.id;

            for (auto [key, val] : old_archetype->data)
            {
                auto new_vector = data[key];
                auto old_vector = old_archetype->data[key];
                new_vector->transfer_component(old_vector, entity, insertion_point);
            }
            add_component(insertion_point, component);
            entities[insertion_point] = entity;
            return insertion_point;
        }

        template<class ComponentType>
        size_t transfer_components_for_removal(IArchetype* old_archetype, astra::entity& entity)
        {
            if (free_list.empty())
            {
                for (auto [key, val] : data)
                {
                    if(key == typeid(ComponentType).name())
                        continue;

                    auto new_vector = data[key];
                    auto old_vector = old_archetype->data[key];
                    new_vector->transfer_component(old_vector, entity);
                }
                // It should just be added to the back since we just got done 
                entities.push_back(entity);
                return entities.size() - 1;
            }

            astra::entity dead_entity = free_list.front();
            free_list.pop();

            free_set.erase(dead_entity.id);


            size_t insertion_point = dead_entity.id;

            for (auto [key, val] : old_archetype->data)
            {
                if (key == typeid(ComponentType).name())
                    continue;

                auto new_vector = data[key];
                auto old_vector = old_archetype->data[key];
                new_vector->transfer_component(old_vector, entity, insertion_point);
            }
            //entities.push_back(entity);
            entities[insertion_point] = entity;
            return insertion_point;
        }

        void remove_type(std::string type_name)
        {
            data.erase(type_name);
            types.erase(type_name);
        }

        template<class... ComponentTypes>
        auto at(astra::entity& entity)
        {
            auto ent = data
                | ranges::views::filter([=](auto entry) {
                        std::set<std::string> type_set = { typeid(ComponentTypes).name()... };
                        return type_set.contains(entry.first);
                    })
                | ranges::views::values
                | ranges::views::transform([=](IComponentVector* vec) {
                        return ranges::views::zip(
                            get_component_vector<ComponentTypes>()->data...
                        );
                    })
                | ranges::views::join
                | ranges::views::slice(entity.id, entity.id + 1) 
                | ranges::to_vector;

               return ent[0];
        }

        std::set<std::string>       types;
        std::vector<astra::entity>  entities;
        std::queue<entity>          free_list;
        std::set<size_t>            free_set;
        component_map        data;
    private:
    };

    template<class ...Types>
    class archetype : public IArchetype
    {
    public:

        archetype()
        {
            entities = std::vector<entity>();
        }

        archetype(astra::archetype<Types...>& other, astra::entity& entity)
        {
            entities = std::vector<astra::entity>();
            free_list = std::queue<astra::entity>();
            
            data = component_map();
            for (auto [key, val] : other.data)
            {
                types.insert(key);
                data[key] = val->copy(entity);
            }
                
        }

        /**
         * Create and Return an entity.
         *
         * Takes the given components and adds each to the back
         * of the relative component vector
         *
         * @param Components of variadic types
         * @return The Astra Entity that was created
         */
        template<typename ...ComponentTypes>
        astra::entity insert(ComponentTypes...components)
        {
            // No Entities have been deleted, just add everything to the back
            if (free_list.empty())
            {
                (add_component(components), ...);
                astra::entity ent = astra::entity(entities.size(), types, 0);
                entities.push_back(ent);
                return ent;
            }

            // An entity has been deleted, we need to update everything at that position
            astra::entity previous_entity = free_list.front();
            free_list.pop();

            free_set.erase(previous_entity.id);

            astra::entity new_entity = astra::entity(previous_entity.id, types, previous_entity.generation + 1);
            (add_component(new_entity.id, components), ...);
            entities.at(new_entity.id) = new_entity;
            return new_entity;
        }

        /**
         * Number of entities present in Archetype
         *
         * @return size_t size
         */
        inline size_t size()
        {
            return entities.size();
        }

        IArchetype* copy(astra::entity& entity) override
        {
            IArchetype* new_copy = new astra::archetype<Types...>(*this, entity);
            return new_copy;
        }

    private:
    };
}

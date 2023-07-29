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

        /**
        * Erases the entity from all Component Vectors
        *
        * @param   the Entity to Delete
        * @return  void
        */
        virtual void erase(astra::entity& entity) = 0;

        virtual IArchetype* copy(astra::entity entity) = 0;

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

		std::set<std::string>       types;
        std::vector<astra::entity>  entities;
		std::queue<entity>          free_list;
		astra::component_map        data;
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

        archetype(astra::archetype<Types...>& other, astra::entity entity)
        {
            entities = std::vector<astra::entity>();
            free_list = std::queue<astra::entity>();

			data = component_map();			
            for (auto [key, val] : other.data)
				data[key] = val->copy(entity);

			
        }

        /**
         * Create and Return an entity. 
         *
         * Takes the given components 
         *
         * @param values Container whose values are summed.
         * @return sum of `values`, or 0.0 if `values` is empty.
         */
        template<typename ...ComponentTypes>
        entity insert(ComponentTypes...components)
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

            astra::entity new_entity = astra::entity(previous_entity.id, types, previous_entity.generation + 1);
            (add_component(new_entity.id, components), ...);
            entities.at(new_entity.id) = new_entity;
            return new_entity;
        }

        void erase(astra::entity& entity) override
        {
            entity.is_alive = false;
            free_list.push(entity);
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

		IArchetype* copy(astra::entity entity) override
		{
            IArchetype* new_copy = new astra::archetype<Types...>(*this, entity);
            return new_copy;
		}

    private:

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
    };
}

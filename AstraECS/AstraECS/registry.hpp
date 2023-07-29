#pragma once
#include <optional>
#include <algorithm>
#include <spdlog/spdlog.h>
#include <range/v3/all.hpp>
#include <ranges>
#include "entity.hpp"
#include "archetype.hpp"
#include "component_vector.hpp"

namespace astra 
{
	class registry
	{
	public:

		registry() 
		{
			_size = 0;
		}

		~registry()
		{
			// TODO Call delete on all the Archetypes?
		}

		template<class ...ComponentTypes>
		inline auto view()
		{
			return archetypes
				| ranges::views::remove_if([](IArchetype* archetype) {
						for (auto s : std::vector < std::string>{ typeid(ComponentTypes).name()... })
							if (!archetype->types.contains(s))
								return true;
						return false;
					})
				| ranges::views::transform([](astra::IArchetype* a) {
						return ranges::views::zip(
							(a->entities),
							a->get_component_vector<ComponentTypes>()->data...
						) | ranges::views::remove_if([](auto e) { return !ranges::get<0>(e).is_alive; });
					})
				| ranges::views::join;
		}

		template<class ...ComponentTypes>
		auto insert(ComponentTypes...components)
		{
			std::set<std::string> arch_key = { typeid(ComponentTypes).name()... };
			_size++;

			for (auto* arch : archetypes)
			{
				if (arch->types == arch_key)
				{
					astra::archetype<ComponentTypes...>* casted_archetype =
						static_cast<astra::archetype<ComponentTypes...>*>(arch);
					return casted_archetype->insert(components...);
				}
					
			}
				
			
			astra::archetype<ComponentTypes...>* arch = 
				static_cast<astra::archetype<ComponentTypes...>*>(register_type<ComponentTypes...>());
			return arch->insert(components...);
		}

		template<class ComponentType>
		void insert_component(astra::entity entity)
		{
			std::set<std::string> old_type = entity.type;
			std::set<std::string> new_type = std::set<std::string>{ typeid(ComponentType).name() };
			new_type.insert(old_type.begin(), old_type.end());

			astra::IArchetype* old_archetype;
			for (auto* arch : archetypes)
			{
				if (arch->types == old_type)
				{
					astra::IArchetype* new_archetype = arch->copy(entity);
					new_archetype->register_component_type<ComponentType>();
					archetypes.push_back(new_archetype);


					
				}
			}
		}

		/**
		 * Kills the Entity (invalidates) and adds the ID to the Archetypes Free list
		 * 
		 * @param The Entity to erase
		 */
		inline void erase(astra::entity entity)
		{
			for (auto& arch : archetypes)
				if (arch->types == entity.type)
					arch->erase(entity);
		}

		/**
		 * Returns total number of valid entities in the Registry. 
		 * 
		 * @param void
		 * @returns number of entities
		 */
		inline size_t size()
		{
			return _size;
		}
	
	private:
		template<class ...ComponentTypes>
		IArchetype* register_type()
		{
			// Build archetype
			astra::archetype<ComponentTypes...>* arch = new astra::archetype<ComponentTypes...>();
			(arch->register_component_type<ComponentTypes>(), ...);

			// Add to archetypes
			archetypes.push_back(arch);
			return arch;
		}

		size_t _size;
		std::vector<astra::IArchetype*> archetypes;
	};
}


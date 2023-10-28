#pragma once
#include <optional>
#include <algorithm>
#include <spdlog/spdlog.h>
#include <range/v3/view.hpp>
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
            for(auto archetype : archetypes)
                delete archetype;
        }

        template<class ...ComponentTypes>
        inline auto view()
        {
            return archetypes
                | ranges::views::remove_if([=](IArchetype* a) {
                    std::set<std::string> types_set = std::set<std::string>{ typeid(ComponentTypes).name()... };
                        return !std::includes(a->types.begin(), a->types.end(), types_set.begin(), types_set.end());
                    })
                | ranges::views::transform([=](astra::IArchetype* a) {
                        return ranges::views::zip((a->entities),
                        a->get_component_vector<ComponentTypes>()->data...
                        )
                | ranges::views::filter([=](auto&& e) {
                        return ranges::get<0>(e).is_alive;
                        });
                    })
                | ranges::views::join;
        }

        template<class ...ComponentTypes>
        astra::entity insert(ComponentTypes...components)
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

        // TODO Replace this with emplace(...);
        template<class ComponentType>
        void add_component(astra::entity& entity, ComponentType component)
        {
            std::set<std::string> old_entity_type = entity.type;

            std::set<std::string> new_entity_type = entity.type;
            new_entity_type.insert(typeid(ComponentType).name());

            IArchetype* old_archetype = nullptr;
            IArchetype* new_archetype = nullptr;
            for (auto* arch : archetypes)
            {
                if (arch->types == old_entity_type)
                    old_archetype = arch;

                if (arch->types == new_entity_type)
                    new_archetype = arch;
            }
            assert(old_archetype != nullptr);

            // Case 1. The new type is not present in the Registry
            // and we have to create a new Archetype
            if (new_archetype == nullptr)
            {

                astra::IArchetype* new_archetype = old_archetype->copy(entity);
                new_archetype->register_component_type<ComponentType>();
                new_archetype->add_component<ComponentType>(component);
                old_archetype->erase(entity);

                old_archetype->entities[entity.id] = astra::entity(entity);
                entity = astra::entity(0, new_archetype->types, 0);
                new_archetype->entities.push_back(entity);
                archetypes.push_back(new_archetype);
                return;
            }

            // Case 2. The new Archetype already exists, we just need to transfer
            // the data from the old to the new
            size_t insertion_point = new_archetype->transfer_components(old_archetype, entity, component);
            old_archetype->erase(entity);
            old_archetype->entities[entity.id] = astra::entity(entity);

            entity = astra::entity(
                insertion_point,
                new_archetype->types,
                new_archetype->entities[insertion_point].generation + 1
            );

            new_archetype->entities[insertion_point] = entity;
        }

        template<class ComponentType>
        void remove_component(astra::entity& entity)
        {
            std::set<std::string> old_entity_type = entity.type;

            std::set<std::string> new_entity_type = entity.type;
            new_entity_type.erase(typeid(ComponentType).name());

            IArchetype* old_archetype = nullptr;
            IArchetype* new_archetype = nullptr;
            for (auto* arch : archetypes)
            {
                if (arch->types == old_entity_type)
                    old_archetype = arch;

                if (arch->types == new_entity_type)
                    new_archetype = arch;
            }
            assert(old_archetype != nullptr);



            if (new_archetype == nullptr)
            {
                // This all I need...?
                astra::IArchetype* new_archetype = old_archetype->copy(entity);
                new_archetype->remove_type(typeid(ComponentType).name());
                old_archetype->erase(entity);

                old_archetype->entities[entity.id] = astra::entity(entity);
                entity = astra::entity(0, new_archetype->types, 0);
                new_archetype->entities.push_back(entity);
                archetypes.push_back(new_archetype);

                return;
            }

            // Case 2. The new Archetype already exists, we just need to transfer
            // the data from the old to the new
            size_t insertion_point = new_archetype->transfer_components_for_removal<ComponentType>(old_archetype, entity);
            old_archetype->erase(entity);
            old_archetype->entities[entity.id] = astra::entity(entity);

            entity = astra::entity(
                insertion_point,
                new_archetype->types,
                new_archetype->entities[insertion_point].generation + 1
            );


            new_archetype->entities[insertion_point] = entity;
            return;
        }

        /**
         * Kills the Entity (invalidates) and adds the ID to the Archetypes Free list
         *
         * @param The Entity to erase
         */
        inline void erase(astra::entity& entity)
        {
            for (auto& arch : archetypes)
                if (arch->types == entity.type)\
                    arch->erase(entity);

            _size--;
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

        template<class... Types>
        auto at(astra::entity& entity)
        {
            astra::IArchetype* entity_arch = nullptr;
            for (auto* arch : archetypes)
            {
                if (arch->types == entity.type)
                {
                    entity_arch = arch;
                    break;
                }
            }
            assert(entity_arch != nullptr);
            return entity_arch->at<Types...>(entity);
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

// Sys includes
#include <iostream>

// ECSs
#include "entt/entt.hpp"
#include "../AstraECS/registry.hpp"

// Components
#include "../Components/physics_component.h"
#include "../Components/sprite_component.h"
#include "../Components/transform_component.h"

namespace Tests
{
	// Astra
	void test_entity_creation();
	void test_entity_deletion();
	void benchmark_entt(size_t num_entities);
	void benchmark_astra(size_t num_entities);
	
	void add_component_replaces_deleted_entity();
	void add_component_creates_new_archetype();

	void remove_component_creates_new_archetype();
	void remove_component_replaces_deleted_entity();

	void test_views_with_enumerate(size_t num_entities);

	void test_at_method();
};
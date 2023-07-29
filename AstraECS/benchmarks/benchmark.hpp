// Sys includes
#include <iostream>

// ECSs
#include "entt/entt.hpp"
#include "../AstraECS/registry.hpp"

// Components
#include "../Components/physics_component.h"
#include "../Components/sprite_component.h"
#include "../Components/transform_component.h"

class Tester
{
public:


	// EnTT
	void test_entt(size_t num_entities);

	// Astra
	void test_astra(size_t num_entities);
	void test_astra_entity_creation(size_t num_entities);
	void test_astra_entity_deletion(size_t num_entities);

};
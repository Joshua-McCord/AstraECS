#include <iostream>
#include <spdlog/spdlog.h>
#include <spdlog/stopwatch.h>
#include <spdlog/fmt/chrono.h>
#include <chrono>
#include "AstraECS/registry.hpp"
#include "Components/physics_component.h"
#include "Components/sprite_component.h"
#include "Components/transform_component.h"

#include "benchmarks/benchmark.hpp"


int main(void)
{
	//size_t num_entities = 250000 / 2;
	size_t num_entities = 100000;
	
	Tests::add_component_creates_new_archetype();

	Tests::remove_component_creates_new_archetype();
	Tests::remove_component_replaces_deleted_entity();

	Tests::benchmark_astra(num_entities);
	Tests::benchmark_entt(num_entities);
	Tests::test_at_method();

}
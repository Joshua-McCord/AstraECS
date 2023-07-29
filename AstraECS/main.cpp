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
	//size_t num_entities = 500000;
	size_t num_entities = 5;
	Tester tester;
	tester.test_astra(num_entities);
	tester.test_entt(num_entities);
	//tester.test();
	//tester.test_astra_entity_creation(num_entities);
	tester.test_astra_entity_deletion(num_entities);
}
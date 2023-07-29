#include "benchmark.hpp"

using namespace std::chrono;
using std::chrono::duration_cast;
using std::chrono::nanoseconds;
using std::chrono::milliseconds;

void Tester::test_entt(size_t num_entities)
{
	entt::basic_registry registry;

	for (int i = 0; i < num_entities; i++)
	{
		auto entity = registry.create();

		registry.emplace<TransformComponent>(entity, i, i, "Xform " + std::to_string(i));
		registry.emplace<PhysicsComponent>(entity, 0.1f, 0.1f, "Physx " + std::to_string(i));
		registry.emplace<SpriteComponent>(entity);
	}

	for (int i = 0; i < num_entities; i++)
	{
		auto entity = registry.create();

		registry.emplace<TransformComponent>(entity, i, i, "Xform " + std::to_string(i));
		registry.emplace<PhysicsComponent>(entity, 0.1f, 0.1f, "Physx " + std::to_string(i));
	}

	auto start = high_resolution_clock::now();

	auto entities = registry.view<TransformComponent, PhysicsComponent>();

	auto stop = high_resolution_clock::now();
	auto ns = duration_cast<nanoseconds>(stop - start);
	auto ms = duration_cast<milliseconds>(stop - start);

	spdlog::info(
		"EnTT: Queried {} Position and Velocity entities in {} n/s ({} m/s)",
		registry.size(),
		ns.count(),
		ms.count()
	);

	start = high_resolution_clock::now();

	for (auto [entity, xform, physx] : entities.each())
	{
		xform.x += physx.vel;
		xform.y += physx.acc;
	}

	stop = high_resolution_clock::now();

	ns = duration_cast<nanoseconds>(stop - start);
	ms = duration_cast<milliseconds>(stop - start);

	spdlog::info(
		"EnTT: Processed {} Position and Velocity entities in {} n/s ({} m/s)",
		registry.size(),
		ns.count(),
		ms.count()
	);

}

void Tester::test_astra(size_t num_entities)
{

#if 1
	astra::registry registry;

	for (int i = 0; i < num_entities; i++)
	{
		TransformComponent	xform = TransformComponent(i, i, "Xform " + std::to_string(i));
		PhysicsComponent	physx = PhysicsComponent(0.1f, 0.1f, "Physx " + std::to_string(i));
		SpriteComponent		sprite = SpriteComponent();

		registry.insert(xform, physx, sprite);
	}
	for (int i = 0; i < num_entities; i++)
	{
		TransformComponent	xform = TransformComponent(i, i, "Xform " + std::to_string(i));
		PhysicsComponent	physx = PhysicsComponent(0.1f, 0.1f, "Physx " + std::to_string(i));

		registry.insert(xform, physx);
	}

	auto start = high_resolution_clock::now();

	auto entities = registry.view<TransformComponent, PhysicsComponent>();

	auto stop = high_resolution_clock::now();
	auto ns = duration_cast<nanoseconds>(stop - start);
	auto ms = duration_cast<milliseconds>(stop - start);

	spdlog::info(
		"Astra: Queried {} Position and Velocity entities in {} n/s ({} m/s)", 
		registry.size(), 
		ns.count(),
		ms.count()
	);

	start = high_resolution_clock::now();
	
	for (auto [entity, xform, physx] : entities)
	{
		xform.x += physx.vel;
		xform.y += physx.acc;
	}
	stop = high_resolution_clock::now();

	ns = duration_cast<nanoseconds>(stop - start);
	ms = duration_cast<milliseconds>(stop - start);

	spdlog::info(
		"Astra: Processed {} Position and Velocity entities in {} n/s ({} m/s)",
		registry.size(),
		ns.count(),
		ms.count()
	);
#endif
}

void Tester::test_astra_entity_creation(size_t num_entities)
{

#if 0
	astra::registry registry = astra::registry();

	for (int i = 0; i < num_entities; i++)
	{
		TransformComponent	xform = TransformComponent(i, i, "Xform " + std::to_string(i));
		PhysicsComponent	physx = PhysicsComponent(0.1f, 0.1f, "Physx " + std::to_string(i));
		SpriteComponent		sprite = SpriteComponent();

		astra::entity entity = registry.insert(xform, physx, sprite);
	}

	for (int i = 0; i < num_entities; i++)
	{
		TransformComponent	xform = TransformComponent(i, i, "Xform " + std::to_string(i));
		PhysicsComponent	physx = PhysicsComponent(0.1f, 0.1f, "Physx " + std::to_string(i));

		auto entity = registry.insert(xform, physx);
	}

	auto entities = registry.view<TransformComponent, PhysicsComponent>();

	for (auto [entity, xform, physx] : entities)
	{
		xform.x += physx.vel;
		
	}
#endif

}

void Tester::test_astra_entity_deletion(size_t num_entities)
{
	astra::registry registry = astra::registry();

	auto ent = registry.insert(
		TransformComponent(0.0f, 0.0f, "Xform"), 
		PhysicsComponent(1.0f, 0.5f, "Physx")
	);

	registry.insert_component<SpriteComponent>(ent);
}
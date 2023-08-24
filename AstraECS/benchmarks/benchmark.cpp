#include "benchmark.hpp"

using namespace std::chrono;
using std::chrono::duration_cast;
using std::chrono::nanoseconds;
using std::chrono::milliseconds;

/**
 *  Inserting Entities
 */
void Tests::test_entity_creation()
{

}

/**
 *  Erasing Entities
 */
void Tests::test_entity_deletion()
{

}

/**
 *  Benchmarks
 */
void Tests::benchmark_entt(size_t num_entities)
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

void Tests::benchmark_astra(size_t num_entities)
{
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
}

void Tests::test_views_with_enumerate(size_t num_entities)
{
    astra::registry registry = astra::registry();

    auto ent1 = registry.insert(
        TransformComponent(0.0f, 0.0f, "Xform"),
        PhysicsComponent(1.0f, 0.5f, "Physx")
    );

    auto ent2 = registry.insert(
        TransformComponent(0.0f, 0.0f, "Xform"),
        PhysicsComponent(1.0f, 0.5f, "Physx")
    );

    auto ent3 = registry.insert(
        TransformComponent(0.0f, 0.0f, "Xform"),
        PhysicsComponent(1.0f, 0.5f, "Physx")
    );

    auto ent4 = registry.insert(
        TransformComponent(0.0f, 0.0f, "Xform"),
        PhysicsComponent(1.0f, 0.5f, "Physx")
    );

    auto ent5 = registry.insert(
        TransformComponent(0.0f, 0.0f, "Xform"),
        PhysicsComponent(1.0f, 0.5f, "Physx")
    );

    registry.erase(ent5);

    auto entities = registry.view<TransformComponent, PhysicsComponent>();

    for (auto [ent, xform, physx] : entities)
    {
        xform.x += physx.vel;
    }
}

void Tests::test_at_method()
{
    astra::registry registry = astra::registry();

    auto ent1 = registry.insert(TransformComponent(1.0), SpriteComponent(), PhysicsComponent());
    auto ent2 = registry.insert(TransformComponent(2.0), SpriteComponent(), PhysicsComponent());

    auto [xform, physx] = registry.at<TransformComponent, PhysicsComponent>(ent1);
}

/**
 *  Adding Components
 */
void Tests::add_component_replaces_deleted_entity()
{
    astra::registry registry = astra::registry();

    auto ent1 = registry.insert(
        TransformComponent(0.0f, 0.0f, "Xform"),
        PhysicsComponent(1.0f, 0.5f, "Physx"),
        SpriteComponent()
    );

    auto ent2 = registry.insert(
        TransformComponent(0.0f, 0.0f, "Xform"),
        PhysicsComponent(1.0f, 0.5f, "Physx"),
        SpriteComponent()
    );

    registry.erase(ent2);

    auto ent3 = registry.insert(
        TransformComponent(0.0f, 0.0f, "Xform"),
        PhysicsComponent(1.0f, 0.5f, "Physx")
    );



    registry.add_component(ent3, SpriteComponent());

    assert(ent2.is_alive == false);
    assert(ent3.type.size() == 3);
    assert(ent3.is_alive == true);
    assert(ent3.generation == 1);
    assert(ent3.id == ent2.id);
}

void Tests::add_component_creates_new_archetype()
{
    astra::registry registry = astra::registry();

    auto ent1 = registry.insert(
        TransformComponent(0.0f, 0.0f, "Xform"),
        PhysicsComponent(1.0f, 0.5f, "Physx")
    );

    registry.add_component(ent1, SpriteComponent());

    auto ent2 = registry.insert(
        TransformComponent(0.0f, 0.0f, "Xform"),
        PhysicsComponent(1.0f, 0.5f, "Physx")
    );

    assert(ent1.id == 0);
    assert(ent1.generation == 0);
    assert(ent1.type.size() == 3);
    assert(ent1.is_alive = true);

}

/**
 *  Removing Components
 */
void Tests::remove_component_creates_new_archetype()
{
    astra::registry registry = astra::registry();

    auto ent1 = registry.insert(
        TransformComponent(0.0f, 0.0f, "Xform"),
        PhysicsComponent(1.0f, 0.5f, "Physx"),
        SpriteComponent()
    );

    registry.remove_component<SpriteComponent>(ent1);

    assert(ent1.is_alive);
    assert(ent1.generation == 0);
    assert(ent1.type.size() == 2);
    assert(ent1.id == 0);
}

void Tests::remove_component_replaces_deleted_entity()
{
    astra::registry registry = astra::registry();

    auto ent1 = registry.insert(
        TransformComponent(0.0f, 0.0f, "Xform"),
        PhysicsComponent(1.0f, 0.5f, "Physx")
    );

    auto ent2 = registry.insert(
        TransformComponent(0.0f, 0.0f, "Xform"),
        PhysicsComponent(1.0f, 0.5f, "Physx")
    );

    registry.erase(ent1);

    auto ent3 = registry.insert(
        TransformComponent(0.0f, 0.0f, "Xform"),
        PhysicsComponent(1.0f, 0.5f, "Physx"),
        SpriteComponent()
    );

    registry.remove_component<SpriteComponent>(ent3);

    assert(ent3.is_alive);
    assert(ent3.generation == 1);
    assert(ent3.id == 0);
    assert(ent3.type.size() == 2);
}




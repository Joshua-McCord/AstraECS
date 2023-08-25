<p align="center">
<img src="https://github.com/Joshua-McCord/AstraECS/assets/43547244/b4ca14d4-b943-417b-80b8-e6c97fd3f285" alt="Sublime's custom image"/>
<h1 align="center">AstraECS</h1>


> Sic itur ad astra (Thus one journeys to the stars)
>
> -- Brad Pitt looking for his dad...or Virgil

---
`AstraECS` is an easy-to-use library for Entity storage written in C++. `Astra` provides a 
straight-forward API, and leverages modern data storage philosophies to allow the quick creation, 
iteration, updating, and deleting of Entities. 

# Table of Contents

* [Introduction](#introduction)
  * [Motivation](#motivation)
  * [Code Example](#code-example)
* [Integration](#integration)
  * [Requirements](#requirements)
* [Documentation](#documentation)
* [License](#license)

# Introduction

## Motivation
What started as a weekend project to replace my game engine's current 
Entity-Component-System ( _ECS_) inevitably turned into a multi-week-long project. 
My current engine's ECS is reminiscent of a traditional `SoA` and each `ComponentType` was stored 
as a `std::vector<std::optional<T>>`. Each Entity had an associated bit flag, and if the bit flag was
set, then that component existed (e.g. if TRANSFORM_FLAG was set, the Entity had a TransformComponent). 
This was fine and let me bootstrap a small engine/game, but quickly grew slow and not enjoyable
to work with. As I kept using my ECS, I realized I wanted something like an STL container but for Entities. 
Thus I settled on the final API for `Astra`. Most operations that you are familiar with in the STL exist in `Astra`.

## Example Usage

```cpp
#include <astra/registry.hpp>

struct PositionComponent
{
    PositionComponent(glm::vec2 position) : position(position) { }
    glm::vec2 position;
};

struct PhysicsComponent
{
    PhysicsComponent(glm::vec2 vel, glm::vec2 acc) : vel(vel), acc(acc) { }
    glm::vec2 vel;
    glm::vec2 acc;
};

struct move_system
{
    void tick(astra::registry& registry, float dt)
    {
        auto entities = registry.view<PositionComponent, PhysicsComponent>();
        for(auto [entity, xform, physx] : entities)
        {
            xform.position += physx.vel * dt;
            physx.vel = physx.acc;
            physx.acc = glm::vec2(random::float(0, 1), random::float(0, 1));
        }
    }
};


int main() {
    astra::registry registry;
    const float dt = 0.08;

    for(size_t i = 0; i < 1000; ++i)
    {
        auto xform = PositionComponent(glm::vec2(i));
        auto physx = PhysicsComponent(glm::vec2(0.1f), glm::vec2(0.1f));
        registry(xform, physx);
    }

    while(true)
        move_system.tick(registry, dt);
}
```

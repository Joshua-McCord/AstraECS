#pragma once
#include <vector>

#include "entity.hpp"

namespace astra {
    struct IComponentVector
    {
        virtual void erase(size_t index) = 0;
        virtual IComponentVector* copy(astra::entity& entity) = 0;
        virtual void transfer_component(IComponentVector* old, astra::entity& entity) = 0;
        virtual void transfer_component(IComponentVector* old, astra::entity& entity, size_t insertion_point) = 0;
    };

    template<class T>
    class component_vector : public IComponentVector
    {
    public:

        component_vector()
        {
            data = std::vector<T>();
            available_slots = std::vector<size_t>();
        }

        component_vector(astra::IComponentVector& other, astra::entity entity)
        {
            data = std::vector<T>();
            available_slots = std::vector<size_t>();

        }

        inline void erase(size_t index) override
        {
            data.erase(data.begin() + index);
        }

        IComponentVector* copy(astra::entity& entity) override
        {
            IComponentVector* new_component_vector =
                new component_vector<T>(*this, entity);

            static_cast<astra::component_vector<T>*>(
                new_component_vector)->data.push_back(data[entity.id]);

            return new_component_vector;
        }

        void transfer_component(IComponentVector* old, astra::entity& entity) override
        {
            astra::component_vector<T>* casted_component_vector = static_cast<astra::component_vector<T>*>(old);
            T element = casted_component_vector->data[entity.id];
            data.push_back(element);
        }

        void transfer_component(IComponentVector* old, astra::entity& entity, size_t insertion_point) override
        {
            astra::component_vector<T>* casted_component_vector = static_cast<astra::component_vector<T>*>(old);
            T element = casted_component_vector->data[entity.id];
            data[insertion_point] = element;
        }

        std::vector<T> data;
    private:

        std::vector<size_t> available_slots;
    };
}
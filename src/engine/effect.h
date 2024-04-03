#pragma once

namespace engine {
namespace effects {

class Effect {
public:
    virtual void onApply() = 0;
    virtual void onRemove() = 0;
    virtual void onTurn() = 0;
};

}   // namespace effects
}   // namespace engine
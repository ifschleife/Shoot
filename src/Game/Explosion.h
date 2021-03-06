
#pragma once

#include "Entity/EntityBase.h"
#include "Rendering/RenderPtrFwd.h"

namespace game
{
    struct ExplosionConfiguration
    {
        math::Vector position;
        float scale = 1.0f;
        float rotation = 0.0f;
        const char* sprite_file = nullptr;
    };

    class Explosion : public mono::EntityBase
    {
    public:
        
        Explosion(const ExplosionConfiguration& config, mono::EventHandler& event_handler);
        void Update(unsigned int delta) override;
        void Draw(mono::IRenderer& renderer) const override;

    private:

        mono::ISpritePtr m_sprite;
    };
}

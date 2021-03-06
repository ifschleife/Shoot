
#pragma once

#include "Entity/EntityBase.h"
#include "Rendering/RenderPtrFwd.h"
#include <string>

namespace editor
{
    class SpriteEntity : public mono::EntityBase
    {
    public:
        SpriteEntity(const char* name, const char* file);
        
        void SetSelected(bool selected);
        const std::string& Name() const;
        
    private:

        virtual void Draw(mono::IRenderer& renderer) const;
        virtual void Update(unsigned int delta);

        const std::string m_name;
        bool m_selected;
        mono::ISpritePtr m_sprite;
    };
}

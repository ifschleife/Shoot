
#pragma once

#include "ITool.h"
#include "Math/Vector.h"
#include <memory>
#include <vector>

namespace editor
{
    class Editor;

    class PolygonTool : public ITool
    {
    public:

        PolygonTool(Editor* editor);

        virtual void Begin();
        virtual void End();
        virtual bool IsActive() const;
        virtual void HandleContextMenu(int menu_index);
        virtual void HandleMouseDown(const math::Vector& world_pos, mono::IEntityPtr entity);
        virtual void HandleMouseUp(const math::Vector& world_pos);
        virtual void HandleMousePosition(const math::Vector& world_pos);

        Editor* m_editor;
        std::shared_ptr<class PolygonVisualizer2> m_visualizer;

        math::Vector m_mousePosition;
        std::vector<math::Vector> m_points;
    };
}

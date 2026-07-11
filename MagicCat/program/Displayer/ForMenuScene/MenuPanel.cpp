module;

#include <RenderUtils.h>

module Displayer;

namespace mc {
    MenuPanel::MenuPanel()
    {
        SetPosition(MENU_BOX_X1, MENU_TEXT_Y);
    }

    bool MenuPanel::IsPanelFocus() const
    {
        return false;
    }
} // namespace mc

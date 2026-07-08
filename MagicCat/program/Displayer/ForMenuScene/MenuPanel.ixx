module;

#include <RenderUtils.h>

export module Displayer:Menu;

import DisplayerBase;
import InputService;
import RenderService;
import AudioService;
import BattleService;
import ConfigService;
import SceneService;
import OSService;
import ButtonGroup;

namespace mc {

export class MenuPanel : public Displayer {
public:
    MenuPanel(int boxX1, int textY)
    {
        SetPosition(boxX1, textY);
    }

    virtual ~MenuPanel() = default;

    virtual bool IsActive() const = 0;
    virtual void Activate() = 0;
    virtual void Deactivate() = 0;

protected:
};


export std::unique_ptr<MenuPanel> CreateRulesPanel(int boxX1, int textY,
                                                    IInputService& input,
                                                    IRenderService& render);

} 

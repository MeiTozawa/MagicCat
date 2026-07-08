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
    /// boxX1 / textY 在构造时注入，存入 Displayer::x / y
    MenuPanel(int boxX1, int textY)
    {
        SetPosition(boxX1, textY);
    }

    virtual ~MenuPanel() = default;

    /// 返回面板是否处于"已激活"状态
    virtual bool IsActive() const = 0;

    /// 从顶部按钮栏进入本面板时由 MenuScene 调用（重置内部焦点）
    virtual void Activate() = 0;

    /// 用户退出面板时由 MenuScene 或面板自身调用（标记为非激活）
    virtual void Deactivate() = 0;

protected:
    // x = boxX1, y = textY（由 Displayer 基类提供）
};

// --- 子面板前向声明（完整定义在各自 .cpp 中）---
export class RulesPanel;
export class VolumePanel;
export class SavePanel;
export class LoadPanel;
export class ExitPanel;

} // namespace mc

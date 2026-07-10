module;

#include <optional>
#include <span>
#include <RenderUtils.h>

export module ButtonGroup;
import InputService;
import RenderService;
import OSService;
import EventBus;

namespace mc {
    /// @brief ナビゲーション方向 — 縦並び（Up/Down）か横並び（Left/Right）かを指定する
    export enum class ButtonGroupLayout
    {
        Vertical, ///< 上下キー（Up/Down）でフォーカス移動
        Horizontal, ///< 左右キー（Left/Right）でフォーカス移動
    };

    /// @brief メニュー等のボタンフォーカスが変更された際に発行されるイベント
    export struct MenuFocusChangedEvent : IEvent {};

    /// @brief キーボード・マウス・ゲームパッド入力を統合し、ボタン群のフォーカス管理を行うクラス。
    ///
    /// 使い方:
    ///   1. コンストラクタに Rect 配列・入力サービス・レンダーサービス・レイアウトを渡す。
    ///   2. 毎フレーム Update() を呼ぶ（マウスホバー検出・カーソル変更が行われる）。
    ///   3. GetFocusedIndex() で現在のフォーカスを取得して描画に利用する。
    ///   4. ConsumeClick() で「このフレームにクリック/確定された行インデックス」を取得する。
    export class ButtonGroup
    {
    public:
        /// @param rects         ボタンの矩形リスト（インデックス順）
        /// @param input         入力サービス
        /// @param os        レンダーサービス（カーソル変更に使用）
        /// @param layout        ナビゲーション方向（デフォルト: Vertical）
        /// @param initialFocus  初期フォーカスインデックス（デフォルト: 0）
        ButtonGroup(std::span<const Rect<int>> rects,
                    IInputService& input,
                    IOSService& os,
                    ButtonGroupLayout layout = ButtonGroupLayout::Vertical,
                    int initialFocus = 0)
            : rects(rects.begin(), rects.end())
              , inputService(input)
              , osService(os)
              , layout(layout)
              , focusedIndex(initialFocus) {}

        /// @brief 毎フレーム呼び出す。
        ///        マウスホバーによるフォーカス更新、カーソル形状の切り替えを行う。
        ///        キーボード/ゲームパッドのナビゲーション入力も処理する。
        void Update()
        {
            UpdateFocusFromMouse();
            UpdateCursor();
            HandleNavigationInput();
        }

        /// @brief 現在フォーカスされているボタンのインデックスを返す
        int GetFocusedIndex() const { return focusedIndex; }

        /// @brief フォーカスを明示的に設定する（外部からリセット時など）
        void SetFocusedIndex(int index) { focusedIndex = Clamp(index); }

        /// @brief フォーカスのオフセットを返す
        /// @return -1: prev  0: not changed  1: next
        int GetFocusChangeDelta() const { return changeDelta; }

        /// @brief このフレームに確定（クリックまたは Confirm キー）されたボタンのインデックスを返す。
        ///        何も確定されていない場合は std::nullopt。
        ///        内部フラグはこの呼び出しでリセットされる（consume）。
        std::optional<int> ConsumeConfirm()
        {
            auto click = inputService.OnMouseClick(InputAction::MouseClick);
            if (click.x != -1 && click.y != -1)
            {
                for (int i = 0; i < static_cast<int>(rects.size()); ++i)
                {
                    if (click.In(rects[i]))
                    {
                        focusedIndex = i;
                        return i;
                    }
                }
            }

            // キーボード / ゲームパッド確定キー
            if (inputService.IsPressed(InputAction::Confirm))
                return focusedIndex;

            return std::nullopt;
        }

        /// @brief 現在マウスが指定インデックスのボタン上にあるかを返す
        bool IsHovered(int index) const
        {
            if (index < 0 || index >= static_cast<int>(rects.size())) return false;
            return inputService.IsMouseOver(rects[index]);
        }

        /// @brief ボタン数を返す
        int Count() const { return static_cast<int>(rects.size()); }

    private:
        void UpdateFocusFromMouse()
        {
            for (int i = 0; i < static_cast<int>(rects.size()); ++i)
            {
                if (inputService.IsMouseOver(rects[i]))
                {
                    if (focusedIndex != i)
                    {
                        focusedIndex = i;
                        EventBus::Publish(MenuFocusChangedEvent{});
                    }
                    return;
                }
            }
        }

        void UpdateCursor() const
        {
            for (const auto& r : rects)
            {
                if (inputService.IsMouseOver(r))
                {
                    osService.SetCursorPointer();
                    return;
                }
            }
            osService.SetCursorArrow();
        }

        void HandleNavigationInput()
        {
            const InputAction prev = (layout == ButtonGroupLayout::Vertical)
                                         ? InputAction::Up
                                         : InputAction::Left;
            const InputAction next = (layout == ButtonGroupLayout::Vertical)
                                         ? InputAction::Down
                                         : InputAction::Right;
            changeDelta = 0;
            if (inputService.IsPressed(prev))
                changeDelta--;
            else if (inputService.IsPressed(next))
                changeDelta++;

            if (changeDelta != 0)
            {
                const int newIndex = Clamp(focusedIndex + changeDelta);
                if (newIndex != focusedIndex)
                {
                    focusedIndex = newIndex;
                    EventBus::Publish(MenuFocusChangedEvent{});
                }
            }
        }

        int Clamp(int index) const
        {
            if (index < 0) return 0;
            const int last = static_cast<int>(rects.size()) - 1;
            return index > last ? last : index;
        }

        std::vector<Rect<int>> rects;
        IInputService& inputService;
        IOSService& osService;
        ButtonGroupLayout layout;
        int focusedIndex, changeDelta = 0;
    };
} // namespace mc

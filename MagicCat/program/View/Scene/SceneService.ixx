module;

#include <vector>
#include <memory>
#include <functional>

export module SceneService;

import CardService;

import EventBus;
import InputService;
import AssetService;
import CharacterService;
import RenderService;

namespace mc
{
    export enum class ESceneState
    {
        Info, Combat, Rules
    };



    export struct EnterCutSceneEvent : IEvent {};

    export struct ExitCutSceneEvent : IEvent {};

    export struct ActionSelectionEvent : IEvent
    {
        int selectedIndex;
        bool isMagicMenuOpen;
        explicit ActionSelectionEvent(const int selected_index, bool isMagicMenuOpen = false)
            : selectedIndex(selected_index), isMagicMenuOpen(isMagicMenuOpen)
        {
        }
    };

    /// @brief ビュー（UIや描画等）の基本インターフェース
    export class IView
    {
    public:
        virtual ~IView() = default;

        /// @brief フレームごとの更新処理を行う
        /// @param deltaTime 前回のフレームからの経過時間
        virtual void Update(float deltaTime) = 0;
    };

    /// @brief 各シーン（Info, Combat, Rules等）の基本インターフェース
    export class IScene
    {
    public:
        virtual ~IScene() = default;

        /// @brief シーンが開始（アクティブ化）された際に呼ばれる初期化処理
        virtual void Start() = 0;

        /// @brief シーンの毎フレームの更新処理を行う
        /// @param deltaTime 前回のフレームからの経過時間
        virtual void Update(float deltaTime) = 0;
    };


    /// @brief シーンの遷移やスタック管理を行うサービス
    export class ISceneService
    {
    public:
        virtual ~ISceneService() = default;

        /// @brief 現在アクティブなシーンの更新処理を行う
        /// @param deltaTime 前回のフレームからの経過時間
        virtual void Update(float deltaTime) = 0;

        /// @brief 新しいシーンをスタックに積み、アクティブにする
        /// @param type プッシュするシーンの種類
        virtual void PushScene(ESceneState type) = 0;

        /// @brief 現在のシーンをスタックからポップし、前のシーンに戻る
        virtual void PopScene() = 0;

        /// @brief シーンのインスタンスをサービスに登録する（初期化時に使用）
        /// @param type 登録するシーンの種類
        /// @param scene 登録するシーンのインスタンス（所有権を移動）
        virtual void RegisterScene(ESceneState type, std::unique_ptr<IScene>&& scene) = 0;

        /// @brief 現在アクティブなシーンの種類を取得する
        /// @return 現在のシーンの種類
        virtual ESceneState GetCurrentScene() = 0;

        /// @brief スタックをクリアし、指定されたシーンを現在のシーンとして設定する
        /// @param type 新しく設定するシーンの種類
        virtual void SetCurrentScene(ESceneState type) = 0;
    };

    export std::unique_ptr<ISceneService> CreateSceneService(ICharacterService& characterService);
    export std::unique_ptr<IScene> CreateInfoScene(IInputService& inputService, ISceneService& sceneService, IRenderService& renderService);
    export std::unique_ptr<IScene> CreateCombatScene(ICharacterService& characterService, ISceneService& sceneService, IAssetService& assetService, ICardService& cardService, IInputService& inputService, IRenderService& renderService);
    export std::unique_ptr<IScene> CreateRulesScene(IInputService& inputService, ISceneService& sceneService, IAssetService& assetService, IRenderService& renderService);
} // namespace mc

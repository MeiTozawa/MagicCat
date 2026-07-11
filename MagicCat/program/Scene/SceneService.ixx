module;

#include <memory>

export module SceneService;

import CardService;

import EventBus;
import InputService;
import AssetService;
import RenderService;
import AudioService;
import PersistenceService;
import BattleService;
import OSService;

namespace mc {
    export enum class ESceneState
    {
        Info, Combat, Menu, Cutscene
    };


    /// @brief カットシーンシーンに遷移した際に発行されるイベント
    export struct EnterCutSceneEvent : IEvent {};

    /// @brief カットシーンシーンから退出した際に発行されるイベント
    export struct ExitCutSceneEvent : IEvent {};

    /// @brief カットシーン全体の演出アニメーションが完了した際に発行されるイベント
    export struct CutsceneFinishedEvent : IEvent {};

    /// @brief プレイヤーがカード選択や魔法選択を変更した際に発行されるイベント
    export struct ActionSelectionEvent : IEvent
    {
        int selectedIndex;
        bool isMagicMenuOpen;
        bool silent; 

        explicit ActionSelectionEvent(const int selected_index, bool isMagicMenuOpen = false, bool silent = false)
            : selectedIndex(selected_index), isMagicMenuOpen(isMagicMenuOpen), silent(silent) {}
    };

    /// @brief 各画面（シーン）の基本クラス
    export class IScene
    {
    public:
        virtual ~IScene() = default;

        /// @brief シーン開始時の処理
        virtual void Start() = 0;

        /// @brief シーン更新処理
        /// @param deltaTime 前のフレームからの経過時間（秒）
        virtual void Update(float deltaTime) = 0;
    };

    /// @brief シーンの遷移やスタック管理を行うインターフェース
    export class ISceneService
    {
    public:
        virtual ~ISceneService() = default;

        /// @brief 現在アクティブなシーンの更新処理を行う
        /// @param deltaTime 前回のフレームからの経過時間（秒）
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

    export std::unique_ptr<ISceneService> CreateSceneService(IRenderService* renderService = nullptr,
                                                             IInputService* inputService = nullptr,
                                                             IOSService* osService = nullptr);
    export std::unique_ptr<IScene> CreateMenuScene(IInputService& inputService, ISceneService& sceneService,
                                                    IAssetService& assetService, IRenderService& renderService,
                                                    IAudioService& audioService, IPersistenceService& persistenceService,
                                                    IBattleService& battleService, IOSService& osService);
    export std::unique_ptr<IScene> CreateInfoScene(IInputService& inputService, ISceneService& sceneService,
                                                   IRenderService& renderService, IBattleService& battleService);
    export std::unique_ptr<IScene> CreateCombatScene(ISceneService& sceneService, IAssetService& assetService,
                                                     ICardService& cardService, IInputService& inputService,
                                                     IRenderService& renderService, IBattleService& battleService,
                                                     IOSService& osService);
    export std::unique_ptr<IScene> CreateCutsceneScene(ISceneService& sceneService,
                                                       IAssetService& assetService, IRenderService& renderService,
                                                       IBattleService& battleService);
} // namespace mc

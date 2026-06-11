// module;
//
// module AnimationFactory;
//
// import AssetService;
// import ServiceLocator;
// import EventBus;
// import CharacterService;
// import HealthComponent;
// import FadeAnimation;
//
// namespace mc
// {
//     class TextAnimation : public IAnimationPlayer
//     {
//         IEvent event;
//         std::wstring message;
//         int x, y;
//
//     public:
//         void Update(float deltaTime) override {}
//         bool IsPlaying() override { return true; }
//     };
//
//     std::unique_ptr<IAnimationPlayer> CreateTextAnimation(int playerDamage)
//     {
//         return std::make_unique<TextAnimation>(playerDamage);
//     }
// } // namespace mc

module;

export module EffectorFactory;
import AnimationFactory;
import Displayer;
namespace mc
{
    export class EffectorPlayer : virtual public IDisplayer
    {
    protected:
        std::unique_ptr<IDisplayer> displayer;
        bool isPlaying = false;

    public:
        explicit EffectorPlayer(std::unique_ptr<IDisplayer>&& displayer) : displayer(std::move(displayer)) {}
        ~EffectorPlayer() override = default;
        virtual void Play() = 0;
        virtual bool IsPlaying() final { return isPlaying; }
        void Update(float deltaTime) override = 0;
        void Draw(float deltaTime) const override = 0;
    };

    export std::unique_ptr<EffectorPlayer> CreateHitFlashEffector(
        std::unique_ptr<IDisplayer>&& displayer, uint32_t color, int flashTime = 300
    );
    export std::unique_ptr<EffectorPlayer> CreateFadeEffector(
        std::unique_ptr<IDisplayer>&& displayer, int fadeInTime, int holdTime, int fadeOutTime
    );
}

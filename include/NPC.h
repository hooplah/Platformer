#ifndef NPC_H
#define NPC_H

#include <memory>

#include "AIModuleProcessor.h"
#include "HealthBar.h"
#include "Weapon.h"
#include "WorldRef.h"
#include "Player.h"
#include "FuncUtils.h"

class IAIModule;

class NPC : public SpriteObject, public ICollideable
{
    public:
        NPC(SpriteInfo& info, sf::Vector2f pos, WorldRef& worldRef);
        ~NPC();

        void update(WorldRef& worldRef);
        void draw(sf::RenderTarget& target, float alpha);

        bool onContactBegin(std::weak_ptr<ICollideable> object, bool fromLeft, bool fromTop);
        void onContactEnd(std::weak_ptr<ICollideable> object);

        bool walk(sf::Vector2f pos);
        void stop();
        void jump(int dir);
        void respawn();

        void setDirection(int dir){mDirection=dir;}
        void setNeedToUpdatePath(bool update){mNeedToUpdatePath=update;}

        bool isJumping(){return mJumping;}
        bool needToUpdatePath(){return mNeedToUpdatePath;}
        int getDirection(){return mDirection;}
        Weapon& getWeapon(){return mWeapon;}
        std::weak_ptr<ICollideable> getTarget(){return mTarget;}
        int getKillerTag(){return mKillerTag;}
        float getWeaponAngle(){return mWeaponAngle;}
        sf::Vector2f getFeetPosition(){return (mRenderPosition+sf::Vector2f(mHitBox.width, mHitBox.height));}
        HealthBar& getHealth(){return mHealth;}

    protected:
        sf::Vector2f mSpawnPoint;
        float mRunSpeed;
        float mJumpSpeed;
        int mDirection; // direction npc is facing - for animations
        bool mGrounded;
        bool mJumping;
        bool mNeedToUpdatePath;

        HealthBar mHealth;

        AIModuleProcessor mAIModuleProcessor;

        Weapon mWeapon;
        std::weak_ptr<ICollideable> mTarget;
        int mKillerTag; // what kind of entity killed this npc
        float mWeaponAngle;
};

#endif // NPC_H

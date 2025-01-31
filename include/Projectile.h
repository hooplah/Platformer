#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "SpriteObject.h"
#include "ICollideable.h"
#include "Constants.h"

class Projectile : public SpriteObject, public ICollideable
{
    public:
        Projectile(SpriteInfo& info, sf::Vector2f pos, int damage, float range, int ownerTag);
        ~Projectile();

        virtual void update(WorldRef& worldRef);
        void draw(sf::RenderTarget& target, float alpha);

        // Accessors
        int getDamage(){return mDamage;}
        int getOwnerTag(){return mOwnerTag;}

        // Mutators
        virtual void setFiringAngle(float angle){mFiringAngle=angle; mSprite.setRotation(angle*RADTODEG);}
        void setSpeed(float speed){mSpeed=speed;}

    protected:
        int mDamage;
        float mSpeed;
        float mFiringAngle;
        float mDistanceLeft;
        int mOwnerTag;
};

#endif // PROJECTILE_H

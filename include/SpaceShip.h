#ifndef SPACESHIP_H
#define SPACESHIP_H

#include <vector>

#include "SpriteObject.h"
#include "ICollideable.h"
#include "Weapon.h"
#include "WorldRef.h"

class SpaceShip : public SpriteObject, public ICollideable
{
    public:
        SpaceShip(SpriteInfo& info, sf::Vector2f pos);
        ~SpaceShip();

        void update(WorldRef& worldRef);
        void draw(sf::RenderTarget& target, float alpha);
        //void drawStationary(sf::RenderTarget& target);
        void handleEvents(sf::Event& event, WorldRef& worldRef);

        bool onContactBegin(std::weak_ptr<ICollideable> object, bool fromLeft, bool fromTop);
        void onContactEnd(std::weak_ptr<ICollideable> object);

    private:
        float mAcceleration;

        std::vector<Weapon> mWeapons;
        sf::Vector2f mMousePosition;
        sf::Vector2f mWeaponTarget;
        float mWeaponAngle;
};

#endif // SPACESHIP_H

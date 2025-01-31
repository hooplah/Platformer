#include "Player.h"

#include <iostream>

#include "Assets.h"
#include "Constants.h"
#include "FuncUtils.h"
#include "Direction.h"
#include "EntityTags.h"
#include "Projectile.h"
#include "SpaceShip.h"
#include "GrenadeLauncher.h"

Player::Player(SpriteInfo& info, sf::Vector2f pos, WorldRef& worldRef) :
    SpriteObject(info, pos),
    ICollideable(info.mHitBox, info.mFrameDim, EntityTags::PLAYER),
    mHealth(100.f, sf::Vector2f(30.f, 2.f), false),
    mInventoryHUD(sf::Vector2f(0, 0))
{
    mRunSpeed = 3.f;
    mJumpSpeed = 5.5f;
    mDirection = Direction::STILL_RIGHT;
    mGrounded = false;
    mJumping = true;
    mPhysicsPosition = pos;
    mFallDamageRate = 10.f;

    mInventory.push_back(std::move(std::make_unique<Weapon>(Assets::sprites["pistol"], EntityTags::PLAYER)));
    mInventory.push_back(std::move(std::make_unique<GrenadeLauncher>(Assets::sprites["grenade"], EntityTags::PLAYER)));
    mInventoryHUD.addInventoryItem(SpriteObject(Assets::sprites["pistol"], sf::Vector2f()));
    mInventoryHUD.addInventoryItem(SpriteObject(Assets::sprites["grenade"], sf::Vector2f()));

    mInVehicle = false;
    mHealth.mActive = false;
}

Player::~Player()
{
    //dtor
}

void Player::update(WorldRef& worldRef)
{
    SpriteObject::update();

    mOldPhysicsPosition = mPhysicsPosition;
    mPhysicsPosition += mVelocity;

    // animations
    if (!mGrounded) // above ground
    {
        if (mDirection == Direction::LEFT)
            setFrameLoop(33, 33);
        else if (mDirection == Direction::STILL_LEFT)
            setFrameLoop(35, 35);

        else if (mDirection == Direction::RIGHT)
            setFrameLoop(30, 30);
        else if (mDirection == Direction::STILL_RIGHT)
            setFrameLoop(32, 32);
    }
    else if (mGrounded) // walking on ground
    {
        if (mDirection == Direction::LEFT)
            setFrameLoop(12, 17);
        else if (mDirection == Direction::STILL_LEFT)
            setFrameLoop(0, 5);

        else if (mDirection == Direction::RIGHT)
            setFrameLoop(18, 23);
        else if (mDirection == Direction::STILL_RIGHT)
            setFrameLoop(6, 11);
    }

    if (mHealth.mHP <= 0.f)
    {
        kill();
    }

    if (mInVehicle)
        mPhysicsPosition = mVehicle.lock()->getPhysicsPosition();//+mVehicle.lock()->getCenter();

    if (mInventory.getItem(mInventoryHUD.getInventoryIndex())->getName() == "Weapon")
    {
        auto weap = static_cast<Weapon*>(&*mInventory.getItem(mInventoryHUD.getInventoryIndex()));

        weap->setFiringAngle(mWeaponAngle);
        weap->setRenderPosition(mRenderPosition + getCenter());

        sf::Vector2f weapFirePoint = weap->getRenderPosition();//+weap->getFirePoint();
        mWeaponAngle = atan2(mWeaponTarget.y - weapFirePoint.y, mWeaponTarget.x - weapFirePoint.x);
    }

    mInventory.getItem(mInventoryHUD.getInventoryIndex())->update();
    mHealth.setPosition(mRenderPosition+getCenter()+sf::Vector2f(0.f, -32.f));
}

void Player::draw(sf::RenderTarget& target, float alpha)
{
    if (!mInVehicle)
        SpriteObject::draw(target, alpha);

    mRenderPosition = mPhysicsPosition*alpha + mOldPhysicsPosition*(1.f - alpha);

    mHealth.draw(target);

    //mInventory.getItem(mInventoryHUD.getInventoryIndex())->draw(target, alpha);
    mWeaponTarget = target.mapPixelToCoords(sf::Vector2i(mMousePosition.x, mMousePosition.y));
}

void Player::drawStationary(sf::RenderTarget& target)
{
    if (!mInVehicle)
        mInventoryHUD.draw(target);

    // TODO: move to inventory item ui when there is one
    /*std::string sWeaponText = std::to_string(mWeapon.getMagazines());
    sWeaponText.append("/");
    sWeaponText.append(std::to_string(mWeapon.getAmmo()));
    sf::Text weaponText(sWeaponText, Assets::fonts["8bit"].mFont);
    target.draw(weaponText); // magazine / ammo_in_magazine*/
}

void Player::handleEvents(sf::Event& event, WorldRef& worldRef)
{
    mInventoryHUD.handleEvents(event, mInventory.getItemList().size());

    if (!mInVehicle)
    {
        if (event.type == sf::Event::MouseButtonPressed)
        {
            if (event.mouseButton.button == sf::Mouse::Left)
            {
                if (mWeaponTarget.x < mRenderPosition.x)
                {
                    if (mDirection != Direction::STILL_LEFT && mDirection != Direction::STILL_RIGHT) // moving
                        mDirection = Direction::LEFT;
                    else // still
                        mDirection = Direction::STILL_LEFT;
                }
                else if (mWeaponTarget.x > mRenderPosition.x)
                {
                    if (mDirection != Direction::STILL_LEFT && mDirection != Direction::STILL_RIGHT) // moving
                        mDirection = Direction::RIGHT;
                    else // still
                        mDirection = Direction::STILL_RIGHT;
                }

                mInventory.getItem(mInventoryHUD.getInventoryIndex())->use(worldRef);
            }
        }
        else if (event.type == sf::Event::MouseMoved)
        {
            mMousePosition = sf::Vector2f(event.mouseMove.x, event.mouseMove.y);
        }
        else if (event.type == sf::Event::KeyPressed)
        {
            if (event.key.code == sf::Keyboard::Space && !mJumping)
            {
                mJumping = true;
                mVelocity.y = 0.f;
                mVelocity.y -= mJumpSpeed;
                mGrounded = false;
            }
            if (event.key.code == sf::Keyboard::A)
            {
                mVelocity.x = -mRunSpeed;
                mDirection = Direction::LEFT;
            }
            else if (event.key.code == sf::Keyboard::D)
            {
                mVelocity.x = mRunSpeed;
                mDirection = Direction::RIGHT;
            }
            else if (event.key.code == sf::Keyboard::E)
            {
                if (worldRef.getObjectsWithinArea(EntityTags::VEHICLE, sf::FloatRect(mRenderPosition-sf::Vector2f(300.f, 300.f),
                sf::Vector2f(600.f, 600.f))).size() > 0)
                {
                    if (!worldRef.getClosestObject(EntityTags::VEHICLE, mRenderPosition).expired())
                    {
                        auto vehicle = worldRef.getClosestObject(EntityTags::VEHICLE, mRenderPosition);
                        if (vehicle.lock()->getTag() == EntityTags::VEHICLE);
                        {
                            if (length(vehicle.lock()->getPhysicsPosition() - mPhysicsPosition) < 100)
                            {
                                mInVehicle = true;
                                mVehicle = std::static_pointer_cast<Vehicle>(vehicle.lock());
                                mVehicle.lock()->setInVehicle(true);
                            }
                        }
                    }
                }
            }
        }

        else if (event.type == sf::Event::KeyReleased)
        {
            if (event.key.code == sf::Keyboard::A)
            {
                mVelocity.x  = 0.f;
                mDirection = Direction::STILL_LEFT;
            }
            else if (event.key.code == sf::Keyboard::D)
            {
                mVelocity.x  = 0.f;
                mDirection = Direction::STILL_RIGHT;
            }

            if (event.key.code == sf::Keyboard::R)
            {
                //mWeapon.reload();
            }
        }
        else if (event.type == sf::Event::MouseWheelMoved)
        {
            if (event.mouseWheel.delta > 0)
            {
                if (mInventoryHUD.getInventoryIndex() == mInventory.getItemList().size()-1)
                    mInventoryHUD.setInventoryIndex(0);
                else
                    mInventoryHUD.setInventoryIndex(mInventoryHUD.getInventoryIndex()+1);
            }

            else if (event.mouseWheel.delta < 0)
            {
                if (mInventoryHUD.getInventoryIndex() == 0)
                    mInventoryHUD.setInventoryIndex(mInventory.getItemList().size()-1);
                else
                    mInventoryHUD.setInventoryIndex(mInventoryHUD.getInventoryIndex()-1);
            }

        }
    }
    else if (mInVehicle)
    {
        mVehicle.lock()->handleEvents(event, worldRef);

        if (event.type == sf::Event::KeyPressed)
        {
            if (event.key.code == sf::Keyboard::E)
            {
                mInVehicle = false;
                mVelocity = sf::Vector2f(0.f, 0.f);
                mVehicle.lock()->setVelocity(sf::Vector2f(0.f, 0.f));
                mVehicle.lock()->setInVehicle(false);
                mDirection = Direction::STILL_RIGHT;
                mPhysicsPosition = mVehicle.lock()->getPhysicsPosition();
            }
        }
    }
}

void Player::stop()
{
    if (mDirection == Direction::LEFT)
        mDirection = Direction::STILL_LEFT;
    else if (mDirection == Direction::RIGHT)
        mDirection = Direction::STILL_RIGHT;

    mVelocity = sf::Vector2f(0.f, mVelocity.y);
}

void Player::respawn(sf::Vector2f pos)
{
    mHealth.mHP = mHealth.mMaxHP;
    //mWeapon.setWeaponClips(3, 10);
    mPhysicsPosition = pos;
    mRenderPosition = pos;
    mVelocity = sf::Vector2f(0.f, 0.f);
    mAlive = true;
}

bool Player::onContactBegin(std::weak_ptr<ICollideable> object, bool fromLeft, bool fromTop)
{
    if (mInVehicle)
        return false;

    if (object.lock()->isStatic() && fromTop)
    {
        if (mVelocity.y/mFallDamageRate > 1.f)
        {
            mHealth.mHP -= mVelocity.y; // terraria fall damage = 10(h - 400), where h = fall distance
            mHealth.mActive = true;
            mHealth.mActiveClock.restart();
        }

        mGrounded = true;
        mJumping = false;
    }

    if (object.lock()->getTag() == EntityTags::PROJECTILE)
    {
        Projectile* proj = static_cast<Projectile*>(&*object.lock());

        if (proj->getOwnerTag() != mTag)
        {
            mHealth.mHP -= proj->getDamage();
            //mHealth.mActive = true;
            //mHealth.mActiveClock.restart();

            proj->kill();
        }

        return false;
    }

    return true;
}

void Player::onContactEnd(std::weak_ptr<ICollideable> object)
{
}

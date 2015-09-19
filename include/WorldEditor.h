#ifndef WORLDEDITOR_H
#define WORLDEDITOR_H

#include <SFML/Graphics.hpp>

#include <vector>

#include "World.h"
#include "WorldObject.h"

class WorldEditorObject : public WorldObject
{
    public:
        WorldEditorObject(SpriteInfo& info, sf::Vector2f pos, std::string id) : WorldObject(info, pos), mID(id){}
        ~WorldEditorObject(){}

        std::string getID(){return mID;}

    private:
        std::string mID;
};

class WorldEditor
{
    public:
        WorldEditor(std::string path);
        ~WorldEditor();

        void update(int ticks);
        void draw(sf::RenderTarget& target, float alpha);
        void handleEvents(sf::Event& event);

        void loadWorld();
        void saveWorld();

    private:
        bool checkCollision(std::weak_ptr<ICollideable> a, std::weak_ptr<ICollideable> b);
        void resolveCollision(std::weak_ptr<ICollideable> a, std::weak_ptr<ICollideable> b);

        template <class T>
        std::vector<T> removeDeadObj(std::vector<T>& v);
        template <class T>
        std::vector<T> removeWeakDeadObj(std::vector<T>& v);

        std::string mDirectoryPath;

        sf::Vector2i mLocalMousePosition;
        sf::Vector2f mGlobalMousePosition;
        sf::Vector2f mCameraPosition;

        std::vector<std::string> mIDs;
        int mCurrentID;

        Camera mCamera;

        std::weak_ptr<WorldObject> mDragObject;
        bool mPlayingHero;
        std::shared_ptr<Player> mHero;

        std::vector<std::shared_ptr<WorldEditorObject>> mWorldObjects;
        std::vector<std::weak_ptr<ICollideable>> mCollideables;

        World mWorld;
};

#endif // WORLDEDITOR_H

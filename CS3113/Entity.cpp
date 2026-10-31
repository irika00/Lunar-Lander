#include "Entity.h"

Entity::Entity() :
    mPosition{ 0.0f, 0.0f }, mMovement{ 0.0f, 0.0f },
    mVelocity{ 0.0f, 0.0f }, mAcceleration{ 0.0f, 0.0f },
    mScale{ DEFAULT_SIZE, DEFAULT_SIZE },
    mColliderDimensions{ DEFAULT_SIZE, DEFAULT_SIZE },
    mTexture{}, mTextureType{ SINGLE }, mSpriteSheetDimensions{},
    mAnimationAtlas{}, mAnimationIndices{}, mDirection{ RIGHT },
    mFrameSpeed{ 0 }, mSpeed{ DEFAULT_SPEED }, mAngle{ 0.0f } {}

Entity::Entity(Vector2 position, Vector2 scale, const char *textureFilepath) :
    mPosition{ position }, mMovement{ 0.0f, 0.0f },
    mVelocity{ 0.0f, 0.0f }, mAcceleration{ 0.0f, 0.0f },
    mScale{ scale }, mColliderDimensions{ scale },
    mTexture{ LoadTexture(textureFilepath) }, mTextureType{ SINGLE },
    mSpriteSheetDimensions{}, mAnimationAtlas{}, mAnimationIndices{},
    mDirection{ RIGHT }, mFrameSpeed{ 0 }, mSpeed{ DEFAULT_SPEED },
    mAngle{ 0.0f } {}

Entity::Entity(
    Vector2                               position,
    Vector2                               scale,
    const char                           *textureFilepath,
    TextureType                           textureType,
    Vector2                               spriteSheetDimensions,
    std::map<Direction, std::vector<int>> animationAtlas
) :
    mPosition{ position }, mMovement{ 0.0f, 0.0f },
    mVelocity{ 0.0f, 0.0f }, mAcceleration{ 0.0f, 0.0f },
    mScale{ scale }, mColliderDimensions{ scale },
    mTexture{ LoadTexture(textureFilepath) }, mTextureType{ textureType },
    mSpriteSheetDimensions{ spriteSheetDimensions },
    mAnimationAtlas{ animationAtlas },
    mAnimationIndices{ animationAtlas.at(UP) }, mDirection{ UP },
    mFrameSpeed{ DEFAULT_FRAME_SPEED }, mSpeed{ DEFAULT_SPEED },
    mAngle{ 0.0f } {}

Entity::~Entity() { UnloadTexture(mTexture); }

/**
 * Checks if two entities are colliding based on their positions and collider
 * dimensions.
 *
 * @param other represents another Entity with which you want to check for
 * collision. It is a pointer to the Entity class.
 *
 * @return returns `true` if the two entities are colliding based on their
 * positions and collider dimensions, and `false` otherwise.
 */
bool Entity::isColliding(Entity *other) const
{
    //if (!other->isActive() || other == this) return false;

    float xDistance = fabs(mPosition.x - other->getPosition().x) -
        ((mColliderDimensions.x + other->getColliderDimensions().x) / 2.0f);
    float yDistance = fabs(mPosition.y - other->getPosition().y) -
        ((mColliderDimensions.y + other->getColliderDimensions().y) / 2.0f);

    return xDistance < 0.0f && yDistance < 0.0f;
}

/**
 * Updates the current frame index of an entity's animation based on the
 * elapsed time and frame speed.
 *
 * @param deltaTime represents the time elapsed since the last frame update.
 */
void Entity::animate(float deltaTime)
{
    mAnimationIndices = mAnimationAtlas.at(mDirection);

    mAnimationTime += deltaTime;
    float secondsPerFrame = 1.0f / mFrameSpeed;

    if (mAnimationTime >= secondsPerFrame)
    {
        mAnimationTime = 0.0f;

        mCurrentFrameIndex++;
        mCurrentFrameIndex %= mAnimationIndices.size();
    }
}

//for drifting platform
void Entity::updatePlatform(float deltaTime)
{
    if (mMoveSpeed == 0.0f) return;

    mPosition.x += mMoveDir * mMoveSpeed * deltaTime;

    if (mPosition.x > mMaxX) { mPosition.x = mMaxX; mMoveDir = -1; }
    if (mPosition.x < mMinX) { mPosition.x = mMinX; mMoveDir =  1; }
}

void Entity::checkCollisionY(Entity *collidableEntities, int collisionCheckCount)
{
    for (int i = 0; i < collisionCheckCount; i++)
    {
        Entity *collidableEntity = &collidableEntities[i];

        if (isColliding(collidableEntity))
        {
            float yDistance = fabs(mPosition.y - collidableEntity->mPosition.y);
            float yOverlap  = fabs(yDistance - (mColliderDimensions.y / 2.0f) - (collidableEntity->mColliderDimensions.y / 2.0f));

            if (mVelocity.y > 0)
            {
                mPosition.y -= yOverlap;       
                mImpactSpeedY = mVelocity.y; 
                bool supported = isSupported(collidableEntities, collisionCheckCount);
                if (mDirection != UP || !supported) mCrashed = true;  
                mHasPhysics   = false;        
                return;
            }
            else if (mVelocity.y < 0)
            {
                mPosition.y += yOverlap;
                mCrashed = true;
                mHasPhysics  = false;
                return;

            }
        }
    }
}

// Entity.cpp
void Entity::checkCollisionX(Entity *collidableEntities, int collisionCheckCount)
{
    for (int i = 0; i < collisionCheckCount; i++)
    {
        Entity *collidableEntity = &collidableEntities[i];

        if (isColliding(collidableEntity))
        {

            float xDistance = fabs(mPosition.x - collidableEntity->mPosition.x);
            float xOverlap  = fabs(xDistance - (mColliderDimensions.x / 2.0f) - (collidableEntity->mColliderDimensions.x / 2.0f));


            float yDistance = fabs(mPosition.y - collidableEntity->mPosition.y);
            float yOverlap  = fabs(yDistance - (mColliderDimensions.y / 2.0f) - (collidableEntity->mColliderDimensions.y / 2.0f));

            if (yOverlap < Y_COLLISION_THRESHOLD) continue;

           if (mVelocity.x > 0)      { mPosition.x -= xOverlap;}
            else if (mVelocity.x < 0) { mPosition.x += xOverlap;}

            mCrashed = true;
            mHasPhysics = false;
            return;

        }
    }
}


void Entity::update(float deltaTime, Entity *collidableEntities, int collisionCheckCount)
{
    if (!mHasPhysics) return;
    if (mUseDirectionalCollider)
        mColliderDimensions = (mDirection == UP) ? mColliderUp : mColliderSide;
    mPosition.y += mVelocity.y * deltaTime; 
    checkCollisionY(collidableEntities, collisionCheckCount);
    mPosition.x += mVelocity.x * deltaTime;
    checkCollisionX(collidableEntities, collisionCheckCount);


}
void Entity::displayCollider()
{
    // draw the collision box
    Rectangle colliderBox = {
        mPosition.x - mColliderDimensions.x / 2.0f,
        mPosition.y - mColliderDimensions.y / 2.0f,
        mColliderDimensions.x,
        mColliderDimensions.y
    };

    DrawRectangleLines(
        colliderBox.x,      // Top-left X
        colliderBox.y,      // Top-left Y
        colliderBox.width,  // Width
        colliderBox.height, // Height
        GREEN               // Color
    );
}


void Entity::update(float deltaTime)
{
    if (!mHasPhysics) return;

    if (mFuel <= 0.0f)
        mAcceleration = { 0.0f, mGravity };

    bool pushX = mAcceleration.x != 0.0f;
    bool pushY = mAcceleration.y != mGravity;

    int active = pushX + pushY;
    mFuel -= mBurnRate * active * deltaTime;
    if (mFuel < 0.0f) mFuel = 0.0f;


    float acc = mAcceleration.x;
    if (!pushX)
    {
        if (fabs(mVelocity.x) > mBrake * deltaTime)
            acc = (mVelocity.x > 0) ? -mBrake : mBrake;
        else
            acc = -mVelocity.x / deltaTime;   
    }

    mVelocity.x += acc * deltaTime;
    mVelocity.y += mAcceleration.y * deltaTime;

    if (mTextureType == ATLAS && (pushX || pushY))
        animate(deltaTime);
}

//to see if ship is landing correctly
bool Entity::isSupported(Entity *tiles, int count) const
{
    float shipBottom = mPosition.y + mColliderDimensions.y / 2.0f;
    float shipL = mPosition.x - mColliderDimensions.x / 2.0f;
    float shipR = mPosition.x + mColliderDimensions.x / 2.0f;

    const int SAMPLES = 5;   

    for (int s = 0; s < SAMPLES; s++)
    {
        float x = shipL + (shipR - shipL) * s / (SAMPLES - 1);
        bool overTile = false;

        for (int i = 0; i < count; i++)
        {
            float top  = tiles[i].mPosition.y - tiles[i].mColliderDimensions.y / 2.0f;
            float left = tiles[i].mPosition.x - tiles[i].mColliderDimensions.x / 2.0f;
            float right = tiles[i].mPosition.x + tiles[i].mColliderDimensions.x / 2.0f;

            if (fabs(shipBottom - top) < 2.0f && x >= left && x <= right)
            {
                overTile = true;
                break;
            }
        }

        if (!overTile) return false;   
    }
    return true;
}
void Entity::render()
{
    Rectangle textureArea;

    switch (mTextureType)
    {
    case SINGLE:
        // Whole texture (UV coordinates)
        textureArea = {
            // top-left corner
            0.0f, 0.0f,

            // bottom-right corner (of texture)
            static_cast<float>(mTexture.width),
            static_cast<float>(mTexture.height)
        };
        break;
    case ATLAS:
        textureArea = getUVRectangle(
            &mTexture,
            mAnimationIndices[mCurrentFrameIndex],
            mSpriteSheetDimensions.x,
            mSpriteSheetDimensions.y
        );
        break;
    default:
        break;
    }

    // Destination rectangle – centred on mPosition
    Rectangle destinationArea = {
        mPosition.x,
        mPosition.y,
        static_cast<float>(mScale.x),
        static_cast<float>(mScale.y)
    };

    // Origin inside the source texture (centre of the texture)
    Vector2 originOffset = {
        static_cast<float>(mScale.x) / 2.0f,
        static_cast<float>(mScale.y) / 2.0f
    };

    if (mTextureType == ATLAS && mDirection == UP)
    {
        destinationArea.width = mScale.y * 0.6f;
        destinationArea.y    += mUpShift;
    }

    originOffset = { destinationArea.width / 2.0f, destinationArea.height / 2.0f };

    // Render the texture on screen
    DrawTexturePro(
        mTexture,
        textureArea, destinationArea, originOffset,
        mAngle, WHITE
    );
}


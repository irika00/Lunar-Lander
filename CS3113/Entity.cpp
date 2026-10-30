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
    mAnimationIndices{ animationAtlas.at(RIGHT) }, mDirection{ RIGHT },
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

// ...
void Entity::checkCollisionY(Entity *collidableEntities, int collisionCheckCount)
{
    for (int i = 0; i < collisionCheckCount; i++)
    {
        // STEP 1: For every entity that our player can collide with...
        Entity *collidableEntity = &collidableEntities[i];

        if (isColliding(collidableEntity))
        {
            // STEP 2: Calculate the distance between its centre and our centre
            //         and use that to calculate the amount of overlap between
            //         both bodies.
            float yDistance = fabs(mPosition.y - collidableEntity->mPosition.y);
            float yOverlap  = fabs(yDistance - (mColliderDimensions.y / 2.0f) - (collidableEntity->mColliderDimensions.y / 2.0f));

            // STEPS 3, 4, & 5: "Unclip" ourselves from the other entity, and
            // zero our vertical velocity.
            if (mVelocity.y > 0)
            {
                mPosition.y -= yOverlap;
                mVelocity.y  = 0;
            }
            else if (mVelocity.y < 0)
            {
            {
                mPosition.y += yOverlap;
                mVelocity.y  = 0;
            }
        }
    }
}}

void Entity::checkCollisionX(Entity *collidableEntities, int collisionCheckCount)
{
    for (int i = 0; i < collisionCheckCount; i++)
    {
        Entity *collidableEntity = &collidableEntities[i];

        if (isColliding(collidableEntity))
        {

            float xDistance = fabs(mPosition.x - collidableEntity->mPosition.x);
            float xOverlap  = fabs(xDistance - (mColliderDimensions.x / 2.0f) - (collidableEntity->mColliderDimensions.x / 2.0f));

            if (mVelocity.x > 0)
            {
                mPosition.x     -= xOverlap;
                mVelocity.x      = 0;
            }
            else if (mVelocity.x < 0)
            {
                mPosition.x    += xOverlap;
                mVelocity.x     = 0;
            }
        }
    }
}


void Entity::update(float deltaTime, Entity *collidableEntities, int collisionCheckCount)
{
   for (int i = 0; i < collisionCheckCount; i++)
    {
        if (isColliding(&collidableEntities[i])) return;
    }

    mPosition.y += mVelocity.y * deltaTime; // after adjusting velocity...
    checkCollisionY(collidableEntities, collisionCheckCount);
    mPosition.x += mVelocity.x * deltaTime;
    checkCollisionX(collidableEntities, collisionCheckCount);

}


void Entity::update(float deltaTime)
{
    // mPosition.x += mMovement.x * mSpeed * deltaTime;
    // mPosition.y += mMovement.y * mSpeed * deltaTime;

    // mAcceleration.x = mMovement.x * 100.0f; 

    // mVelocity.x += mAcceleration.x * deltaTime;
    // mVelocity.y += mAcceleration.y * deltaTime;

    // mPosition.x += mVelocity.x * deltaTime;
    // mPosition.y += mVelocity.y * deltaTime;
    if (!mHasPhysics) return; 
    bool hasFuel = mFuel > 0.0f;

    if (hasFuel && (mMovement.x != 0.0f || mMovement.y != 0.0f))
    {
        mAcceleration.x = mMovement.x * mThrust;
        mAcceleration.y = 4.0f + mMovement.y * mThrust; //4.0f = accerelation

        int active = (mMovement.x != 0.0f) + (mMovement.y != 0.0f);
        mFuel -= mBurnRate * active * deltaTime;
        if (mFuel < 0.0f) mFuel = 0.0f;
    }
    else
    {
        
        mAcceleration.x = 0.0f;
        mAcceleration.y = 4.0f;
    }

    mVelocity.x += mAcceleration.x * deltaTime;
    mVelocity.y += mAcceleration.y * deltaTime;

    mPosition.x += mVelocity.x * deltaTime;
    mPosition.y += mVelocity.y * deltaTime;


    if (mTextureType == ATLAS && GetLength(mMovement) != 0)
        animate(deltaTime);
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

    // Render the texture on screen
    DrawTexturePro(
        mTexture,
        textureArea, destinationArea, originOffset,
        mAngle, WHITE
    );
}

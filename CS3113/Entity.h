#ifndef ENTITY_H
#define ENTITY_H

#include "cs3113.h"

enum Direction { LEFT, UP, RIGHT }; 

class Entity
{
private:
    Vector2 mPosition;
    Vector2 mMovement;
    Vector2 mVelocity;
    Vector2 mAcceleration;

    Vector2 mScale;
    Vector2 mColliderDimensions;

    Texture2D   mTexture;
    TextureType mTextureType;
    Vector2     mSpriteSheetDimensions;

    std::map<Direction, std::vector<int>> mAnimationAtlas;
    std::vector<int>                      mAnimationIndices;
    Direction                             mDirection;
    int                                   mFrameSpeed;

    int   mCurrentFrameIndex = 0;
    float mAnimationTime     = 0.0f;

    int   mSpeed;
    float mThrust; 
    float mAngle;
    float mFuel;
    float mBurnRate;
    float mGravity = 0.0f;
    bool mHasPhysics = false;
    bool  mLanded      = false;
    float mImpactSpeedY = 0.0f;
    bool  mCrashed      = false;
    Vector2 mColliderUp   = { 87.0f, 159.0f };
    Vector2 mColliderSide = { 65.0f, 146.0f };
    bool    mUseDirectionalCollider = false;
    float mMoveSpeed = 0.0f;
    float mMinX = 0.0f, mMaxX = 0.0f;
    int   mMoveDir = 1;
    


    void animate(float deltaTime);
    void checkCollisionX(Entity *collidableEntities, int collisionCheckCount);
    void checkCollisionY(Entity *collidableEntities, int collisionCheckCount);

public:
    static constexpr int DEFAULT_SIZE        = 250;
    static constexpr int DEFAULT_SPEED       = 200;
    static constexpr int DEFAULT_FRAME_SPEED = 14;
    static constexpr float Y_COLLISION_THRESHOLD = 0.5f;
    float mUpShift = 34.0f; //to exclude the flame from ship
    float mBrake = 60.0f; 

    Entity();
    Entity(Vector2 position, Vector2 scale, const char *textureFilepath);
    Entity(
        Vector2                               position,
        Vector2                               scale,
        const char                           *textureFilepath,
        TextureType                           textureType,
        Vector2                               spriteSheetDimensions,
        std::map<Direction, std::vector<int>> animationAtlas
    );
    ~Entity();

    void update(float deltaTime);
    void displayCollider();
    void render();
    void updatePlatform(float deltaTime);
    void normaliseMovement() { Normalise(&mMovement); }

    void moveUp()    { mMovement.y = -1; mDirection = UP;    }
    void moveLeft()  { mMovement.x = -1; mDirection = LEFT;  }
    void moveRight() { mMovement.x =  1; mDirection = RIGHT; }
    void resetAcceleration()  { mAcceleration = { 0.0f, mGravity }; }   

    void accelerateLeft()  { mAcceleration.x = -mThrust;            mDirection = LEFT;  }
    void accelerateRight() { mAcceleration.x =  mThrust;            mDirection = RIGHT; }
    void accelerateUp()    { mAcceleration.y =  mGravity - mThrust; mDirection = UP;    }
    bool isSupported(Entity *tiles, int count) const;
    void resetMovement() { mMovement = { 0.0f, 0.0f }; }
    void update(float deltaTime, Entity *collidableEntities, int collisionCheckCount);

    Vector2     getPosition()              const { return mPosition;              }
    Vector2     getMovement()              const { return mMovement;              }
    Vector2     getVelocity()              const { return mVelocity;              }
    Vector2     getAcceleration()          const { return mAcceleration;          }
    Vector2     getScale()                 const { return mScale;                 }
    Vector2     getColliderDimensions()    const { return mColliderDimensions;    }
    Vector2     getSpriteSheetDimensions() const { return mSpriteSheetDimensions; }
    Texture2D   getTexture()               const { return mTexture;               }
    TextureType getTextureType()           const { return mTextureType;           }
    Direction   getDirection()             const { return mDirection;             }
    int         getFrameSpeed()            const { return mFrameSpeed;            }
    int         getSpeed()                 const { return mSpeed;                 }
    float       getAngle()                 const { return mAngle;                 }
    float       getFuel()                 const { return mFuel;                 }
    float       getThrust()                 const { return mThrust;                 }
    float       getBurnRate()               const {return mBurnRate;               }
    bool isColliding(Entity *other) const;
    bool  hasLanded()      const { return mLanded; }
    bool hasPhysics()       const {return mHasPhysics;}
    float getImpactSpeedY() const { return mImpactSpeedY; }
    bool  hasCrashed()      const { return mCrashed; }

    std::map<Direction, std::vector<int>> getAnimationAtlas() const { return mAnimationAtlas; }

    void setPosition(Vector2 newPosition)
        { mPosition = newPosition;                 }
    void setMovement(Vector2 newMovement)
        { mMovement = newMovement;                 }
    void setAcceleration(Vector2 newAcceleration)
        { 
            mAcceleration = newAcceleration; 
            mGravity = newAcceleration.y;
                
        }
    void setScale(Vector2 newScale)
        { mScale = newScale;                       }
    void setTexture(const char *textureFilepath)
        { mTexture = LoadTexture(textureFilepath); }
    void setColliderDimensions(Vector2 newDimensions)
        { mColliderDimensions = newDimensions;     }
    void setSpriteSheetDimensions(Vector2 newDimensions)
        { mSpriteSheetDimensions = newDimensions;  }
    void setSpeed(int newSpeed)
        { mSpeed = newSpeed;                       }
    void setFrameSpeed(int newSpeed)
        { mFrameSpeed = newSpeed;                  }
    void setAngle(float newAngle)
        { mAngle = newAngle;                       }
    void setFuel(float newFuel)
        { mFuel = newFuel;                           }
    void setThrust(float newThrust)
        { mThrust = newThrust;                      }
    void setBurnRate(float newBurnRate)
        { mBurnRate = newBurnRate;                  }
    void setPhysics(bool on) { mHasPhysics = on; }
    void setDirectionalColliders(Vector2 up, Vector2 side)
    {
        mColliderUp = up;
        mColliderSide = side;
        mUseDirectionalCollider = true;
        mColliderDimensions = (mDirection == UP) ? up : side;
    }
    void setPlatformMotion(float speed, float minX, float maxX)
    { mMoveSpeed = speed; mMinX = minX; mMaxX = maxX; }
};

#endif // ENTITY_H

//
// Created by Lucas N. Ferreira on 28/09/23.
//

#include "AABBColliderComponent.h"
#include "../../Actors/Actor.h"
#include "../../Game.h"

AABBColliderComponent::AABBColliderComponent(class Actor* owner, int dx, int dy, int w, int h,
        ColliderLayer layer, bool isStatic, int updateOrder)
        :Component(owner, updateOrder)
        ,mOffset(Vector2((float)dx, (float)dy))
        ,mIsStatic(isStatic)
        ,mWidth(w)
        ,mHeight(h)
        ,mLayer(layer)
{
    GetGame()->AddCollider(this);
}

AABBColliderComponent::~AABBColliderComponent() {
    GetGame()->RemoveCollider(this);
}

Vector2 AABBColliderComponent::GetMin() const {

    Vector2 pos = mOwner->GetPosition() + mOffset;
    pos.x -= (mWidth/2.0f);
    pos.y -= (mHeight/2.0f);
    return pos;
}

Vector2 AABBColliderComponent::GetMax() const {
    Vector2 pos = mOwner->GetPosition() + mOffset;
    pos.x += (mWidth/2.0f);
    pos.y += (mHeight/2.0f);
    return pos;
}

bool AABBColliderComponent::Intersect(const AABBColliderComponent& b) const {
    Vector2 aMax = this->GetMax();
    Vector2 aMin = this->GetMin();
    Vector2 bMax = b.GetMax();
    Vector2 bMin = b.GetMin();
    if(aMin.x <= bMax.x && aMax.x >= bMin.x && 
       aMin.y <= bMax.y && aMax.y >= bMin.y) return true;

    return false;
}

void AABBColliderComponent::SetSize(int w, int h){
    mWidth = w;
    mHeight = h;
}

float AABBColliderComponent::GetMinVerticalOverlap(AABBColliderComponent* b) const {
    if(!this->Intersect(*b)) return 0.0f;

    Vector2 aMax = this->GetMax();
    Vector2 aMin = this->GetMin();
    Vector2 bMax = b->GetMax();
    Vector2 bMin = b->GetMin();

    return min(aMax.y - bMin.y, bMax.y - aMin.y);

}

float AABBColliderComponent::GetMinHorizontalOverlap(AABBColliderComponent* b) const {

    if(!this->Intersect(*b)) return 0.0f;

    Vector2 aMax = this->GetMax();
    Vector2 aMin = this->GetMin();
    Vector2 bMax = b->GetMax();
    Vector2 bMin = b->GetMin();

    return min(aMax.x - bMin.x, bMax.x - aMin.x);

}

float AABBColliderComponent::DetectHorizontalCollision(RigidBodyComponent *rigidBody) {
    if (mIsStatic) return 0.0f;

    auto colliders = GetGame()->GetColliders();
    float minOverlap = 0.0f;

    for(auto collider : colliders) {
        if(!collider->IsEnabled() || collider == this) continue;

        if(this->Intersect(*collider)) {
            float verticalOverlap = this->GetMinVerticalOverlap(collider);
            float horizontalOverlap = this->GetMinHorizontalOverlap(collider);

            if(horizontalOverlap < verticalOverlap) {
                minOverlap = horizontalOverlap;
                
                float rightOverlap = this->GetMax().x - collider->GetMin().x;
                float leftOverlap = collider->GetMax().x - this->GetMin().x;
                
                float displacement = (rightOverlap < leftOverlap) ? -minOverlap : minOverlap;

                this->ResolveHorizontalCollisions(rigidBody, displacement);
                mOwner->OnHorizontalCollision(minOverlap, collider); 
                return minOverlap;
            }
        }
    }

    return 0.0f;
}

float AABBColliderComponent::DetectVerticalCollision(RigidBodyComponent *rigidBody) {
    if (mIsStatic) return 0.0f;

    auto colliders = GetGame()->GetColliders();
    float minOverlap = 0.0f;
    
    for(auto collider : colliders) {
        if(!collider->IsEnabled() || collider == this) continue;

        if(this->Intersect(*collider)) {
            float verticalOverlap = this->GetMinVerticalOverlap(collider);
            float horizontalOverlap = this->GetMinHorizontalOverlap(collider);
            
            if(verticalOverlap <= horizontalOverlap) {
                minOverlap = verticalOverlap;

                float downOverlap = this->GetMax().y - collider->GetMin().y;
                float upOverlap = collider->GetMax().y - this->GetMin().y;

                float displacement = (downOverlap < upOverlap) ? -minOverlap : minOverlap;
                
                mOwner->OnVerticalCollision(minOverlap, collider);
                collider->GetOwner()->OnVerticalCollision(minOverlap, this);
                
                this->ResolveVerticalCollisions(rigidBody, displacement);
                return minOverlap;
            }
        }
    }

    return 0.0f;
}

void AABBColliderComponent::ResolveHorizontalCollisions(RigidBodyComponent *rigidBody, const float minXOverlap) {
    Vector2 pos = mOwner->GetPosition();
    pos.x += minXOverlap;
    mOwner->SetPosition(pos);

    Vector2 vel = rigidBody->GetVelocity();
    vel.x = 0.0f;
    rigidBody->SetVelocity(vel);
}

void AABBColliderComponent::ResolveVerticalCollisions(RigidBodyComponent *rigidBody, const float minYOverlap) {
    Vector2 pos = mOwner->GetPosition();
    pos.y += minYOverlap;
    mOwner->SetPosition(pos);

    Vector2 vel = rigidBody->GetVelocity();
    vel.y = 0.0f;
    rigidBody->SetVelocity(vel);

    if(minYOverlap < 0.0f) mOwner->SetOnGround();

}

void AABBColliderComponent::DebugDraw(class Renderer *renderer)
{
    renderer->DrawRect(mOwner->GetPosition() + mOffset,Vector2((float)mWidth, (float)mHeight), mOwner->GetRotation(),
                       Color::Green, mOwner->GetGame()->GetCameraPos(), RendererMode::LINES);
}
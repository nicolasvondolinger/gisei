#include "Ninja.h"
#include "Dart.h"
#include "Block.h"
#include "../Game.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Drawing/DashAfterimageComponent.h"
#include "../Components/Physics/RigidBodyComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"
#include "SkeletonWarrior.h"
#include "SkeletonSpearman.h"
#include "SkeletonArcher.h"
#include "KarasuTengu.h"
#include "SkeletonWarrior.h"
#include "SkeletonSpearman.h"
#include "SkeletonArcher.h"

class EnergyLance : public Actor {
public:
    EnergyLance(Game* game, Vector2 pos, float dir) : Actor(game) {
        // CORREÇÃO: Tamanho Quadrado (64x64) para manter proporção do frame 32x32 (escala 2x)
        // Se quiser o tamanho original exato, coloque 32, 32.
        auto* anim = new AnimatorComponent(this, 64, 64);
        
        // CORREÇÃO: 8 Frames conforme sua imagem
        anim->AddAnimation("cast", "../Assets/Sprites/Magic/3.png", 8, 24.0f, false);
        anim->SetAnimation("cast");

        // CORREÇÃO: Posição mais próxima
        // 40.0f coloca logo a frente do Ninja (antes era 80.0f)
        SetPosition(pos + Vector2(dir * 40.0f, 0.0f));
        mScale.x = dir;

        mLifeTime = 0.33f; // Sincronizado com o tempo de ataque
    }

    void OnUpdate(float deltaTime) override {
        mLifeTime -= deltaTime;
        if (mLifeTime <= 0.0f) {
            SetState(ActorState::Destroy);
        }
    }
private:
    float mLifeTime;
};

Ninja::Ninja(Game *game, const float accelerationForce, const float jumpSpeed)
    : Actor(game)
      , mIsRunning(false)
      , mIsDead(false)
      , mIsAttacking(false)
      , mIsDashing(false)
      , mIsShooting(false)
      , mAttackTimer(0.0f)
      , mDashTimer(0.0f)
      , mShootTimer(0.0f)
      , mDartSpawned(false)
      , mDashSpeed(1200.0f)
      , mDashDirection(Vector2::Zero)
      , mIsInvincible(false)
      , mInvincibleTimer(0.0f)
      , mDashCooldown(0.0f)
      , mShootCooldown(0.0f)
      , mAccelerationForce(accelerationForce)
      , mMaxSpeed(400.0f)
      , mJumpSpeed(jumpSpeed)
      , mMaxHealth(3)
      , mHealth(3)
      , mActionState(ActionState::None)
{
    mDrawComponent = new AnimatorComponent(this, 64, 64);

    mDrawComponent->AddAnimation("idle", "../Assets/Sprites/Ninja/Idle.png", 6, 10.0f, true);
    mDrawComponent->AddAnimation("walk", "../Assets/Sprites/Ninja/Walk.png", 8, 10.0f, true);
    mDrawComponent->AddAnimation("run", "../Assets/Sprites/Ninja/Run.png", 6, 10.0f, true);
    mDrawComponent->AddAnimation("jump", "../Assets/Sprites/Ninja/Jump.png", 8, 10.0f, false);
    mDrawComponent->AddAnimation("dead", "../Assets/Sprites/Ninja/Dead.png", 4, 10.0f, false);
    mDrawComponent->AddAnimation("attack1", "../Assets/Sprites/Ninja/Attack_1.png", 6, 12.0f, false);
    mDrawComponent->AddAnimation("attack2", "../Assets/Sprites/Ninja/Attack_2.png", 4, 12.0f, false);
    mDrawComponent->AddAnimation("hurt", "../Assets/Sprites/Ninja/Hurt.png", 2, 10.0f, false);
    mDrawComponent->AddAnimation("shot", "../Assets/Sprites/Ninja/Shot.png", 6, 12.0f, false);
    
    mDrawComponent->AddAnimation("crouch_down", "../Assets/Sprites/Ninja/agacha.png", 5, 10.0f, false);
    mDrawComponent->AddAnimation("crouch_up", "../Assets/Sprites/Ninja/sobe.png", 5, 10.0f, false);    
    mDrawComponent->AddAnimation("shield_start", "../Assets/Sprites/Ninja/inicio_defesa.png", 2, 10.0f, false);    
    mDrawComponent->AddAnimation("shield_end", "../Assets/Sprites/Ninja/fim_defesa.png", 2, 10.0f, false);

    mDrawComponent->SetAnimation("idle");

    mShieldEffectActor = new Actor(game);
    
    // Configura a animação (ajuste 64, 64 se a imagem for maior, ex: 96, 96)
    mShieldEffectAnim = new AnimatorComponent(mShieldEffectActor, 64, 64);
    
    // Carrega o loop (8 frames, loop = true)
    mShieldEffectAnim->AddAnimation("shield_loop", "../Assets/Sprites/Ninja/8.png", 8, 15.0f, true);
    mShieldEffectAnim->SetAnimation("shield_loop");
    
    // Começa invisível
    mShieldEffectAnim->SetVisible(false);

    mRigidBodyComponent = new RigidBodyComponent(this, 1.0f, 6.0f, true);
    
    mColliderComponent = new AABBColliderComponent (
        this,
        0,
        0,
        20.0f,
        48.0f,
        ColliderLayer::Player
    );
    
    new DashAfterimageComponent(this, 95);
}

void Ninja::OnProcessInput(const uint8_t* state) {
    if (mIsDead || mIsAttacking || mIsDashing || mIsShooting) return;

    // --- LÓGICA DE DEFESA (ESCUDO - TECLA I) ---
    bool keyI = state[SDL_SCANCODE_I];
    
    // Só defende se estiver no chão
    if (keyI && mIsOnGround) {
        if (mActionState == ActionState::None) {
            mActionState = ActionState::ShieldStart;
            mDrawComponent->SetAnimation("shield_start"); 
            mRigidBodyComponent->SetVelocity(Vector2::Zero);
        }
    } else {
        if (mActionState == ActionState::ShieldHolding) {
            mActionState = ActionState::ShieldEnd;
            mDrawComponent->SetAnimation("shield_end");
        }
    }

    if (mActionState == ActionState::ShieldStart || 
        mActionState == ActionState::ShieldHolding || 
        mActionState == ActionState::ShieldEnd) {
        mRigidBodyComponent->SetVelocity(Vector2::Zero);
        return; 
    }

    // --- LÓGICA DE AGACHAR (TECLA S) ---
    bool keyS = state[SDL_SCANCODE_S];

    if (keyS && mIsOnGround) { // CORREÇÃO 1: Só agacha se estiver no chão
        if (mActionState == ActionState::None) {
            mActionState = ActionState::CrouchGoingDown;
            mDrawComponent->SetAnimation("crouch_down");
        }
    } else {
        // CORREÇÃO 2: Se soltou a tecla, sobe IMEDIATAMENTE.
        // Verifica se está Segurando (Holding) OU se ainda está Descendo (GoingDown)
        if (mActionState == ActionState::CrouchHolding || mActionState == ActionState::CrouchGoingDown) {
            mActionState = ActionState::CrouchGoingUp;
            mDrawComponent->SetAnimation("crouch_up");
        }
    }
    
    // CORREÇÃO EXTRA: Se cair do chão enquanto agacha, cancela o agachamento
    if (!mIsOnGround && (mActionState == ActionState::CrouchHolding || mActionState == ActionState::CrouchGoingDown)) {
        mActionState = ActionState::None;
    }

    // --- MOVIMENTAÇÃO (A / D) ---
    Vector2 force = Vector2::Zero;
    mIsRunning = false;

    if (mActionState == ActionState::None || mActionState == ActionState::CrouchHolding) {
        
        if(state[SDL_SCANCODE_D]){
            force.x += mAccelerationForce;
            mIsRunning = true;
        }
        if(state[SDL_SCANCODE_A]){
            force.x -= mAccelerationForce;
            mIsRunning = true;
        }

        if (mActionState == ActionState::CrouchHolding) {
            force.x *= 0.5f;
        }
    }

    if (mActionState == ActionState::None) {
        // PULO (Espaço)
        if(state[SDL_SCANCODE_SPACE] && mIsOnGround) {
            mGame->PlaySound(mGame->GetJumpSound());
            Vector2 vel = mRigidBodyComponent->GetVelocity();
            vel.y = mJumpSpeed;
            mRigidBodyComponent->SetVelocity(vel);
        }

        // ATAQUE (J)
        if(state[SDL_SCANCODE_J] && !mIsAttacking) {
            mIsAttacking = true;
            mAttackTimer = 0.33f;
            mRigidBodyComponent->SetVelocity(Vector2::Zero);
            mHitEnemiesThisAttack.clear();

            new EnergyLance(GetGame(), mPosition, mScale.x);
        }

        // DASH (K)
        if(state[SDL_SCANCODE_K] && !mIsDashing && mDashCooldown <= 0.0f) {
            mIsDashing = true;
            mDashTimer = 0.2f; 
            mDashCooldown = mDashCooldownTime;
            
            Vector2 dashVel(mDashSpeed * mScale.x, 0.0f);
            mRigidBodyComponent->SetVelocity(dashVel);

            mIsInvincible = true;
            mInvincibleTimer = 0.2f;
        }

        // TIRO (L)
        if(state[SDL_SCANCODE_L] && !mIsShooting && mShootCooldown <= 0.0f) {
            mIsShooting = true;
            mShootTimer = 0.5f;
            mShootCooldown = mShootCooldownTime;
            mDartSpawned = false;
            mRigidBodyComponent->SetVelocity(Vector2::Zero);
        }
    }

    if (!mIsDashing) {
        mRigidBodyComponent->ApplyForce(force);
    }
}

void Ninja::OnUpdate(float deltaTime) {
    if(mIsDead) return;

    if (mIsAttacking) {
        mAttackTimer -= deltaTime;
        if (mAttackTimer <= 0.0f) {
            mIsAttacking = false;
            mHitEnemiesThisAttack.clear();
        }
        mRigidBodyComponent->SetVelocity(Vector2::Zero);
        CheckAttackHit();
    }

    if (mIsDashing) {
        mDashTimer -= deltaTime;
        
        mRigidBodyComponent->SetVelocity(Vector2(mDashSpeed * mScale.x, 0.0f));

        DashParticle p;
        p.position = mPosition; 
        p.position.y += (rand() % 40) - 20; 
        float particleSpeed = -(mDashSpeed * 0.5f); 
        p.velocity = Vector2(particleSpeed * mScale.x, float((rand() % 100) - 50));
        p.lifetime = 0.3f;
        p.maxLifetime = 0.3f;
        p.size = 6.0f;
        mDashParticles.push_back(p);

        if (mDashTimer <= 0.0f) {
            mIsDashing = false;
            Vector2 exitVel = mRigidBodyComponent->GetVelocity();
            exitVel.x = (exitVel.x > 0 ? mMaxSpeed : -mMaxSpeed);
            mRigidBodyComponent->SetVelocity(exitVel);
        }
    }

    if (mIsShooting) {
        mShootTimer -= deltaTime;
        
        int currentFrame = mDrawComponent->GetCurrentFrame();
        if (!mDartSpawned && currentFrame == 3) {
            mDartSpawned = true;
            Vector2 direction(mScale.x, 0.0f);
            Vector2 spawnOffset(mScale.x * 40.0f, 30.0f);
            Dart* dart = new Dart(mGame, direction);
            dart->SetPosition(mPosition + spawnOffset);
        }
        
        if (mShootTimer <= 0.0f) {
            mIsShooting = false;
        }
        mRigidBodyComponent->SetVelocity(Vector2::Zero);
    }

    if (mActionState == ActionState::CrouchGoingDown) {
        if (mDrawComponent->GetCurrentFrame() >= 4) {
            mActionState = ActionState::CrouchHolding;
        }
    }
    else if (mActionState == ActionState::CrouchGoingUp) {
        if (mDrawComponent->GetCurrentFrame() >= 4) {
            mActionState = ActionState::None;
        }
    }
    else if (mActionState == ActionState::ShieldStart) {
        if (mDrawComponent->GetCurrentFrame() >= 1) {
            mActionState = ActionState::ShieldHolding;
        }
    }
    else if (mActionState == ActionState::ShieldEnd) {
        if (mDrawComponent->GetCurrentFrame() >= 1) {
            mActionState = ActionState::None;
        }
    }
    
    for(auto it = mDashParticles.begin(); it != mDashParticles.end();) {
        it->lifetime -= deltaTime;
        it->position += it->velocity * deltaTime;
        if(it->lifetime <= 0.0f) {
            it = mDashParticles.erase(it);
        } else {
            ++it;
        }
    }
    
    if (mDashCooldown > 0.0f) mDashCooldown -= deltaTime;
    if (mShootCooldown > 0.0f) mShootCooldown -= deltaTime;
    
    float levelEndX = (Game::LEVEL_WIDTH * Game::TILE_SIZE) - (Game::TILE_SIZE / 2.0f);
    if (mPosition.x >= levelEndX + Game::TILE_SIZE) {
        StageClear();
        return;
    }

    if (mPosition.y > Game::WINDOW_HEIGHT + 100.0f){
        Kill();
        return;
    }

    Vector2 velocity = mRigidBodyComponent->GetVelocity();

    if (!mIsDashing) { 
        if (velocity.x > mMaxSpeed) velocity.x = mMaxSpeed;
        if (velocity.x < -mMaxSpeed) velocity.x = -mMaxSpeed;
    }

    mRigidBodyComponent->SetVelocity(velocity);

    if (std::abs(velocity.y) > 20.0f) {
        mIsOnGround = false;
    }

    if (std::abs(velocity.x) > 1.0f) {
        mScale.x = (velocity.x > 0.0f) ? 1.0f : -1.0f;
    }

    if (mIsInvincible && !mIsDashing) {
        mInvincibleTimer -= deltaTime;
        bool flicker = fmodf(mInvincibleTimer * 4.0f, 1.0f) < 0.5f;
        mDrawComponent->SetVisible(flicker);

        if (mInvincibleTimer <= 0.0f) {
            mIsInvincible = false;
            mDrawComponent->SetVisible(true);
        }
    }

    const float cameraX = GetGame()->GetCameraPos().x;
    if (mPosition.x < cameraX + 25.0f) {
        mPosition.x = cameraX + 25.0f;
        Vector2 v = mRigidBodyComponent->GetVelocity();
        v.x = 0.0f;
        mRigidBodyComponent->SetVelocity(v);
    }

    bool showShieldEffect = (mActionState == ActionState::ShieldStart || 
                             mActionState == ActionState::ShieldHolding);

    mShieldEffectAnim->SetVisible(showShieldEffect);

    if (showShieldEffect) mShieldEffectActor->SetPosition(mPosition - Vector2(0.0f, 5.0f));
    

    ManageAnimations();
}

void Ninja::TakeDamage(const Vector2& damageSource) {
    if (mIsDead || mIsInvincible) return;

    mHealth--;
    mGame->StartHitStop(0.15f); 
    
    if (mHealth <= 0) {
        Kill();
    } else {
        mIsInvincible = true;
        mInvincibleTimer = 1.5f; 
        mGame->PlaySound(mGame->GetBumpSound());
        mDrawComponent->SetAnimation("hurt");

        float knockbackDir = (mPosition.x - damageSource.x) > 0.0f ? 1.0f : -1.0f;

        // Aplica força (X = Direção calculada, Y = Pulo para cima)
        Vector2 knockbackForce(knockbackDir * 800.0f, -400.0f);
        
        mRigidBodyComponent->SetVelocity(knockbackForce);
        
        mIsDashing = false;
    }
}

void Ninja::ManageAnimations() {
    if(mIsDead) return;

    // Prioridades
    if(mIsShooting) mDrawComponent->SetAnimation("shot");
    else if(mIsDashing) mDrawComponent->SetAnimation("run");
    else if(mIsAttacking) mDrawComponent->SetAnimation("attack2");
    
    // --- ESTADOS ESPECIAIS ---
    else if(mActionState == ActionState::ShieldStart)   mDrawComponent->SetAnimation("shield_start");
    else if(mActionState == ActionState::ShieldHolding) mDrawComponent->SetAnimation("shield_start"); // Mantém congelado no fim
    else if(mActionState == ActionState::ShieldEnd)     mDrawComponent->SetAnimation("shield_end");
    
    else if(mActionState == ActionState::CrouchGoingDown) mDrawComponent->SetAnimation("crouch_down");
    else if(mActionState == ActionState::CrouchHolding)   mDrawComponent->SetAnimation("crouch_down"); // Mantém congelado no fim
    else if(mActionState == ActionState::CrouchGoingUp)   mDrawComponent->SetAnimation("crouch_up");
    
    // --- ESTADOS PADRÃO ---
    else if(!mIsOnGround) mDrawComponent->SetAnimation("jump");
    else if(mIsRunning && std::abs(mRigidBodyComponent->GetVelocity().x) > 10.0f) mDrawComponent->SetAnimation("run");
    else mDrawComponent->SetAnimation("idle");
}

void Ninja::Kill() {
    if(mIsDead) return;

    if(!mIsInvincible){
        mIsDead = true;
        mDrawComponent->SetAnimation("dead");

        mRigidBodyComponent->SetEnabled(false);
        mColliderComponent->SetEnabled(false);
        
        // --- DESTRÓI O EFEITO JUNTO ---
        if (mShieldEffectActor) {
            mShieldEffectActor->SetState(ActorState::Destroy);
        }

        Mix_HaltMusic();
        int channel = mGame->PlaySound(mGame->GetDeadSound());
        mGame->SetDeathSoundChannel(channel);
        mGame->SetScene(GameScene::GameOver);
    }
}

void Ninja::OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other) {
    if(mIsDead) return;

    // Colisão com Inimigos
    if(other->GetLayer() == ColliderLayer::Enemy) {
        if(mIsAttacking || mIsDashing) {
            // Nota: O ideal seria chamar ApplyDamage aqui ao invés de Kill direto
            other->GetOwner()->Kill();
        } 
    } 
    // Colisão com Espinhos (Mantida)
    else if (other->GetLayer() == ColliderLayer::Blocks) {
        Block* block = dynamic_cast<Block*>(other->GetOwner());
        if (block && block->GetType() == EBlockType::Thorns) {
            if (!mIsInvincible) {
                 TakeDamage(block->GetPosition());
            }
        }
    }
}

void Ninja::OnVerticalCollision(const float minOverlap, AABBColliderComponent* other) {
    if(mIsDead) return;

    if(other->GetLayer() == ColliderLayer::Blocks){
        
        Block* block = dynamic_cast<Block*>(other->GetOwner());
        if (block && block->GetType() == EBlockType::Thorns) {
            if (!mIsInvincible) TakeDamage(block->GetPosition());
            return; 
        }

        // Lógica de pisar no chão
        if (mRigidBodyComponent->GetVelocity().y < 0.0f) {
            Vector2 vel = mRigidBodyComponent->GetVelocity();
            vel.y = 0.0f;
            mRigidBodyComponent->SetVelocity(vel);
        }
    }
}

void Ninja::CreateDashParticles(const Vector2& startPos, const Vector2& endPos) {
    int numParticles = 20;
    for(int i = 0; i < numParticles; i++) {
        DashParticle particle;
        float t = static_cast<float>(i) / numParticles;
        particle.position = startPos + (endPos - startPos) * t;
        
        float angle = (rand() % 360) * Math::Pi / 180.0f;
        float speed = 50.0f + (rand() % 100);
        particle.velocity = Vector2(Math::Cos(angle) * speed, Math::Sin(angle) * speed);
        
        particle.maxLifetime = 0.3f + (rand() % 100) / 500.0f;
        particle.lifetime = particle.maxLifetime;
        particle.size = 3.0f + (rand() % 5);
        
        mDashParticles.push_back(particle);
    }
}

void Ninja::StageClear() {
    mIsDead = true;
    mRigidBodyComponent->SetEnabled(false);
    mColliderComponent->SetEnabled(false);
    mRigidBodyComponent->SetVelocity(Vector2::Zero);

    Mix_HaltMusic();
    int channel = mGame->PlaySound(mGame->GetStageClearSound());
    mGame->SetStageClearSoundChannel(channel);
    mGame->SetWaitingToQuit(true);
}

void Ninja::CheckAttackHit()
{
    // --- HITBOX AJUSTADA ---
    // halfW = 45.0f (Total 90 de largura) -> Maior que a espada normal (60), mas menor que antes (140)
    const float halfW = 45.0f;
    const float halfH = 30.0f; 

    // Centro ajustado para frente (50.0f) para alinhar com a nova animação
    Vector2 center = mPosition + Vector2(mScale.x * 50.0f, -5.0f);

    Vector2 atkMin(center.x - halfW, center.y - halfH);
    Vector2 atkMax(center.x + halfW, center.y + halfH);

    for (auto col : mGame->GetColliders()) {
        if (!col->IsEnabled() || col->GetLayer() != ColliderLayer::Enemy) continue;
        auto owner = col->GetOwner();
        if (!owner || owner->GetState() != ActorState::Active) continue;

        Vector2 cMin = col->GetMin();
        Vector2 cMax = col->GetMax();

        bool overlap = (atkMin.x <= cMax.x && atkMax.x >= cMin.x &&
                        atkMin.y <= cMax.y && atkMax.y >= cMin.y);
        if (overlap) {
            if (mHitEnemiesThisAttack.find(owner) != mHitEnemiesThisAttack.end()) continue;

            if (auto warrior = dynamic_cast<SkeletonWarrior*>(owner)) {
                warrior->ApplyDamage(1);
            } else if (auto spear = dynamic_cast<SkeletonSpearman*>(owner)) {
                spear->ApplyDamage(1);
            } else if (auto archer = dynamic_cast<SkeletonArcher*>(owner)) {
                archer->ApplyDamage(1);
            } else if (auto boss = dynamic_cast<KarasuTengu*>(owner)) {
                boss->ApplyDamage(1);
            } else {
                owner->Kill();
            }
            mHitEnemiesThisAttack.insert(owner);
        }
    }
}
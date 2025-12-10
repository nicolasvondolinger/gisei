#pragma once
#include <string>

class BossEnemy {
public:
    virtual ~BossEnemy() = default;
    virtual int GetHealth() const = 0;
    virtual int GetMaxHealth() const = 0;
    virtual bool IsDying() const = 0;
    virtual const std::string& GetName() const = 0;
};

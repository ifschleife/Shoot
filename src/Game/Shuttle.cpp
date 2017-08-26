
#include "Shuttle.h"
#include "Physics/IBody.h"
#include "Physics/IShape.h"
#include "Physics/CMFactory.h"
#include "Random.h"

#include "Entity/EntityBase.h"
#include "Rendering/IRenderer.h"
#include "Rendering/Sprite/ISprite.h"
#include "Rendering/Sprite/SpriteFactory.h"
#include "Rendering/Color.h"

#include "Math/MathFunctions.h"
#include "Math/Matrix.h"

#include "AIKnowledge.h"
#include "EntityProperties.h"
#include "CollisionConfiguration.h"

#include "Factories.h"
#include "Weapons/IWeaponSystem.h"
#include "Weapons/IWeaponFactory.h"

#include "Effects/TrailEffect.h"

#include <cmath>


using namespace game;

namespace constants
{    
    enum
    {
        IDLE = 0,
        THRUSTING
    };
}

class SpriteEntity : public mono::EntityBase
{
public:

    SpriteEntity(const char* sprite_file)
    {
        m_sprite = mono::CreateSprite(sprite_file);
    }

    virtual void Draw(mono::IRenderer& renderer) const
    {
        if(m_enabled)
            renderer.DrawSprite(*m_sprite);
    }

    virtual void Update(unsigned int delta)
    {
        m_sprite->doUpdate(delta);
    }

    void SetAnimation(int animation_id)
    {
        m_sprite->SetAnimation(animation_id);
    }

    mono::ISpritePtr m_sprite;
    bool m_enabled = false;
};


Shuttle::Shuttle(const math::Vector& position, mono::EventHandler& eventHandler, const System::ControllerState& controller)
    : m_controller(this, eventHandler, controller),
      m_fire(false),
      m_player_info(nullptr)
{
    m_position = position;
    m_scale = math::Vector(1.0f, 1.0f);
    
    m_physics.body = mono::PhysicsFactory::CreateBody(10.0f, INFINITY);
    m_physics.body->SetPosition(m_position);
    m_physics.body->SetCollisionHandler(this);

    mono::IShapePtr shape = mono::PhysicsFactory::CreateShape(m_physics.body, m_scale.x / 2.0f, math::zeroVec);
    shape->SetElasticity(0.1f);
    shape->SetCollisionFilter(CollisionCategory::PLAYER, PLAYER_MASK);
    
    m_physics.shapes.push_back(shape);

    m_sprite = mono::CreateSprite("res/sprites/shuttle.sprite");
    m_sprite->SetAnimation(constants::IDLE);

    m_left_booster = std::make_shared<SpriteEntity>("res/sprites/booster.sprite");
    m_left_booster->SetScale(math::Vector(0.5f, 0.5f));
    m_left_booster->SetRotation(-math::PI_2());
    m_left_booster->SetPosition(math::Vector(-0.6f, 0.0f));

    m_right_booster = std::make_shared<SpriteEntity>("res/sprites/booster.sprite");
    m_right_booster->SetScale(math::Vector(0.5f, 0.5f));
    m_right_booster->SetRotation(math::PI_2());
    m_right_booster->SetPosition(math::Vector(0.6f, 0.0f));
    
    AddChild(std::make_shared<game::TrailEffect>(m_position));
    AddChild(m_left_booster);
    AddChild(m_right_booster);

    SetProperty(EntityProperties::DAMAGABLE);
    
    // Make sure we have a weapon
    SelectWeapon(WeaponType::STANDARD);
}

void Shuttle::Draw(mono::IRenderer& renderer) const
{
    renderer.DrawSprite(*m_sprite);
}

void Shuttle::Update(unsigned int delta)
{
    m_controller.Update(delta);
    m_sprite->doUpdate(delta);

    if(m_fire)
    {
        const math::Vector& position_shift = math::VectorFromAngle(m_rotation) * 0.5f;
        const float direction_shift = math::ToRadians(mono::Random(-4.0f, 4.0f));

        m_weapon->Fire(m_position + position_shift, m_rotation + direction_shift);
    }

    if(m_player_info)
    {
        m_player_info->position = m_position;
        m_player_info->ammunition = m_weapon->AmmunitionLeft();
    }
}

void Shuttle::OnCollideWith(const mono::IBodyPtr& body)
{ }

void Shuttle::OnPostStep()
{ }

void Shuttle::SelectWeapon(WeaponType weapon)
{
    m_weapon = weapon_factory->CreateWeapon(weapon, WeaponFaction::PLAYER);
}

void Shuttle::ApplyRotationForce(float force)
{
    const math::Vector forceVector(force, 0.0);

    // First apply the rotational force at an offset of 20 in y axis, then negate the vector
    // and apply it to zero to counter the movement when we only want rotation.
    m_physics.body->ApplyForce(forceVector, math::Vector(0, 20));
    m_physics.body->ApplyForce(forceVector * -1, math::zeroVec);
}

void Shuttle::ApplyThrustForce(float force)
{
    const float rotation = Rotation();
    const math::Vector unit(-std::sin(rotation), std::cos(rotation));

    m_physics.body->ApplyForce(unit * force, math::zeroVec);
}

void Shuttle::ApplyImpulse(const math::Vector& force)
{
    m_physics.body->ApplyImpulse(force, m_position);
}

void Shuttle::Fire()
{
    m_fire = true;
}

void Shuttle::StopFire()
{
    m_fire = false;
}

void Shuttle::SetBoosterThrusting(BoosterPosition position, bool enable)
{
    const int state = enable ? constants::THRUSTING : constants::IDLE;

    switch(position)
    {
        case BoosterPosition::LEFT:
            m_left_booster->SetAnimation(state);
            m_left_booster->m_enabled = enable;
            break;
        case BoosterPosition::RIGHT:
            m_right_booster->SetAnimation(state);
            m_right_booster->m_enabled = enable;
            break;
        case BoosterPosition::MAIN:
            m_sprite->SetAnimation(state);
            break;
        case BoosterPosition::ALL:
            m_left_booster->SetAnimation(state);
            m_left_booster->m_enabled = enable;
            m_right_booster->SetAnimation(state);
            m_right_booster->m_enabled = enable;
            m_sprite->SetAnimation(state);
            break;
    }
}

void Shuttle::SetShading(const mono::Color::RGBA& shade)
{
    m_sprite->SetShade(shade);
}

void Shuttle::SetPlayerInfo(PlayerInfo* info)
{
    m_player_info = info;
}

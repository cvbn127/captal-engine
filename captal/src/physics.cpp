#include "physics.hpp"

#include <stdexcept>

//Chipmunk2D must have been compiled with the same definitions:
#define CP_COLLISION_TYPE_TYPE uint64_t
#define CP_GROUP_TYPE uint64_t
#define CP_BITMASK_TYPE uint64_t
#define CP_TIMESTAMP_TYPE uint64_t

#include <chipmunk/chipmunk.h>

namespace cpt
{

static inline cpVect tocp(const glm::vec2& vec) noexcept
{
    return cpVect{static_cast<cpFloat>(vec.x), static_cast<cpFloat>(vec.y)};
}

static inline cpFloat tocp(float value) noexcept
{
    return static_cast<cpFloat>(value);
}

static inline glm::vec2 fromcp(const cpVect& vec) noexcept
{
    return glm::vec2{static_cast<float>(vec.x), static_cast<float>(vec.y)};
}

static inline float fromcp(cpFloat value) noexcept
{
    return static_cast<float>(value);
}

void physical_collision_arbiter::set_restitution(float restitution) noexcept
{
    cpArbiterSetRestitution(m_arbiter, tocp(restitution));
}

void physical_collision_arbiter::set_friction(float friction) noexcept
{
    cpArbiterSetFriction(m_arbiter, tocp(friction));
}

void physical_collision_arbiter::set_surface_velocity(const glm::vec2& surface_velocity) noexcept
{
    cpArbiterSetSurfaceVelocity(m_arbiter, tocp(surface_velocity));
}

void physical_collision_arbiter::set_user_data(void* user_data) noexcept
{
    cpArbiterSetUserData(m_arbiter, user_data);
}

std::pair<physical_shape&, physical_shape&> physical_collision_arbiter::shapes() const noexcept
{
    cpShape* first{};
    cpShape* second{};
    cpArbiterGetShapes(m_arbiter, &first, &second);

    return {*reinterpret_cast<physical_shape*>(cpShapeGetUserData(first)), *reinterpret_cast<physical_shape*>(cpShapeGetUserData(second))};
}

std::pair<physical_body&, physical_body&> physical_collision_arbiter::bodies() const noexcept
{
    cpBody* first{};
    cpBody* second{};
    cpArbiterGetBodies(m_arbiter, &first, &second);

    return {*reinterpret_cast<physical_body*>(cpBodyGetUserData(first)), *reinterpret_cast<physical_body*>(cpBodyGetUserData(second))};
}

std::size_t physical_collision_arbiter::contact_count() const noexcept
{
    return static_cast<std::size_t>(cpArbiterGetCount(m_arbiter));
}

glm::vec2 physical_collision_arbiter::normal() const noexcept
{
    return fromcp(cpArbiterGetNormal(m_arbiter));
}

std::pair<glm::vec2, glm::vec2> physical_collision_arbiter::points(std::size_t contact_index) const noexcept
{
    return std::make_pair(fromcp(cpArbiterGetPointA(m_arbiter, static_cast<int>(contact_index))), fromcp(cpArbiterGetPointB(m_arbiter, static_cast<int>(contact_index))));
}

float physical_collision_arbiter::depth(std::size_t contact_index) const noexcept
{
    return fromcp(cpArbiterGetDepth(m_arbiter, static_cast<int>(contact_index)));
}

bool physical_collision_arbiter::is_first_contact() const noexcept
{
    return static_cast<bool>(cpArbiterIsFirstContact(m_arbiter));
}

bool physical_collision_arbiter::is_removal() const noexcept
{
    return static_cast<bool>(cpArbiterIsRemoval(m_arbiter));
}

physical_world::physical_world()
:m_world{cpSpaceNew()}
{
    if(!m_world)
        throw std::runtime_error{"Can not allocate physical world."};

    cpSpaceSetUserData(m_world, this);
}

physical_world::~physical_world()
{
    cpSpaceFree(m_world);
}

void physical_world::add_collision(collision_type_t first_type, collision_type_t second_type, collision_handler handler)
{
    add_callback(cpSpaceAddCollisionHandler(m_world, first_type, second_type), std::move(handler));
}

void physical_world::add_wildcard(collision_type_t type, collision_handler handler)
{
    add_callback(cpSpaceAddWildcardHandler(m_world, type), std::move(handler));
}

void physical_world::point_query(const glm::vec2& point, float max_distance, group_t group, collision_id_t id, collision_id_t mask, point_query_callback_type callback)
{
    const cpShapeFilter filter{cpShapeFilterNew(group, id, mask)};

    const auto native_callback = [](cpShape* native_shape, cpVect point, cpFloat distance, cpVect gradient, void *data)
    {
        const point_query_callback_type& callback{*reinterpret_cast<const point_query_callback_type*>(data)};
        physical_shape& shape{*reinterpret_cast<physical_shape*>(cpShapeGetUserData(native_shape))};
        const point_hit info{shape, fromcp(point), fromcp(distance), fromcp(gradient)};

        callback(info);
    };

    cpSpacePointQuery(m_world, tocp(point), tocp(max_distance), filter, native_callback, &callback);
}

void physical_world::region_query(float x, float y, float width, float height, group_t group, collision_id_t id, collision_id_t mask, region_query_callback_type callback)
{
    const cpShapeFilter filter{cpShapeFilterNew(group, id, mask)};

    const auto native_callback = [](cpShape* native_shape, void* data)
    {
        const region_query_callback_type& callback{*reinterpret_cast<const region_query_callback_type*>(data)};
        physical_shape& shape{*reinterpret_cast<physical_shape*>(cpShapeGetUserData(native_shape))};
        const region_hit info{shape};

        callback(info);
    };

    cpSpaceBBQuery(m_world, cpBBNew(tocp(x), tocp(y), tocp(x + width), tocp(y + height)), filter, native_callback, &callback);
}

void physical_world::ray_query(const glm::vec2& from, const glm::vec2& to, float thickness, group_t group, collision_id_t id, collision_id_t mask, ray_callback_type callback)
{
    const cpShapeFilter filter{cpShapeFilterNew(group, id, mask)};

    const auto native_callback = [](cpShape* native_shape, cpVect point, cpVect normal, cpFloat alpha, void* data)
    {
        const ray_callback_type& callback{*reinterpret_cast<const ray_callback_type*>(data)};
        physical_shape& shape{*reinterpret_cast<physical_shape*>(cpShapeGetUserData(native_shape))};
        const ray_hit info{shape, fromcp(point), fromcp(normal), fromcp(alpha)};

        callback(info);
    };

    cpSpaceSegmentQuery(m_world, tocp(from), tocp(to), tocp(thickness), filter, native_callback, &callback);
}


std::vector<physical_world::point_hit> physical_world::point_query(const glm::vec2& point, float max_distance, group_t group, collision_id_t id, collision_id_t mask)
{
    std::vector<point_hit> output{};

    point_query(point, max_distance, group, id, mask, [&output](point_hit hit)
    {
        output.emplace_back(hit);
    });

    return output;
}

std::vector<physical_world::region_hit> physical_world::region_query(float x, float y, float width, float height, group_t group, collision_id_t id, collision_id_t mask)
{
    std::vector<region_hit> output{};

    region_query(x, y, width, height, group, id, mask, [&output](region_hit hit)
    {
        output.emplace_back(hit);
    });

    return output;
}

std::vector<physical_world::ray_hit> physical_world::ray_query(const glm::vec2& from, const glm::vec2& to, float thickness, group_t group, collision_id_t id, collision_id_t mask)
{
    std::vector<ray_hit> output{};

    ray_query(from, to, thickness, group, id, mask, [&output](ray_hit hit)
    {
        output.emplace_back(hit);
    });

    return output;
}

std::optional<physical_world::point_hit> physical_world::point_query_nearest(const glm::vec2& point, float max_distance, group_t group, collision_id_t id, collision_id_t mask)
{
    const cpShapeFilter filter{cpShapeFilterNew(group, id, mask)};

    cpPointQueryInfo info{};
    if(cpSpacePointQueryNearest(m_world, tocp(point), tocp(max_distance), filter, &info))
    {
        physical_shape& shape{*reinterpret_cast<physical_shape*>(cpShapeGetUserData(info.shape))};
        return point_hit{shape, fromcp(info.point), fromcp(info.distance), fromcp(info.gradient)};
    }

    return std::nullopt;
}

std::optional<physical_world::ray_hit> physical_world::ray_query_first(const glm::vec2& from, const glm::vec2& to, float thickness, group_t group, collision_id_t id, collision_id_t mask)
{
    const cpShapeFilter filter{cpShapeFilterNew(group, id, mask)};

    cpSegmentQueryInfo info{};
    if(cpSpaceSegmentQueryFirst(m_world, tocp(from), tocp(to), tocp(thickness), filter, &info))
    {
        physical_shape& shape{*reinterpret_cast<physical_shape*>(cpShapeGetUserData(info.shape))};
        return ray_hit{shape, fromcp(info.point), fromcp(info.normal), fromcp(info.alpha)};
    }

    return std::nullopt;
}

void physical_world::update(float time)
{
    m_time += time;
    const std::uint32_t steps{std::min(static_cast<std::uint32_t>(m_time / m_step), m_max_steps)};

    for(std::uint32_t i{}; i < steps; ++i)
    {
        cpSpaceStep(m_world, tocp(m_step));
        m_time -= m_step;
    }
}

void physical_world::set_gravity(const glm::vec2& gravity) noexcept
{
    cpSpaceSetGravity(m_world, tocp(gravity));
}

void physical_world::set_damping(float damping) noexcept
{
    cpSpaceSetDamping(m_world, tocp(damping));
}

void physical_world::set_idle_threshold(float idle_threshold) noexcept
{
    cpSpaceSetIdleSpeedThreshold(m_world, tocp(idle_threshold));
}

void physical_world::set_sleep_threshold(float sleep_threshold) noexcept
{
    cpSpaceSetSleepTimeThreshold(m_world, tocp(sleep_threshold));
}

void physical_world::set_collision_slop(float collision_slop) noexcept
{
    cpSpaceSetCollisionSlop(m_world, tocp(collision_slop));
}

void physical_world::set_collision_bias(float collision_bias) noexcept
{
    cpSpaceSetCollisionBias(m_world, tocp(collision_bias));
}

void physical_world::set_collision_persistence(uint64_t collision_persistance) noexcept
{
    cpSpaceSetCollisionPersistence(m_world, collision_persistance);
}

void physical_world::set_iteration_count(std::uint32_t count) noexcept
{
    cpSpaceSetIterations(m_world, static_cast<int>(count));
}

glm::vec2 physical_world::gravity() const noexcept
{
    return fromcp(cpSpaceGetGravity(m_world));
}

float physical_world::damping() const noexcept
{
    return fromcp(cpSpaceGetDamping(m_world));
}

float physical_world::idle_threshold() const noexcept
{
    return fromcp(cpSpaceGetIdleSpeedThreshold(m_world));
}

float physical_world::sleep_threshold() const noexcept
{
    return fromcp(cpSpaceGetSleepTimeThreshold(m_world));
}

float physical_world::collision_slop() const noexcept
{
    return fromcp(cpSpaceGetCollisionSlop(m_world));
}

float physical_world::collision_bias() const noexcept
{
    return fromcp(cpSpaceGetCollisionBias(m_world));
}

std::uint64_t physical_world::collision_persistence() const noexcept
{
    return cpSpaceGetCollisionPersistence(m_world);
}

void physical_world::add_callback(cpCollisionHandler *cphandler, collision_handler handler)
{
    auto it{m_callbacks.find(cphandler)};
    if(it == std::end(m_callbacks))
    {
        it = m_callbacks.emplace(std::make_pair(cphandler, std::make_unique<collision_handler>(std::move(handler)))).first;
    }
    else
    {
        it->second = std::make_unique<collision_handler>(std::move(handler));
    }

    collision_handler& new_handler{*it->second};
    cphandler->userData = &new_handler;

    if(new_handler.collision_begin)
    {
        cphandler->beginFunc = [](cpArbiter* cparbiter, cpSpace* cpspace, void* userdata) -> cpBool
        {
            cpBody* cpfirst{};
            cpBody* cpsecond{};
            cpArbiterGetBodies(cparbiter, &cpfirst, &cpsecond);

            physical_world& world{*reinterpret_cast<physical_world*>(cpSpaceGetUserData(cpspace))};
            physical_body& first{*reinterpret_cast<physical_body*>(cpBodyGetUserData(cpfirst))};
            physical_body& second{*reinterpret_cast<physical_body*>(cpBodyGetUserData(cpsecond))};
            physical_collision_arbiter arbiter{cparbiter};
            collision_handler& handler{*reinterpret_cast<collision_handler*>(userdata)};

            return static_cast<cpBool>(handler.collision_begin(world, first, second, std::move(arbiter), handler.userdata));
        };
    }
    else
    {
        cphandler->beginFunc = [](cpArbiter*, cpSpace*, void*) -> cpBool
        {
            return cpTrue;
        };
    }

    if(new_handler.collision_pre_solve)
    {
        cphandler->preSolveFunc = [](cpArbiter* cparbiter, cpSpace* cpspace, void* userdata) -> cpBool
        {
            cpBody* cpfirst{};
            cpBody* cpsecond{};
            cpArbiterGetBodies(cparbiter, &cpfirst, &cpsecond);

            physical_world& world{*reinterpret_cast<physical_world*>(cpSpaceGetUserData(cpspace))};
            physical_body& first{*reinterpret_cast<physical_body*>(cpBodyGetUserData(cpfirst))};
            physical_body& second{*reinterpret_cast<physical_body*>(cpBodyGetUserData(cpsecond))};
            physical_collision_arbiter arbiter{cparbiter};
            collision_handler& handler{*reinterpret_cast<collision_handler*>(userdata)};

            return static_cast<cpBool>(handler.collision_pre_solve(world, first, second, std::move(arbiter), handler.userdata));
        };
    }
    else
    {
        cphandler->preSolveFunc = [](cpArbiter*, cpSpace*, void*) -> cpBool
        {
            return cpTrue;
        };
    }

    if(new_handler.collision_post_solve)
    {
        cphandler->postSolveFunc = [](cpArbiter* cparbiter, cpSpace* cpspace, void* userdata) -> void
        {
            cpBody* cpfirst{};
            cpBody* cpsecond{};
            cpArbiterGetBodies(cparbiter, &cpfirst, &cpsecond);

            physical_world& world{*reinterpret_cast<physical_world*>(cpSpaceGetUserData(cpspace))};
            physical_body& first{*reinterpret_cast<physical_body*>(cpBodyGetUserData(cpfirst))};
            physical_body& second{*reinterpret_cast<physical_body*>(cpBodyGetUserData(cpsecond))};
            physical_collision_arbiter arbiter{cparbiter};
            collision_handler& handler{*reinterpret_cast<collision_handler*>(userdata)};

            handler.collision_post_solve(world, first, second, std::move(arbiter), handler.userdata);
        };
    }
    else
    {
        cphandler->postSolveFunc = [](cpArbiter*, cpSpace*, void*) -> void
        {

        };
    }

    if(new_handler.collision_end)
    {
        cphandler->separateFunc = [](cpArbiter* cparbiter, cpSpace* cpspace, void* userdata) -> void
        {
            cpBody* cpfirst{};
            cpBody* cpsecond{};
            cpArbiterGetBodies(cparbiter, &cpfirst, &cpsecond);

            physical_world& world{*reinterpret_cast<physical_world*>(cpSpaceGetUserData(cpspace))};
            physical_body& first{*reinterpret_cast<physical_body*>(cpBodyGetUserData(cpfirst))};
            physical_body& second{*reinterpret_cast<physical_body*>(cpBodyGetUserData(cpsecond))};
            physical_collision_arbiter arbiter{cparbiter};
            collision_handler& handler{*reinterpret_cast<collision_handler*>(userdata)};

            handler.collision_end(world, first, second, std::move(arbiter), handler.userdata);
        };
    }
    else
    {
        cphandler->separateFunc = [](cpArbiter*, cpSpace*, void*) -> void
        {

        };
    }
}

physical_shape::physical_shape(physical_body_ptr body, float radius, const glm::vec2& offset)
:m_body{std::move(body)}
,m_shape{cpCircleShapeNew(m_body->handle(), tocp(radius), tocp(offset))}
{
    if(!m_shape)
        throw std::runtime_error{"Can not allocate physical shape."};

    cpShapeSetUserData(m_shape, this);
    cpSpaceAddShape(m_body->world()->handle(), m_shape);
    m_body->register_shape(this);
}

physical_shape::physical_shape(physical_body_ptr body, const glm::vec2& first, const glm::vec2& second, float thickness)
:m_body{std::move(body)}
,m_shape{cpSegmentShapeNew(m_body->handle(), tocp(first), tocp(second), tocp(thickness))}
{
    if(!m_shape)
        throw std::runtime_error{"Can not allocate physical shape."};

    cpShapeSetUserData(m_shape, this);
    cpSpaceAddShape(m_body->world()->handle(), m_shape);
    m_body->register_shape(this);
}

physical_shape::physical_shape(physical_body_ptr body, const std::vector<glm::vec2>& points, float radius)
:m_body{std::move(body)}
{
    std::vector<cpVect> native_vertices{};
    native_vertices.reserve(std::size(points));
    for(auto&& vertex : points)
    {
        native_vertices.emplace_back(tocp(vertex));
    }

    m_shape = cpPolyShapeNew(m_body->handle(), static_cast<int>(std::size(points)), std::data(native_vertices), cpTransformIdentity, tocp(radius));
    if(!m_shape)
        throw std::runtime_error{"Can not allocate physical shape."};

    cpShapeSetUserData(m_shape, this);
    cpSpaceAddShape(m_body->world()->handle(), m_shape);
    m_body->register_shape(this);
}

physical_shape::physical_shape(physical_body_ptr body, float width, float height, float radius)
:m_body{std::move(body)}
,m_shape{cpBoxShapeNew(m_body->handle(), tocp(width), tocp(height), tocp(radius))}
{
    if(!m_shape)
        throw std::runtime_error{"Can not allocate physical shape."};

    cpSpaceAddShape(m_body->world()->handle(), m_shape);

    cpShapeSetUserData(m_shape, this);
    m_body->register_shape(this);
}

physical_shape::~physical_shape()
{
    m_body->unregister_shape(this);
    cpSpaceRemoveShape(m_body->world()->handle(), m_shape);
    cpShapeFree(m_shape);
}

void physical_shape::set_sensor(bool enable) noexcept
{
    cpShapeSetSensor(m_shape, static_cast<cpBool>(enable));
}

void physical_shape::set_elasticity(float elasticity) noexcept
{
    cpShapeSetElasticity(m_shape, tocp(elasticity));
}

void physical_shape::set_friction(float friction) noexcept
{
    cpShapeSetFriction(m_shape, tocp(friction));
}

void physical_shape::set_surface_velocity(const glm::vec2& surface_velocity) noexcept
{
    cpShapeSetSurfaceVelocity(m_shape, tocp(surface_velocity));
}

void physical_shape::set_collision_type(collision_type_t type) noexcept
{
    cpShapeSetCollisionType(m_shape, type);
}

void physical_shape::set_filter(group_t group, collision_id_t categories, collision_id_t collision_mask) noexcept
{
    cpShapeSetFilter(m_shape, cpShapeFilterNew(group, categories, collision_mask));
}

bool physical_shape::is_sensor() const noexcept
{
    return static_cast<bool>(cpShapeGetSensor(m_shape));
}

float physical_shape::elasticity() const noexcept
{
    return fromcp(cpShapeGetElasticity(m_shape));
}

float physical_shape::friction() const noexcept
{
    return fromcp(cpShapeGetFriction(m_shape));
}

glm::vec2 physical_shape::surface_velocity() const noexcept
{
    return fromcp(cpShapeGetSurfaceVelocity(m_shape));
}

collision_type_t physical_shape::collision_type() const noexcept
{
    return cpShapeGetCollisionType(m_shape);
}

group_t physical_shape::group() const noexcept
{
    return cpShapeGetFilter(m_shape).group;
}

collision_id_t physical_shape::categories() const noexcept
{
    return cpShapeGetFilter(m_shape).categories;
}

collision_id_t physical_shape::collision_mask() const noexcept
{
    return cpShapeGetFilter(m_shape).mask;
}

float circle_moment(float mass, float radius, const glm::vec2& origin, float inner_radius) noexcept
{
    return fromcp(cpMomentForCircle(tocp(mass), tocp(radius), tocp(inner_radius), tocp(origin)));
}

float segment_moment(float mass, const glm::vec2& first, const glm::vec2& second, float thickness) noexcept
{
    return fromcp(cpMomentForSegment(tocp(mass), tocp(first), tocp(second), tocp(thickness)));
}

float polygon_moment(float mass, const std::vector<glm::vec2>& points, const glm::vec2& offset, float radius) noexcept
{
    std::vector<cpVect> native_vertices{};
    native_vertices.reserve(std::size(points));
    for(auto&& vertex : points)
    {
        native_vertices.emplace_back(tocp(vertex));
    }

    return fromcp(cpMomentForPoly(tocp(mass), static_cast<int>(std::size(points)), std::data(native_vertices), tocp(offset), tocp(radius)));
}

float square_moment(float mass, float width, float height) noexcept
{
    return fromcp(cpMomentForBox(tocp(mass), tocp(width), tocp(height)));
}

physical_body::physical_body(physical_world_ptr world, physical_body_type type, float mass, float moment)
:m_world{std::move(world)}
{
    if(type == physical_body_type::dynamic)
    {
        m_body = cpBodyNew(tocp(mass), tocp(moment));
    }
    else if(type == physical_body_type::steady)
    {
        m_body = cpBodyNewStatic();
    }
    else if(type == physical_body_type::kinematic)
    {
        m_body = cpBodyNewKinematic();
    }

    if(!m_body)
        throw std::runtime_error{"Can not allocate physical body."};

    cpBodySetUserData(m_body, this);
    cpSpaceAddBody(m_world->handle(), m_body);
}

physical_body::~physical_body()
{
    cpSpaceRemoveBody(m_world->handle(), m_body);
    cpBodyFree(m_body);
}

void physical_body::apply_force(const glm::vec2& force, const glm::vec2& point) noexcept
{
    cpBodyApplyForceAtWorldPoint(m_body, tocp(force), tocp(point));
}

void physical_body::apply_local_force(const glm::vec2& force, const glm::vec2& point) noexcept
{
    cpBodyApplyForceAtLocalPoint(m_body, tocp(force), tocp(point));
}

void physical_body::apply_impulse(const glm::vec2& impulse, const glm::vec2& point) noexcept
{
    cpBodyApplyImpulseAtWorldPoint(m_body, tocp(impulse), tocp(point));
}

void physical_body::apply_local_impulse(const glm::vec2& impulse, const glm::vec2& point) noexcept
{
    cpBodyApplyImpulseAtLocalPoint(m_body, tocp(impulse), tocp(point));
}

void physical_body::add_torque(float torque) noexcept
{
    cpBodySetTorque(m_body, cpBodyGetTorque(m_body) + tocp(torque));
}

void physical_body::set_angular_velocity(float velocity) noexcept
{
    cpBodySetAngularVelocity(m_body, tocp(velocity));
}

void physical_body::set_mass(float mass) noexcept
{
    cpBodySetMass(m_body, tocp(mass));
}

void physical_body::set_mass_center(const glm::vec2& center) noexcept
{
    cpBodySetCenterOfGravity(m_body, tocp(center));
}

void physical_body::set_moment_of_inertia(float moment) noexcept
{
    cpBodySetMoment(m_body, tocp(moment));
}

void physical_body::set_position(const glm::vec2& position) noexcept
{
    cpBodySetPosition(m_body, tocp(position));
    cpSpaceReindexShapesForBody(m_world->handle(), m_body);
}

void physical_body::set_rotation(float rotation) noexcept
{
    cpBodySetAngle(m_body, tocp(rotation));
}

void physical_body::set_velocity(const glm::vec2& velocity) noexcept
{
    cpBodySetVelocity(m_body, tocp(velocity));
}

void physical_body::sleep() noexcept
{
    cpBodySleep(m_body);
}

void physical_body::wake_up() noexcept
{
    cpBodyActivate(m_body);
}

glm::vec2 physical_body::world_to_local(const glm::vec2& vec) noexcept
{
    return fromcp(cpBodyWorldToLocal(m_body, tocp(vec)));
}

glm::vec2 physical_body::local_to_world(const glm::vec2& vec) noexcept
{
    return fromcp(cpBodyLocalToWorld(m_body, tocp(vec)));
}

cpt::bounding_box physical_body::bounding_box() const noexcept
{
    if(std::empty(m_shapes))
    {
        return cpt::bounding_box{};
    }

    cpBB bb{};
    for(auto shape : m_shapes)
    {
        bb = cpBBMerge(bb, cpShapeGetBB(shape->handle()));
    }

    return cpt::bounding_box{fromcp(bb.l), fromcp(bb.b), fromcp(bb.r - bb.l), fromcp(bb.t - bb.b)};
}

float physical_body::angular_velocity() const noexcept
{
    return fromcp(cpBodyGetAngularVelocity(m_body));
}

float physical_body::mass() const noexcept
{
    return fromcp(cpBodyGetMass(m_body));
}

glm::vec2 physical_body::mass_center() const noexcept
{
    return fromcp(cpBodyGetCenterOfGravity(m_body));
}

float physical_body::moment_of_inertia() const noexcept
{
    return fromcp(cpBodyGetMoment(m_body));
}

glm::vec2 physical_body::position() const noexcept
{
    return fromcp(cpBodyGetPosition(m_body));
}

float physical_body::rotation() const noexcept
{
    return fromcp(cpBodyGetAngle(m_body));
}

glm::vec2 physical_body::velocity() const noexcept
{
    return fromcp(cpBodyGetVelocity(m_body));
}

bool physical_body::sleeping() const noexcept
{
    return static_cast<bool>(cpBodyIsSleeping(m_body));
}

physical_body_type physical_body::type() const noexcept
{
    switch(cpBodyGetType(m_body))
    {
        case CP_BODY_TYPE_DYNAMIC: return physical_body_type::dynamic;
        case CP_BODY_TYPE_STATIC: return physical_body_type::steady;
        case CP_BODY_TYPE_KINEMATIC: return physical_body_type::kinematic;
    }

    std::terminate();
}

physical_constraint::physical_constraint(pin_joint_t, physical_body_ptr first, physical_body_ptr second, const glm::vec2& first_anchor, const glm::vec2& second_anchor)
:m_constaint{cpPinJointNew(first->handle(), second->handle(), tocp(first_anchor), tocp(second_anchor))}
,m_type{physical_constraint_type::pin_joint}
,m_first_body{std::move(first)}
,m_second_body{std::move(second)}
{
    if(!m_constaint)
        throw std::runtime_error{"Can not create physical constaint."};

    cpConstraintSetUserData(m_constaint, this);
    cpSpaceAddConstraint(m_first_body->world()->handle(), m_constaint);
}

physical_constraint::physical_constraint(slide_joint_t, physical_body_ptr first, physical_body_ptr second, const glm::vec2& first_anchor, const glm::vec2& second_anchor, float min, float max)
:m_constaint{cpSlideJointNew(first->handle(), second->handle(), tocp(first_anchor), tocp(second_anchor), tocp(min), tocp(max))}
,m_type{physical_constraint_type::slide_joint}
,m_first_body{std::move(first)}
,m_second_body{std::move(second)}
{
    if(!m_constaint)
        throw std::runtime_error{"Can not create physical constaint."};

    cpConstraintSetUserData(m_constaint, this);
    cpSpaceAddConstraint(m_first_body->world()->handle(), m_constaint);
}

physical_constraint::physical_constraint(pivot_joint_t, physical_body_ptr first, physical_body_ptr second, const glm::vec2& pivot)
:m_constaint{cpPivotJointNew(first->handle(), second->handle(), tocp(pivot))}
,m_type{physical_constraint_type::pivot_joint}
,m_first_body{std::move(first)}
,m_second_body{std::move(second)}
{
    if(!m_constaint)
        throw std::runtime_error{"Can not create physical constaint."};

    cpConstraintSetUserData(m_constaint, this);
    cpSpaceAddConstraint(m_first_body->world()->handle(), m_constaint);
}

physical_constraint::physical_constraint(pivot_joint_t, physical_body_ptr first, physical_body_ptr second, const glm::vec2& first_anchor, const glm::vec2& second_anchor)
:m_constaint{cpPivotJointNew2(first->handle(), second->handle(), tocp(first_anchor), tocp(second_anchor))}
,m_type{physical_constraint_type::pivot_joint}
,m_first_body{std::move(first)}
,m_second_body{std::move(second)}
{
    if(!m_constaint)
        throw std::runtime_error{"Can not create physical constaint."};

    cpConstraintSetUserData(m_constaint, this);
    cpSpaceAddConstraint(m_first_body->world()->handle(), m_constaint);
}

physical_constraint::physical_constraint(groove_joint_t, physical_body_ptr first, physical_body_ptr second, const glm::vec2& first_groove, const glm::vec2& second_groove, const glm::vec2& anchor)
:m_constaint{cpGrooveJointNew(first->handle(), second->handle(), tocp(first_groove), tocp(second_groove), tocp(anchor))}
,m_type{physical_constraint_type::groove_joint}
,m_first_body{std::move(first)}
,m_second_body{std::move(second)}
{
    if(!m_constaint)
        throw std::runtime_error{"Can not create physical constaint."};

    cpConstraintSetUserData(m_constaint, this);
    cpSpaceAddConstraint(m_first_body->world()->handle(), m_constaint);
}

physical_constraint::physical_constraint(damped_spring_t, physical_body_ptr first, physical_body_ptr second, const glm::vec2& first_anchor, const glm::vec2& second_anchor, float rest_length, float stiffness, float damping)
:m_constaint{cpDampedSpringNew(first->handle(), second->handle(), tocp(first_anchor), tocp(second_anchor), tocp(rest_length), tocp(stiffness), tocp(damping))}
,m_type{physical_constraint_type::damped_spring}
,m_first_body{std::move(first)}
,m_second_body{std::move(second)}
{
    if(!m_constaint)
        throw std::runtime_error{"Can not create physical constaint."};

    cpConstraintSetUserData(m_constaint, this);
    cpSpaceAddConstraint(m_first_body->world()->handle(), m_constaint);
}

physical_constraint::physical_constraint(damped_rotary_spring_t, physical_body_ptr first, physical_body_ptr second, float rest_angle, float stiffness, float damping)
:m_constaint{cpDampedRotarySpringNew(first->handle(), second->handle(), tocp(rest_angle), tocp(stiffness), tocp(damping))}
,m_type{physical_constraint_type::damped_rotary_spring}
,m_first_body{std::move(first)}
,m_second_body{std::move(second)}
{
    if(!m_constaint)
        throw std::runtime_error{"Can not create physical constaint."};

    cpConstraintSetUserData(m_constaint, this);
    cpSpaceAddConstraint(m_first_body->world()->handle(), m_constaint);
}

physical_constraint::physical_constraint(rotary_limit_joint_t, physical_body_ptr first, physical_body_ptr second, float min, float max)
:m_constaint{cpRotaryLimitJointNew(first->handle(), second->handle(), tocp(min), tocp(max))}
,m_type{physical_constraint_type::rotary_limit_joint}
,m_first_body{std::move(first)}
,m_second_body{std::move(second)}
{
    if(!m_constaint)
        throw std::runtime_error{"Can not create physical constaint."};

    cpConstraintSetUserData(m_constaint, this);
    cpSpaceAddConstraint(m_first_body->world()->handle(), m_constaint);
}

physical_constraint::physical_constraint(ratchet_joint_t, physical_body_ptr first, physical_body_ptr second, float phase, float ratchet)
:m_constaint{cpRatchetJointNew(first->handle(), second->handle(), tocp(phase), tocp(ratchet))}
,m_type{physical_constraint_type::ratchet_joint}
,m_first_body{std::move(first)}
,m_second_body{std::move(second)}
{
    if(!m_constaint)
        throw std::runtime_error{"Can not create physical constaint."};

    cpConstraintSetUserData(m_constaint, this);
    cpSpaceAddConstraint(m_first_body->world()->handle(), m_constaint);
}

physical_constraint::physical_constraint(gear_joint_t, physical_body_ptr first, physical_body_ptr second, float phase, float ratio)
:m_constaint{cpGearJointNew(first->handle(), second->handle(), tocp(phase), tocp(ratio))}
,m_type{physical_constraint_type::gear_joint}
,m_first_body{std::move(first)}
,m_second_body{std::move(second)}
{
    if(!m_constaint)
        throw std::runtime_error{"Can not create physical constaint."};

    cpConstraintSetUserData(m_constaint, this);
    cpSpaceAddConstraint(m_first_body->world()->handle(), m_constaint);
}

physical_constraint::physical_constraint(motor_joint_t, physical_body_ptr first, physical_body_ptr second, float rate)
:m_constaint{cpSimpleMotorNew(first->handle(), second->handle(), tocp(rate))}
,m_type{physical_constraint_type::motor_joint}
,m_first_body{std::move(first)}
,m_second_body{std::move(second)}
{
    if(!m_constaint)
        throw std::runtime_error{"Can not create physical constaint."};

    cpConstraintSetUserData(m_constaint, this);
    cpSpaceAddConstraint(m_first_body->world()->handle(), m_constaint);
}

physical_constraint::~physical_constraint()
{
    cpSpaceRemoveConstraint(cpBodyGetSpace(cpConstraintGetBodyA(m_constaint)), m_constaint);
    cpConstraintFree(m_constaint);
}

void physical_constraint::set_max_force(float force) noexcept
{
    cpConstraintSetMaxForce(m_constaint, tocp(force));
}

void physical_constraint::set_error_bias(float bias) noexcept
{
    cpConstraintSetErrorBias(m_constaint, tocp(bias));
}

void physical_constraint::set_max_bias(float bias) noexcept
{
    cpConstraintSetMaxBias(m_constaint, tocp(bias));
}

void physical_constraint::set_collide_bodies(bool enable) noexcept
{
    cpConstraintSetCollideBodies(m_constaint, static_cast<bool>(enable));
}

float physical_constraint::max_force() const noexcept
{
    return fromcp(cpConstraintGetMaxForce(m_constaint));
}

float physical_constraint::error_bias() const noexcept
{
    return fromcp(cpConstraintGetErrorBias(m_constaint));
}

float physical_constraint::max_bias() const noexcept
{
    return fromcp(cpConstraintGetMaxBias(m_constaint));
}

bool physical_constraint::collide_bodies() const noexcept
{
    return static_cast<bool>(cpConstraintGetCollideBodies(m_constaint));
}

void physical_constraint::set_pin_joint_first_anchor(const glm::vec2& anchor) noexcept
{
    cpPinJointSetAnchorA(m_constaint, tocp(anchor));
}

void physical_constraint::set_pin_joint_second_anchor(const glm::vec2& anchor) noexcept
{
    cpPinJointSetAnchorB(m_constaint, tocp(anchor));
}

void physical_constraint::set_pin_joint_distance(float distance) noexcept
{
    cpPinJointSetDist(m_constaint, tocp(distance));
}

glm::vec2 physical_constraint::pin_joint_first_anchor() const noexcept
{
    return fromcp(cpPinJointGetAnchorA(m_constaint));
}

glm::vec2 physical_constraint::pin_joint_second_anchor() const noexcept
{
    return fromcp(cpPinJointGetAnchorB(m_constaint));
}

float physical_constraint::pin_joint_distance() const noexcept
{
    return fromcp(cpPinJointGetDist(m_constaint));
}


void physical_constraint::set_slide_joint_first_anchor(const glm::vec2& anchor) noexcept
{
    cpSlideJointSetAnchorA(m_constaint, tocp(anchor));
}

void physical_constraint::set_slide_joint_second_anchor(const glm::vec2& anchor) noexcept
{
    cpSlideJointSetAnchorB(m_constaint, tocp(anchor));
}

void physical_constraint::set_slide_joint_min(float min) noexcept
{
    cpSlideJointSetMin(m_constaint, tocp(min));
}

void physical_constraint::set_slide_joint_max(float max) noexcept
{
    cpSlideJointSetMax(m_constaint, tocp(max));
}

glm::vec2 physical_constraint::slide_joint_first_anchor() const noexcept
{
    return fromcp(cpSlideJointGetAnchorA(m_constaint));
}

glm::vec2 physical_constraint::slide_joint_second_anchor() const noexcept
{
    return fromcp(cpSlideJointGetAnchorB(m_constaint));
}

float physical_constraint::slide_joint_min() const noexcept
{
    return fromcp(cpSlideJointGetMin(m_constaint));
}

float physical_constraint::slide_joint_max() const noexcept
{
    return fromcp(cpSlideJointGetMax(m_constaint));
}


void physical_constraint::set_pivot_joint_first_anchor(const glm::vec2& anchor) noexcept
{
    cpPivotJointSetAnchorA(m_constaint, tocp(anchor));
}

void physical_constraint::set_pivot_joint_second_anchor(const glm::vec2& anchor) noexcept
{
    cpPivotJointSetAnchorB(m_constaint, tocp(anchor));
}

glm::vec2 physical_constraint::pivot_joint_first_anchor() const noexcept
{
    return fromcp(cpPivotJointGetAnchorA(m_constaint));
}

glm::vec2 physical_constraint::pivot_joint_second_anchor() const noexcept
{
    return fromcp(cpPivotJointGetAnchorB(m_constaint));
}


void physical_constraint::set_groove_joint_first_groove(const glm::vec2& groove) noexcept
{
    cpGrooveJointSetGrooveA(m_constaint, tocp(groove));
}

void physical_constraint::set_groove_joint_second_groove(const glm::vec2& groove) noexcept
{
    cpGrooveJointSetGrooveB(m_constaint, tocp(groove));
}

void physical_constraint::set_groove_joint_anchor(const glm::vec2& anchor) noexcept
{
    cpGrooveJointSetAnchorB(m_constaint, tocp(anchor));
}

glm::vec2 physical_constraint::groove_joint_first_groove() const noexcept
{
    return fromcp(cpGrooveJointGetGrooveA(m_constaint));
}

glm::vec2 physical_constraint::groove_joint_second_groove() const noexcept
{
    return fromcp(cpGrooveJointGetGrooveB(m_constaint));
}

glm::vec2 physical_constraint::groove_joint_anchor() const noexcept
{
    return fromcp(cpGrooveJointGetAnchorB(m_constaint));
}


void physical_constraint::set_damped_spring_first_anchor(const glm::vec2& anchor) noexcept
{
    cpDampedSpringSetAnchorA(m_constaint, tocp(anchor));
}

void physical_constraint::set_damped_spring_second_anchor(const glm::vec2& anchor) noexcept
{
    cpDampedSpringSetAnchorB(m_constaint, tocp(anchor));
}

void physical_constraint::set_damped_spring_rest_length(float rest_length) noexcept
{
    cpDampedSpringSetRestLength(m_constaint, tocp(rest_length));
}

void physical_constraint::set_damped_spring_stiffness(float stiffness) noexcept
{
    cpDampedSpringSetStiffness(m_constaint, tocp(stiffness));
}

void physical_constraint::set_damped_spring_damping(float damping) noexcept
{
    cpDampedSpringSetDamping(m_constaint, tocp(damping));
}

glm::vec2 physical_constraint::damped_spring_first_anchor() const noexcept
{
    return fromcp(cpDampedSpringGetAnchorA(m_constaint));
}

glm::vec2 physical_constraint::damped_spring_second_anchor() const noexcept
{
    return fromcp(cpDampedSpringGetAnchorB(m_constaint));
}

float physical_constraint::damped_spring_rest_length() const noexcept
{
    return fromcp(cpDampedSpringGetRestLength(m_constaint));
}

float physical_constraint::damped_spring_stiffness() const noexcept
{
    return fromcp(cpDampedSpringGetStiffness(m_constaint));
}

float physical_constraint::damped_spring_damping() const noexcept
{
    return fromcp(cpDampedSpringGetDamping(m_constaint));
}


void physical_constraint::set_damped_rotary_spring_rest_angle(float rest_angle) noexcept
{
    cpDampedRotarySpringSetRestAngle(m_constaint, tocp(rest_angle));
}

void physical_constraint::set_damped_rotary_spring_stiffness(float stiffness) noexcept
{
    cpDampedRotarySpringSetStiffness(m_constaint, tocp(stiffness));
}

void physical_constraint::set_damped_rotary_spring_damping(float damping) noexcept
{
    cpDampedRotarySpringSetDamping(m_constaint, tocp(damping));
}

float physical_constraint::damped_rotary_spring_rest_angle() const noexcept
{
    return fromcp(cpDampedRotarySpringGetRestAngle(m_constaint));
}

float physical_constraint::damped_rotary_spring_stiffness() const noexcept
{
    return fromcp(cpDampedRotarySpringGetStiffness(m_constaint));
}

float physical_constraint::damped_rotary_spring_damping() const noexcept
{
    return fromcp(cpDampedRotarySpringGetDamping(m_constaint));
}


void physical_constraint::set_rotary_limit_joint_min(float min) noexcept
{
    cpRotaryLimitJointSetMin(m_constaint, tocp(min));
}

void physical_constraint::set_rotary_limit_joint_max(float max) noexcept
{
    cpRotaryLimitJointSetMax(m_constaint, tocp(max));
}

float physical_constraint::rotary_limit_joint_min() const noexcept
{
    return fromcp(cpRotaryLimitJointGetMin(m_constaint));
}

float physical_constraint::rotary_limit_joint_max() const noexcept
{
    return fromcp(cpRotaryLimitJointGetMax(m_constaint));
}


void physical_constraint::set_ratchet_joint_angle(float angle) noexcept
{
    cpRatchetJointSetAngle(m_constaint, tocp(angle));
}

void physical_constraint::set_ratchet_joint_phase(float phase) noexcept
{
    cpRatchetJointSetPhase(m_constaint, tocp(phase));
}

void physical_constraint::set_ratchet_joint_ratchet(float ratchet) noexcept
{
    cpRatchetJointSetRatchet(m_constaint, tocp(ratchet));
}

float physical_constraint::ratchet_joint_angle() const noexcept
{
    return fromcp(cpRatchetJointGetAngle(m_constaint));
}

float physical_constraint::ratchet_joint_phase() const noexcept
{
    return fromcp(cpRatchetJointGetPhase(m_constaint));
}

float physical_constraint::ratchet_joint_ratchet() const noexcept
{
    return fromcp(cpRatchetJointGetRatchet(m_constaint));
}


void physical_constraint::set_gear_joint_phase(float phase) noexcept
{
    cpGearJointSetPhase(m_constaint, tocp(phase));
}

void physical_constraint::set_gear_joint_ratio(float ratio) noexcept
{
    cpGearJointSetRatio(m_constaint, tocp(ratio));
}

float physical_constraint::gear_joint_phase() const noexcept
{
    return fromcp(cpGearJointGetPhase(m_constaint));
}

float physical_constraint::gear_joint_ratio() const noexcept
{
    return fromcp(cpGearJointGetRatio(m_constaint));
}


void physical_constraint::set_motor_joint_rate(float rate) noexcept
{
    cpSimpleMotorSetRate(m_constaint, tocp(rate));
}

float physical_constraint::motor_joint_rate() const noexcept
{
    return fromcp(cpSimpleMotorGetRate(m_constaint));
}


}

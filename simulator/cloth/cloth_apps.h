//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "capsule_app.h"              // capsule collider
#include "ccd_app.h"                  // capsule collider with animation ccd
#include "convex_collision_app.h"     // convex collider
#include "distance_constraint_app.h"  // distance constraints
#include "free_fall_app.h"            // plane collider
#include "friction_app.h"             // friction comparsion
#include "geodesic_app.h"             // geodesic distance for tether constraints
#include "inter_collision_app.h"      // lap together
#include "local_global_app.h"
#include "multi_solver_app.h"
#include "plane_collision_app.h"           // plane collider
#include "self_collision_app.h"            // self lap together
#include "sphere_app.h"                    // sphere collider
#include "stiffness_per_constraint_app.h"  // setTetherConstraintStiffness
#include "teleport_app.h"
#include "tether_app.h"
#include "time_step_app.h"
#include "triangle_app.h"          // mesh collider
#include "virtual_particle_app.h"  // virtual particles
#include "wind_app.h"
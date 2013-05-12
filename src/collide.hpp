#ifndef COLLIDE_HPP_JYOBV4TP
#define COLLIDE_HPP_JYOBV4TP


#include "level.hpp"
#include "actor.hpp"


bool collide_level_actor (const Level& level, Actor& actor, int tile_size);
bool collide_actor_actor (Actor& a, Actor& b);


#endif /* end of include guard: COLLIDE_HPP_JYOBV4TP */


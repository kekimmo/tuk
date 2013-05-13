#ifndef ACTION_HPP_RKI6CFHG
#define ACTION_HPP_RKI6CFHG


#include "vec.hpp"
#include "actor.hpp"


struct Action {
  Action (Actor& actor);
  virtual ~Action () {};
  virtual void perform () const = 0;

  Actor& actor;
};


struct WaitAction : public Action {
  WaitAction (Actor& actor);
  virtual void perform () const override;
};


struct MoveAction : public Action {
  MoveAction (Actor& actor, const Point& target);

  virtual void perform () const override;

  const Point& target;
};


#endif /* end of include guard: ACTION_HPP_RKI6CFHG */


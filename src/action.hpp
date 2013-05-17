#ifndef ACTION_HPP_RKI6CFHG
#define ACTION_HPP_RKI6CFHG

#include <string>
#include "vec.hpp"
#include "actor.hpp"
#include "level.hpp"


struct Action {
  Action (Actor& actor);
  virtual ~Action () {};
  virtual void perform () const = 0;
  virtual std::string str () const = 0;

  Actor& actor;
};


struct WaitAction : public Action {
  WaitAction (Actor& actor);
  virtual void perform () const override;
  virtual std::string str () const override;
};


struct MoveAction : public Action {
  MoveAction (Actor& actor, const Point& target);

  virtual void perform () const override;
  virtual std::string str () const override;

  const Point target;
};


struct DigAction : public Action {
  DigAction (Actor& actor, const Point& target, Level& level);

  virtual void perform () const override;
  virtual std::string str () const override;

  const Point target;
  Level& level;
};


struct MineAction : public Action {
  MineAction (Actor& actor, const Point& target, Level& level);
  virtual void perform () const override;
  virtual std::string str () const override;

  const Point target;
  Level& level;
};


struct DepositAction : public Action {
  DepositAction (Actor& actor, const Point& target, Level& level);
  virtual void perform () const override;
  virtual std::string str () const override;

  const Point target;
  Level& level;
};

#endif /* end of include guard: ACTION_HPP_RKI6CFHG */


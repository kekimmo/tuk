#ifndef TASK_HPP_5ZBIXUXE
#define TASK_HPP_5ZBIXUXE


#include "actor.hpp"
#include "action.hpp"
#include "level.hpp"


class Task {
  public:
    virtual ~Task () {}
    virtual Action* work () = 0;
    virtual bool ready () const = 0;
};


class GoTask : public Task {
  public:
    GoTask (const Level& level, Actor& actor, const Vec<int>& target);

    virtual Action* work () override;
    virtual bool ready () const override;

  private:
    const Level& _level;
    Actor& _actor;
    const Vec<int> _target;
};


#endif /* end of include guard: TASK_HPP_5ZBIXUXE */


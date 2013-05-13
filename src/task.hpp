#ifndef TASK_HPP_5ZBIXUXE
#define TASK_HPP_5ZBIXUXE


#include "actor.hpp"
#include "level.hpp"


class Task {
  public:
    virtual ~Task () {}
    virtual void work () = 0;
    virtual bool ready () const = 0;
};


class MoveTask : public Task {
  public:
    MoveTask (const Level& level, Actor& actor, const Vec<int>& target);

    virtual void work () override;
    virtual bool ready () const override;

  private:
    const Level& _level;
    Actor& _actor;
    const Vec<int> _target;
};


#endif /* end of include guard: TASK_HPP_5ZBIXUXE */


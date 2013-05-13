#ifndef TASK_HPP_5ZBIXUXE
#define TASK_HPP_5ZBIXUXE


#include "actor.hpp"
#include "action.hpp"
#include "level.hpp"
#include "debuginfo.hpp"


class Task {
  public:
    virtual ~Task () {}
    virtual Action* work (DebugInfo& dbg) = 0;
    virtual bool ready () const = 0;
};


class GoTask : public Task {
  public:
    GoTask (const Level& level, Actor& actor, const Point& target);

    virtual Action* work (DebugInfo& dbg) override;
    virtual bool ready () const override;

  private:
    const Level& _level;
    Actor& _actor;
    const Point _target;
    std::list<Point> _path;
};


class DigTask : public Task {
  public:
    DigTask (Level& level, Actor& actor, const Point& target);

    virtual Action* work (DebugInfo& dbg) override;
    virtual bool ready () const override;

  private:
    bool find_digging_place (Point& point) const;
    Level& _level;
    Actor& _actor;
    const Point _target;
    Point _digging_place;
    Task* _subtask;
    enum {
      STARTING,
      FINDING,
      DIGGING,
      DONE,
    } _state;

};


#endif /* end of include guard: TASK_HPP_5ZBIXUXE */


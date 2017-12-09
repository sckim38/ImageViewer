#pragma once

#include <memory>
#include <string>
#include <iostream>

template <class State>
using StateRef = std::unique_ptr<State>;

template <typename StateMachine, class State>
class GenericState
{
public:
    explicit GenericState(StateMachine &m, StateRef<State> &state) :
        m(m), state(state) {}

    template <class ConcreteState>
    static void init(StateMachine &m, StateRef<State> &state) {
        state = StateRef<State>(new ConcreteState(m, state));
        state->entry();
    }

protected:
    template <class ConcreteState>
    void change() {
        exit();
        init<ConcreteState>(m, state);
    }

    void reenter() {
        exit();
        entry();
    }

private:
    virtual void entry() {}
    virtual void exit() {}

protected:
    StateMachine &m;

private:
    StateRef<State> &state;
};


class Machine
{
public:
    Machine() {}
    ~Machine() {}
    void start();

public:
    enum Color {
        BLUE,
        RED
    };

public:
    void liftUp() { levelState->liftUp(); }
    void bringDown() { levelState->bringDown(); }
    void paint(Color color) { directionState->paint(color); }
    void turnRight() { directionState->turnRight(); }
    void turnLeft() { directionState->turnLeft(); }

private:
    static void print(const std::string &str) { std::cout << str << std::endl; }

    static void unhandledEvent() { print("unhandled event"); }
    void changedColor() { print("changed color"); }

private:
    struct LevelState : public GenericState<Machine, LevelState> {
        using GenericState::GenericState;
        virtual void liftUp() { unhandledEvent(); }
        virtual void bringDown() { unhandledEvent(); }
    };
    StateRef<LevelState> levelState;

    struct High : public LevelState {
        using LevelState::LevelState;
        void entry() { print("entering High"); }
        void liftUp() { print("already High"); }
        void bringDown() { change<Low>(); }
        void exit() { print("leaving High"); }
    };

    struct Low : public LevelState {
        using LevelState::LevelState;
        void entry() { print("entering Low"); }
        void liftUp() { change<High>(); }
        void bringDown() { print("already Low"); }
        void exit() { print("leaving Low"); }
    };

private:
    struct ColorState : public GenericState<Machine, ColorState> {
        using GenericState::GenericState;
        virtual void paint(Color color) { (void)color; unhandledEvent(); }
    };

    struct Red : public ColorState {
        using ColorState::ColorState;
        void entry() { m.changedColor(); }
        void paint(Color color);
    };

    struct Blue : public ColorState {
        using ColorState::ColorState;
        void entry() { m.changedColor(); }
        void paint(Color color);
    };

private:
    struct DirectionState : public GenericState<Machine, DirectionState> {
        using GenericState::GenericState;
        virtual void paint(Color color) { (void)color; unhandledEvent(); }
        virtual void turnRight() { unhandledEvent(); }
        virtual void turnLeft() { unhandledEvent(); }
    };
    StateRef<DirectionState> directionState;

    struct Left : public DirectionState {
        using DirectionState::DirectionState;
        void entry() { ColorState::init<Red>(m, colorState); }
        void paint(Color color) { colorState->paint(color); }
        void turnRight() { change<Right>(); }
    private:
        StateRef<ColorState> colorState;
    };

    struct Right : public DirectionState {
        using DirectionState::DirectionState;
        void turnLeft() { change<Left>(); }
    };
};


inline void Machine::start()
{
    LevelState::init<High>(*this, levelState);
    DirectionState::init<Left>(*this, directionState);
}

inline void Machine::Red::paint(Machine::Color color)
{
    if (color == BLUE) change<Blue>();
    else ColorState::paint(color);
}

inline void Machine::Blue::paint(Machine::Color color)
{
    if (color == RED) change<Red>();
    else ColorState::paint(color);
}


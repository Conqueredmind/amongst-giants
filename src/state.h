// state.h
// Terrance Howard (heyterrance@gmail.com)

#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include <stdio.h> // printf
#include <stack> // std::stack

#include <SFML/System.hpp> // float
#include <SFML/Window.hpp> // sf::Event
#include <SFML/Graphics.hpp> // sf::RenderWindow

#include "util.h" // dbprintf

// StateManager
// Manages GameStates in a stack.
class StateManager;

// GameState
// Base class, defines a game state.
class GameState {
public:
  GameState();
  virtual ~GameState();

  // update(dt)
  // Update scene.
  virtual void update(float dt) = 0;

  // draw(window)
  // Draw contents to window.
  virtual void draw(sf::RenderWindow& window) const = 0;

	virtual void refresh() { };

  // save()
  // Save necessary information.
  virtual void save() = 0;

  // manager(), setManager(manager)
  // Get and set StateManager.
  StateManager* manager() const;
  void setManager(StateManager* manager);

protected:
  // push(state)
  // Push a new state on the StaeManager stack.
  void push(GameState* state) const;

  // pop()
  // Pop current state (this) from the StateManager stack.
  void pop() const;

  // replaceWith(state)
  // Pop current state (this) and push new state.
  void replaceWith(GameState* state) const;

private:
  StateManager* manager_;
};

class StateManager {
public:
  StateManager();
  ~StateManager();

  // push(state)
  // Push state onto stack and sets its manager.
  void push(GameState* state);
  
  // pop()
  // Pop a state from stack.
  void pop();

  // replaceWith(state)
  // Calls pop() then push(state).
  void replaceWith(GameState* state);

  // update(dt)
  // Process next frame of logic.
  void update(float dt);
	
	// refresh()
	// Refresh underlying room.
	void refresh();

  // draw(window)
  // Draw top state to window.
  void draw(sf::RenderWindow& window) const;

private:
  // deleteState()
  // Deletes the state on top of the stack in the case of pop() and
  // replaceWith(...).
  void deleteState();

  std::stack<GameState*> states_;
  GameState* state_;
  bool keep_state_;
};

#endif

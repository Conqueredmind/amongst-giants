// world_state.cc
// Terrance Howard (heyterrance@gmail.com)

#include "world_state.h"

WorldState::WorldState() : GameState(), jack_(300.0f, 30.0f),
                           room_idx_(Room::getIndex(1,0)) {
  for (int i = 0; i < Room::kRoomsX; ++i)
    for (int j = 0; j < Room::kRoomsY; ++j) {
      rooms_.push_back(nullptr);
      dbprintf("%d: (%d, %d)\n", Room::getIndex(i, j), i, j);
    }
  loadRoom(room_idx_);
}

WorldState::~WorldState() {
  for (size_t i = 0; i < rooms_.size(); ++i)
    if (rooms_[i])
      delete rooms_[i];
}

void WorldState::reloadRoom() {
  Room* old = rooms_[room_idx_];
  rooms_[room_idx_] = new Room(old->x, old->y);
  delete old;
}

void WorldState::loadRoom(int index, bool load_adjacent) {
  const std::pair<int, int> p = Room::getPos(index);
  return loadRoom(p.first, p.second, load_adjacent);
}

void WorldState::loadRoom(int i, int j, bool load_adjacent) {
  const int idx = Room::getIndex(i, j);
  if (idx >= 0 and idx < Room::kRooms and not rooms_[idx]) {
    rooms_[idx] = new Room(i, j);
    if (load_adjacent) loadAdjacent(i, j);
  }
}

void WorldState::loadAdjacent(int index) {
  const std::pair<int, int> p = Room::getPos(index);
  return loadAdjacent(p.first, p.second);
}

void WorldState::loadAdjacent(int i, int j) {
  loadRoom(i-1, j, false);
  loadRoom(i+1, j, false);
  loadRoom(i, j-1, false);
  loadRoom(i, j+1, false);
}

void WorldState::update(float dt) {
  if (dt == 0.0f) return;
  processKeyboard(dt);
  checkCollision(dt);
  checkRoomBounds(dt);
  jack_.update(dt);
  loadRoom(room_idx_, true);
}

void WorldState::checkCollision(float dt) {
  const Room* room = rooms_[room_idx_];
  if (!room) return;
  const float grav = 198.0f;
  jack_.ddy = grav;
  
  for (const sf::Rect<float>& block: room->blocks) {
    auto bound_pair = jack_.fakeUpdate(dt); 
    // Check x collisions
    if ( bound_pair.first.intersects(block) ) {
      if (jack_.dx < 0.0f) jack_.x = block.left + block.width;
      else if (jack_.dx > 0.0f) jack_.x = block.left - jack_.width();
      jack_.dx = 0.0f;
    }

    // Check y collisions
    if ( bound_pair.second.intersects(block) ) {
      if (jack_.dy < 0.0f) {
        jack_.y = block.top + block.height;
        jack_.can_jump = false;
      } else if (jack_.dy > 0.0f) {
        jack_.y = block.top - jack_.height();
        jack_.in_air = false;
        jack_.can_jump = true;
      }
      jack_.dy = 0.0f;
      jack_.ddy = 0.0f;
    }
  }
  if (jack_.ddy != 0.0f) jack_.can_jump = false;
}

void WorldState::processKeyboard(float dt) {
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
    while (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
      // Wait for it to be released before reloading room
    }
    dt = 0.0f;
    reloadRoom();
  }
  moveJack();
}

void WorldState::checkRoomBounds(float dt) {
  float x1 = jack_.x + jack_.dx * dt;
  if (x1 + jack_.width() > WIN_WIDTH) {
    const int next_idx = rooms_[room_idx_]->next(+1, 0);
    if (next_idx != room_idx_) {
      room_idx_ = next_idx;
      jack_.x = 0;
    }
  } else if (x1 < 0) {
    const int next_idx = rooms_[room_idx_]->next(-1, 0);
    if (next_idx != room_idx_) {
      room_idx_ = next_idx;
      jack_.x = WIN_WIDTH - jack_.width();
    }
  }
  
  float y1 = jack_.y + jack_.dy * dt;
  if (y1 + jack_.height() > WIN_HEIGHT) { // at the bottom
    const int next_idx = rooms_[room_idx_]->next(0, +1);
    if (next_idx != room_idx_) {
      room_idx_ = next_idx;
      jack_.y = 0.0f;
    }
  } else if (y1 < 0) { // at the top
    const int next_idx = rooms_[room_idx_]->next(0, -1);
    if (next_idx != room_idx_) {
      room_idx_ = next_idx;
      jack_.y = WIN_HEIGHT - jack_.height();
    }
  }
}

void WorldState::moveJack() {
  const float jump_vel(188.0f);
  const float vel(95.0f);

  // Press Left or Right
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) or
      sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    jack_.dx = -vel;
  else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) or
             sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    jack_.dx = vel;

  // Press Up or Down
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) or
      sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
    if (jack_.can_jump) {
      jack_.dy = -jump_vel;
      jack_.can_jump = false;
      jack_.in_air = true;
    }
  } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) or
             sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
    jack_.dy = jump_vel;
  }
}

void WorldState::draw(sf::RenderWindow& window) const {
  window.draw(jack_);
  if (rooms_[room_idx_]) {
    window.draw(*rooms_[room_idx_]);
  }
}
  
void WorldState::save() {
  for (Room* room : rooms_)
    if(room) room->save();
}

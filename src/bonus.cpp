#include "bonus.hpp"

Bonus::Bonus(BonusType type, bool temp, int duration, int amt) {
  this->type = type;
  this->temp = temp;
  this->duration = duration;
  this->amt = amt;
}

BonusType Bonus::get_type() {
  return this->type;
}

bool Bonus::is_temp() {
  return this->temp;
}

int Bonus::get_duration() {
  return this->duration;
}

// Returns true if successful 
bool Bonus::decrement() {
  if (this->temp) {
    this->duration--;
    return true;
  }
  return false;
}

int Bonus::get_amt() {
  return this->amt;
}


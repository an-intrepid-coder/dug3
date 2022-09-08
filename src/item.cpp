#include "item.hpp"

Consumable::Consumable(ItemEffect effect, string name, int charges) {
  this->effect = effect;
  this->name = name;
  this->charges = charges;
}

ItemEffect Consumable::get_effect() {
  return this->effect;
}

int Consumable::get_charges(){
  return this->charges;
}

int Consumable::change_charges(int amt) {
  this->charges = this->charges + amt;
  if (this->charges < 0) {
    this->charges = 0;
  }
  return this->charges;
}

string Consumable::get_name() {
  return this->name;
}

Consumable MinorHealingPotion() {
  return Consumable(MINOR_HEALING, "Minor Healing Potion", 1);
}

string Gear::get_name() {
  return this->name;
}


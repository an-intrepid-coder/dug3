#ifndef ITEM_HPP
#define ITEM_HPP

#include <string>

using std::string;

typedef enum {
  MINOR_HEALING,
} ItemEffect;

class Consumable {
  public:
    Consumable(ItemEffect effect, string name, int charges);
    ItemEffect get_effect();
    int get_charges();
    int change_charges(int amt);
    string get_name();
  private:
    ItemEffect effect;
    int charges;
    string name;
};

Consumable MinorHealingPotion();

class Gear { // TODO
  public:
    string get_name();
  private:
    string name;
};

#endif


#ifndef BONUS_HPP
#define BONUS_HPP

typedef enum {
  EXTRA_DMG_BONUS,
  // TODO: Many more!
} BonusType;

class Bonus {
  public:
    Bonus(BonusType type, bool temp, int duration, int amt);
    BonusType get_type();
    bool is_temp();
    int get_duration();
    bool decrement();
    int get_amt();
  private:
    BonusType type;
    bool temp;
    int duration;
    int amt;
};

#endif


#ifndef BONUS_HPP
#define BONUS_HPP

typedef enum {
  EXTRA_DMG_BONUS,
  // TODO: Many more!
} BonusType;

typedef struct {
  BonusType type;
  bool temp;
  int duration;
} Bonus;

#endif


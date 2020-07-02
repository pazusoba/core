#ifndef PAD_H
#define PAD_H

#include <string>

namespace pad {
    /**
     * All orbs in the game.
     * 
     * seal is a tape over a row and orbs cannot move,
     * disabled means it will not get erased even if connected.
     * 
     * Also, consider weight because it can be quite important
     */
    enum orbs { empty, fire, water, wood, light, dark, recovery, jammer, bomb, poison, poison_plus, seal, disabled };
    const int ORB_COUNT = 13;
    // For displaying the orb name
    const std::string ORB_NAMES[ORB_COUNT] = { "Empty", "Fire", "Water", "Wood", "Light", "Dark", "Heal", "Jammer", "Bomb", "Poison", "Poison+", "Tape", "-X-"};
}

#endif

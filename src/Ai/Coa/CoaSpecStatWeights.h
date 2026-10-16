/*
 * Generated from the CoA Loot Decider profiles (CoALootProfiles.lua).
 *
 * Stat weights for gear selection, one set per CoA specialization.
 *
 * WHAT THIS FIXES: StatsWeightCalculator::GenerateBasicWeights is a chain of
 * if/else over class and Blizzard talent tab. CoA classes match no branch and
 * are left with the base weights alone - stamina 0.1, armor 0.001, bonus 1.0.
 * A bot then picks gear by the item's total value, so a healer happily takes
 * agility and a melee spec takes spell power.
 *
 * WHERE THE NUMBERS COME FROM: the CoA Loot Decider addon's specialization profiles
 * (BisBeard default weights, cross-checked against CoA Build Hub and Ascension tooltips, 2026-08-22),
 * so bots and the addon rate gear the same way. Every weight is the addon's value times 2, which keeps
 * the ratios within a specialization identical while the bot's base weights (stamina 0.1, unrecognized
 * stats 1.0) keep the influence they had before. The addon's separate healing weight is left out:
 * spell power on an item already feeds both SPELL_POWER and HEAL_POWER here. Negative weights are the
 * previous table's penalties, kept where the addon gives that stat no weight.
 *
 * needsThrown is not a weight at all. It records that the class has an ability
 * whose EquippedItemSubClassMask allows thrown weapons only - Throw, which
 * thirteen CoA classes learn at level 1, and the whole Headhunting kit around
 * Impaling Spear. The factory never offers INVTYPE_THROWN for the ranged slot,
 * so those bots run with an empty belt and the spells are simply uncastable.
 * Specs that carry a bow there are excluded.
 */

#ifndef PLAYERBOTS_COASPECSTATWEIGHTS_H
#define PLAYERBOTS_COASPECSTATWEIGHTS_H

#include <cstdint>

#include "StatsCollector.h"

struct CoaStatWeight
{
    StatsType stat;
    float weight;
};

struct CoaSpecStats
{
    uint8_t classId;
    uint16_t specId;
    char const* specName;
    char const* shape;              // for traceability only
    char const* weapon;             // twoHand, oneHand, dualWield, shield,
                                    // ranged, caster, any
    bool needsThrown;               // a rotation spell requires a thrown weapon
    CoaStatWeight const* weights;
    uint8_t count;
};

// Barbarian / Brutality (physisch_nah)
static constexpr CoaStatWeight CoaStats_12_2[] = { { STATS_TYPE_MELEE_DPS, 28.0f }, { STATS_TYPE_AGILITY, 4.39f }, { STATS_TYPE_STRENGTH, 4.376f }, { STATS_TYPE_ATTACK_POWER, 2.0f }, { STATS_TYPE_CRIT, 1.522f }, { STATS_TYPE_HASTE, 1.2f }, { STATS_TYPE_HIT, 1.0f }, { STATS_TYPE_EXPERTISE, 1.0f }, { STATS_TYPE_ARMOR_PENETRATION, 0.9f } };
// Barbarian / Ancestry (physisch_nah)
static constexpr CoaStatWeight CoaStats_12_3[] = { { STATS_TYPE_AGILITY, 2.774f }, { STATS_TYPE_STRENGTH, 2.0f }, { STATS_TYPE_ATTACK_POWER, 2.0f }, { STATS_TYPE_HASTE, 1.1f }, { STATS_TYPE_HIT, 1.0f }, { STATS_TYPE_CRIT, 1.0f }, { STATS_TYPE_EXPERTISE, 1.0f }, { STATS_TYPE_ARMOR_PENETRATION, 0.5f } };
// Barbarian / Headhunting (physisch_fern)
static constexpr CoaStatWeight CoaStats_12_1[] = { { STATS_TYPE_RANGED_DPS, 28.0f }, { STATS_TYPE_AGILITY, 2.946f }, { STATS_TYPE_ATTACK_POWER, 2.0f }, { STATS_TYPE_CRIT, 1.3f }, { STATS_TYPE_HASTE, 1.2f }, { STATS_TYPE_ARMOR_PENETRATION, 0.9f } };
// Witch Doctor / Shadowhunting (physisch_fern)
static constexpr CoaStatWeight CoaStats_13_4[] = { { STATS_TYPE_RANGED_DPS, 28.0f }, { STATS_TYPE_AGILITY, 4.0f }, { STATS_TYPE_INTELLECT, 2.4f }, { STATS_TYPE_ATTACK_POWER, 2.0f }, { STATS_TYPE_CRIT, 1.344f }, { STATS_TYPE_HASTE, 1.0f }, { STATS_TYPE_SPELL_POWER, 0.4f } };
// Witch Doctor / Brewing (heilung)
static constexpr CoaStatWeight CoaStats_13_6[] = { { STATS_TYPE_SPELL_POWER, 2.0f }, { STATS_TYPE_HASTE, 1.3f }, { STATS_TYPE_CRIT, 1.0f }, { STATS_TYPE_SPIRIT, 0.6f }, { STATS_TYPE_INTELLECT, 0.14f }, { STATS_TYPE_ATTACK_POWER, -1.0f } };
// Witch Doctor / Voodoo (zauber)
static constexpr CoaStatWeight CoaStats_13_5[] = { { STATS_TYPE_SPIRIT, 3.008f }, { STATS_TYPE_CRIT, 2.476f }, { STATS_TYPE_SPELL_POWER, 2.0f }, { STATS_TYPE_HASTE, 1.2f }, { STATS_TYPE_INTELLECT, 0.346f }, { STATS_TYPE_SPELL_PENETRATION, 0.02f }, { STATS_TYPE_ATTACK_POWER, -1.0f } };
// Felsworn / Slayer (physisch_nah)
static constexpr CoaStatWeight CoaStats_14_8[] = { { STATS_TYPE_MELEE_DPS, 28.0f }, { STATS_TYPE_AGILITY, 5.0f }, { STATS_TYPE_STRENGTH, 2.0f }, { STATS_TYPE_ATTACK_POWER, 2.0f }, { STATS_TYPE_CRIT, 1.468f }, { STATS_TYPE_HASTE, 1.2f }, { STATS_TYPE_EXPERTISE, 1.0f }, { STATS_TYPE_ARMOR_PENETRATION, 0.6f } };
// Felsworn / Infernal (zauber)
static constexpr CoaStatWeight CoaStats_14_7[] = { { STATS_TYPE_CRIT, 2.484f }, { STATS_TYPE_SPELL_POWER, 2.0f }, { STATS_TYPE_HASTE, 1.2f }, { STATS_TYPE_SPIRIT, 1.034f }, { STATS_TYPE_INTELLECT, 0.828f }, { STATS_TYPE_SPELL_PENETRATION, 0.02f }, { STATS_TYPE_ATTACK_POWER, -1.0f } };
// Felsworn / Tyrant (tank)
static constexpr CoaStatWeight CoaStats_14_9[] = { { STATS_TYPE_MELEE_DPS, 28.0f }, { STATS_TYPE_AGILITY, 5.634f }, { STATS_TYPE_STAMINA, 4.0f }, { STATS_TYPE_STRENGTH, 2.27f }, { STATS_TYPE_DEFENSE, 2.1f }, { STATS_TYPE_ATTACK_POWER, 2.0f }, { STATS_TYPE_DODGE, 1.8f }, { STATS_TYPE_PARRY, 1.8f }, { STATS_TYPE_HIT, 1.0f }, { STATS_TYPE_EXPERTISE, 1.0f }, { STATS_TYPE_ARMOR, 1.0f }, { STATS_TYPE_CRIT, 0.7f }, { STATS_TYPE_HASTE, 0.7f }, { STATS_TYPE_ARMOR_PENETRATION, 0.3f } };
// Witch Hunter / Boltslinger (physisch_fern)
static constexpr CoaStatWeight CoaStats_15_10[] = { { STATS_TYPE_RANGED_DPS, 28.0f }, { STATS_TYPE_AGILITY, 2.79f }, { STATS_TYPE_INTELLECT, 2.238f }, { STATS_TYPE_ATTACK_POWER, 2.0f }, { STATS_TYPE_CRIT, 1.876f }, { STATS_TYPE_HASTE, 1.2f }, { STATS_TYPE_SPELL_POWER, 1.0f }, { STATS_TYPE_ARMOR_PENETRATION, 0.6f } };
// Witch Hunter / Black Knight (tank)
static constexpr CoaStatWeight CoaStats_15_97[] = { { STATS_TYPE_MELEE_DPS, 28.0f }, { STATS_TYPE_AGILITY, 6.652f }, { STATS_TYPE_STRENGTH, 2.45f }, { STATS_TYPE_DEFENSE, 2.1f }, { STATS_TYPE_STAMINA, 2.0f }, { STATS_TYPE_ATTACK_POWER, 2.0f }, { STATS_TYPE_DODGE, 1.8f }, { STATS_TYPE_PARRY, 1.8f }, { STATS_TYPE_SPELL_POWER, 1.5f }, { STATS_TYPE_HIT, 1.0f }, { STATS_TYPE_EXPERTISE, 1.0f }, { STATS_TYPE_ARMOR, 0.8f }, { STATS_TYPE_CRIT, 0.7f }, { STATS_TYPE_HASTE, 0.7f }, { STATS_TYPE_INTELLECT, 0.54f }, { STATS_TYPE_ARMOR_PENETRATION, 0.3f } };
// Witch Hunter / Houndmaster (physisch_fern)
static constexpr CoaStatWeight CoaStats_15_11[] = { { STATS_TYPE_RANGED_DPS, 28.0f }, { STATS_TYPE_AGILITY, 2.75f }, { STATS_TYPE_ATTACK_POWER, 2.0f }, { STATS_TYPE_CRIT, 1.782f }, { STATS_TYPE_HASTE, 1.2f }, { STATS_TYPE_SPELL_POWER, 1.0f }, { STATS_TYPE_INTELLECT, 0.672f }, { STATS_TYPE_ARMOR_PENETRATION, 0.6f } };
// Witch Hunter / Inquisition (physisch_nah)
static constexpr CoaStatWeight CoaStats_15_12[] = { { STATS_TYPE_MELEE_DPS, 28.0f }, { STATS_TYPE_AGILITY, 3.49f }, { STATS_TYPE_INTELLECT, 2.164f }, { STATS_TYPE_STRENGTH, 2.0f }, { STATS_TYPE_ATTACK_POWER, 2.0f }, { STATS_TYPE_SPELL_POWER, 1.5f }, { STATS_TYPE_CRIT, 1.288f }, { STATS_TYPE_HASTE, 1.2f }, { STATS_TYPE_EXPERTISE, 1.0f }, { STATS_TYPE_ARMOR_PENETRATION, 0.6f } };
// Stormbringer / Lightning (zauber)
static constexpr CoaStatWeight CoaStats_16_15[] = { { STATS_TYPE_CRIT, 3.126f }, { STATS_TYPE_SPELL_POWER, 2.0f }, { STATS_TYPE_HASTE, 1.2f }, { STATS_TYPE_HIT, 1.0f }, { STATS_TYPE_INTELLECT, 0.418f }, { STATS_TYPE_SPELL_PENETRATION, 0.02f }, { STATS_TYPE_ATTACK_POWER, -1.0f } };
// Stormbringer / Maelstrom (zauber)
static constexpr CoaStatWeight CoaStats_16_14[] = { { STATS_TYPE_SPELL_POWER, 2.0f }, { STATS_TYPE_CRIT, 1.764f }, { STATS_TYPE_HASTE, 1.2f }, { STATS_TYPE_INTELLECT, 0.488f }, { STATS_TYPE_SPELL_PENETRATION, 0.02f }, { STATS_TYPE_ATTACK_POWER, -1.0f } };
// Stormbringer / Wind (zauber)
static constexpr CoaStatWeight CoaStats_16_13[] = { { STATS_TYPE_SPELL_POWER, 2.0f }, { STATS_TYPE_HASTE, 1.1f }, { STATS_TYPE_HIT, 1.0f }, { STATS_TYPE_CRIT, 1.0f }, { STATS_TYPE_INTELLECT, 0.134f }, { STATS_TYPE_ATTACK_POWER, -1.0f } };
// Knight of Xoroth / War (physisch_nah)
static constexpr CoaStatWeight CoaStats_17_18[] = { { STATS_TYPE_MELEE_DPS, 28.0f }, { STATS_TYPE_STRENGTH, 4.796f }, { STATS_TYPE_CRIT, 2.048f }, { STATS_TYPE_ATTACK_POWER, 2.0f }, { STATS_TYPE_AGILITY, 1.442f }, { STATS_TYPE_HASTE, 1.2f }, { STATS_TYPE_EXPERTISE, 1.0f }, { STATS_TYPE_ARMOR_PENETRATION, 0.9f } };
// Knight of Xoroth / Defiance (tank)
static constexpr CoaStatWeight CoaStats_17_17[] = { { STATS_TYPE_MELEE_DPS, 28.0f }, { STATS_TYPE_STRENGTH, 5.098f }, { STATS_TYPE_AGILITY, 2.254f }, { STATS_TYPE_STAMINA, 2.12f }, { STATS_TYPE_DEFENSE, 2.1f }, { STATS_TYPE_ATTACK_POWER, 2.0f }, { STATS_TYPE_DODGE, 1.8f }, { STATS_TYPE_PARRY, 1.8f }, { STATS_TYPE_SPELL_POWER, 1.5f }, { STATS_TYPE_BLOCK_RATING, 1.5f }, { STATS_TYPE_HIT, 1.0f }, { STATS_TYPE_EXPERTISE, 1.0f }, { STATS_TYPE_CRIT, 0.752f }, { STATS_TYPE_HASTE, 0.7f }, { STATS_TYPE_BLOCK_VALUE, 0.52f }, { STATS_TYPE_ARMOR, 0.4f }, { STATS_TYPE_ARMOR_PENETRATION, 0.3f }, { STATS_TYPE_INTELLECT, 0.128f } };
// Knight of Xoroth / Hellfire (zauber)
static constexpr CoaStatWeight CoaStats_17_16[] = { { STATS_TYPE_MELEE_DPS, 28.0f }, { STATS_TYPE_STRENGTH, 4.4f }, { STATS_TYPE_INTELLECT, 2.326f }, { STATS_TYPE_SPELL_POWER, 2.0f }, { STATS_TYPE_CRIT, 1.662f }, { STATS_TYPE_ATTACK_POWER, 1.5f }, { STATS_TYPE_HASTE, 1.2f }, { STATS_TYPE_AGILITY, 1.17f }, { STATS_TYPE_HIT, 1.0f }, { STATS_TYPE_EXPERTISE, 1.0f }, { STATS_TYPE_ARMOR_PENETRATION, 0.6f } };
// Guardian / Gladiator (physisch_nah)
static constexpr CoaStatWeight CoaStats_18_19[] = { { STATS_TYPE_MELEE_DPS, 28.0f }, { STATS_TYPE_STRENGTH, 4.928f }, { STATS_TYPE_ATTACK_POWER, 2.0f }, { STATS_TYPE_CRIT, 1.592f }, { STATS_TYPE_AGILITY, 1.234f }, { STATS_TYPE_HASTE, 1.2f }, { STATS_TYPE_EXPERTISE, 1.0f }, { STATS_TYPE_ARMOR_PENETRATION, 0.9f } };
// Guardian / Inspiration (physisch_nah)
static constexpr CoaStatWeight CoaStats_18_20[] = { { STATS_TYPE_STRENGTH, 4.32f }, { STATS_TYPE_ATTACK_POWER, 2.0f }, { STATS_TYPE_CRIT, 1.2f }, { STATS_TYPE_HASTE, 1.1f }, { STATS_TYPE_HIT, 1.0f }, { STATS_TYPE_EXPERTISE, 1.0f }, { STATS_TYPE_AGILITY, 0.888f }, { STATS_TYPE_ARMOR_PENETRATION, 0.5f } };
// Guardian / Vanguard (tank)
static constexpr CoaStatWeight CoaStats_18_21[] = { { STATS_TYPE_MELEE_DPS, 28.0f }, { STATS_TYPE_STAMINA, 6.0f }, { STATS_TYPE_STRENGTH, 4.0f }, { STATS_TYPE_DEFENSE, 2.1f }, { STATS_TYPE_AGILITY, 2.0f }, { STATS_TYPE_ATTACK_POWER, 2.0f }, { STATS_TYPE_DODGE, 1.8f }, { STATS_TYPE_PARRY, 1.8f }, { STATS_TYPE_BLOCK_RATING, 1.5f }, { STATS_TYPE_HIT, 1.0f }, { STATS_TYPE_EXPERTISE, 1.0f }, { STATS_TYPE_CRIT, 0.7f }, { STATS_TYPE_HASTE, 0.7f }, { STATS_TYPE_BLOCK_VALUE, 0.66f }, { STATS_TYPE_ARMOR, 0.65f }, { STATS_TYPE_ARMOR_PENETRATION, 0.3f } };
// Templar / Crusader (physisch_nah)
static constexpr CoaStatWeight CoaStats_19_24[] = { { STATS_TYPE_MELEE_DPS, 28.0f }, { STATS_TYPE_AGILITY, 3.426f }, { STATS_TYPE_STRENGTH, 2.1f }, { STATS_TYPE_ATTACK_POWER, 2.0f }, { STATS_TYPE_SPELL_POWER, 2.0f }, { STATS_TYPE_CRIT, 1.478f }, { STATS_TYPE_HASTE, 1.2f }, { STATS_TYPE_EXPERTISE, 1.0f }, { STATS_TYPE_ARMOR_PENETRATION, 0.6f }, { STATS_TYPE_INTELLECT, 0.43f } };
// Templar / Oathkeeper (tank)
static constexpr CoaStatWeight CoaStats_19_22[] = { { STATS_TYPE_MELEE_DPS, 28.0f }, { STATS_TYPE_AGILITY, 5.744f }, { STATS_TYPE_STRENGTH, 2.666f }, { STATS_TYPE_STAMINA, 2.1f }, { STATS_TYPE_DEFENSE, 2.1f }, { STATS_TYPE_ATTACK_POWER, 2.0f }, { STATS_TYPE_DODGE, 1.8f }, { STATS_TYPE_PARRY, 1.8f }, { STATS_TYPE_HIT, 1.0f }, { STATS_TYPE_EXPERTISE, 1.0f }, { STATS_TYPE_ARMOR, 0.8f }, { STATS_TYPE_CRIT, 0.7f }, { STATS_TYPE_HASTE, 0.7f }, { STATS_TYPE_INTELLECT, 0.4f }, { STATS_TYPE_ARMOR_PENETRATION, 0.3f } };
// Templar / Zealot (physisch_nah)
static constexpr CoaStatWeight CoaStats_19_23[] = { { STATS_TYPE_MELEE_DPS, 28.0f }, { STATS_TYPE_AGILITY, 2.8f }, { STATS_TYPE_STRENGTH, 2.2f }, { STATS_TYPE_ATTACK_POWER, 2.0f }, { STATS_TYPE_INTELLECT, 1.4f }, { STATS_TYPE_SPELL_POWER, 1.4f }, { STATS_TYPE_CRIT, 1.4f }, { STATS_TYPE_HASTE, 1.0f }, { STATS_TYPE_EXPERTISE, 1.0f }, { STATS_TYPE_ARMOR_PENETRATION, 0.2f } };
// Bloodmage / Sanguine (zauber)
static constexpr CoaStatWeight CoaStats_20_26[] = { { STATS_TYPE_SPELL_POWER, 2.0f }, { STATS_TYPE_CRIT, 1.8f }, { STATS_TYPE_HASTE, 1.2f }, { STATS_TYPE_SPIRIT, 0.54f }, { STATS_TYPE_STAMINA, 0.54f }, { STATS_TYPE_INTELLECT, 0.33f }, { STATS_TYPE_SPELL_PENETRATION, 0.02f }, { STATS_TYPE_ATTACK_POWER, -1.0f } };
// Bloodmage / Accursed (physisch_nah)
static constexpr CoaStatWeight CoaStats_20_27[] = { { STATS_TYPE_MELEE_DPS, 28.0f }, { STATS_TYPE_AGILITY, 3.894f }, { STATS_TYPE_STRENGTH, 2.808f }, { STATS_TYPE_ATTACK_POWER, 2.0f }, { STATS_TYPE_SPELL_POWER, 1.5f }, { STATS_TYPE_CRIT, 1.306f }, { STATS_TYPE_HASTE, 1.2f }, { STATS_TYPE_HIT, 1.0f }, { STATS_TYPE_EXPERTISE, 1.0f }, { STATS_TYPE_ARMOR_PENETRATION, 0.6f }, { STATS_TYPE_INTELLECT, 0.238f } };
// Bloodmage / Eternal (tank)
static constexpr CoaStatWeight CoaStats_20_99[] = { { STATS_TYPE_MELEE_DPS, 28.0f }, { STATS_TYPE_AGILITY, 5.934f }, { STATS_TYPE_STRENGTH, 2.354f }, { STATS_TYPE_STAMINA, 2.16f }, { STATS_TYPE_DEFENSE, 2.1f }, { STATS_TYPE_ATTACK_POWER, 2.0f }, { STATS_TYPE_DODGE, 1.8f }, { STATS_TYPE_PARRY, 1.8f }, { STATS_TYPE_HIT, 1.0f }, { STATS_TYPE_EXPERTISE, 1.0f }, { STATS_TYPE_CRIT, 0.728f }, { STATS_TYPE_HASTE, 0.7f }, { STATS_TYPE_SPELL_POWER, 0.5f }, { STATS_TYPE_ARMOR, 0.4f }, { STATS_TYPE_ARMOR_PENETRATION, 0.3f }, { STATS_TYPE_INTELLECT, 0.134f } };
// Bloodmage / Fleshweaver (heilung)
static constexpr CoaStatWeight CoaStats_20_25[] = { { STATS_TYPE_SPELL_POWER, 2.0f }, { STATS_TYPE_SPIRIT, 1.242f }, { STATS_TYPE_HASTE, 1.2f }, { STATS_TYPE_CRIT, 1.0f }, { STATS_TYPE_INTELLECT, 0.182f }, { STATS_TYPE_ATTACK_POWER, -1.0f } };
// Ranger / Archery (physisch_fern)
static constexpr CoaStatWeight CoaStats_21_28[] = { { STATS_TYPE_RANGED_DPS, 28.0f }, { STATS_TYPE_AGILITY, 2.89f }, { STATS_TYPE_ATTACK_POWER, 2.0f }, { STATS_TYPE_CRIT, 1.262f }, { STATS_TYPE_HASTE, 1.2f }, { STATS_TYPE_HIT, 1.0f }, { STATS_TYPE_ARMOR_PENETRATION, 0.6f }, { STATS_TYPE_SPELL_POWER, 0.3f }, { STATS_TYPE_INTELLECT, 0.108f } };
// Ranger / Brigand (physisch_nah)
static constexpr CoaStatWeight CoaStats_21_30[] = { { STATS_TYPE_MELEE_DPS, 28.0f }, { STATS_TYPE_AGILITY, 3.134f }, { STATS_TYPE_STRENGTH, 2.0f }, { STATS_TYPE_ATTACK_POWER, 2.0f }, { STATS_TYPE_CRIT, 1.41f }, { STATS_TYPE_HASTE, 1.2f }, { STATS_TYPE_HIT, 1.0f }, { STATS_TYPE_EXPERTISE, 1.0f }, { STATS_TYPE_ARMOR_PENETRATION, 0.9f } };
// Ranger / Farstrider (physisch_fern)
static constexpr CoaStatWeight CoaStats_21_29[] = { { STATS_TYPE_AGILITY, 2.78f }, { STATS_TYPE_ATTACK_POWER, 2.0f }, { STATS_TYPE_HASTE, 1.1f }, { STATS_TYPE_CRIT, 1.06f }, { STATS_TYPE_HIT, 1.0f }, { STATS_TYPE_ARMOR_PENETRATION, 0.4f } };
// Chronomancer / Infinite (zauber)
static constexpr CoaStatWeight CoaStats_22_32[] = { { STATS_TYPE_SPELL_POWER, 2.0f }, { STATS_TYPE_CRIT, 1.5f }, { STATS_TYPE_HASTE, 1.2f }, { STATS_TYPE_SPIRIT, 0.92f }, { STATS_TYPE_INTELLECT, 0.192f }, { STATS_TYPE_SPELL_PENETRATION, 0.02f }, { STATS_TYPE_ATTACK_POWER, -1.0f } };
// Chronomancer / Artificer (zauber)
static constexpr CoaStatWeight CoaStats_22_33[] = { { STATS_TYPE_RANGED_DPS, 28.0f }, { STATS_TYPE_SPIRIT, 3.0f }, { STATS_TYPE_AGILITY, 2.2f }, { STATS_TYPE_ATTACK_POWER, 2.0f }, { STATS_TYPE_HASTE, 1.8f }, { STATS_TYPE_SPELL_POWER, 1.4f }, { STATS_TYPE_CRIT, 1.4f }, { STATS_TYPE_SPELL_PENETRATION, 1.0f }, { STATS_TYPE_INTELLECT, 0.2f } };
// Chronomancer / Time (heilung)
static constexpr CoaStatWeight CoaStats_22_31[] = { { STATS_TYPE_SPELL_POWER, 2.0f }, { STATS_TYPE_HASTE, 1.3f }, { STATS_TYPE_SPIRIT, 1.15f }, { STATS_TYPE_CRIT, 1.0f }, { STATS_TYPE_INTELLECT, 0.128f }, { STATS_TYPE_ATTACK_POWER, -1.0f } };
// Necromancer / Animation (zauber)
static constexpr CoaStatWeight CoaStats_23_35[] = { { STATS_TYPE_SPELL_POWER, 2.0f }, { STATS_TYPE_CRIT, 1.2f }, { STATS_TYPE_HASTE, 1.2f }, { STATS_TYPE_HIT, 1.0f }, { STATS_TYPE_INTELLECT, 0.102f }, { STATS_TYPE_SPELL_PENETRATION, 0.02f }, { STATS_TYPE_ATTACK_POWER, -1.0f } };
// Necromancer / Death (zauber)
static constexpr CoaStatWeight CoaStats_23_34[] = { { STATS_TYPE_SPELL_POWER, 2.0f }, { STATS_TYPE_CRIT, 1.26f }, { STATS_TYPE_HASTE, 1.2f }, { STATS_TYPE_INTELLECT, 0.108f }, { STATS_TYPE_SPELL_PENETRATION, 0.02f }, { STATS_TYPE_ATTACK_POWER, -1.0f } };
// Necromancer / Rime (zauber)
static constexpr CoaStatWeight CoaStats_23_36[] = { { STATS_TYPE_SPELL_POWER, 2.0f }, { STATS_TYPE_CRIT, 1.8f }, { STATS_TYPE_HASTE, 1.2f }, { STATS_TYPE_INTELLECT, 0.83f }, { STATS_TYPE_SPELL_PENETRATION, 0.02f }, { STATS_TYPE_ATTACK_POWER, -1.0f } };
// Pyromancer / Draconic (zauber)
static constexpr CoaStatWeight CoaStats_24_39[] = { { STATS_TYPE_CRIT, 3.228f }, { STATS_TYPE_SPELL_POWER, 2.0f }, { STATS_TYPE_HASTE, 1.2f }, { STATS_TYPE_HIT, 1.0f }, { STATS_TYPE_INTELLECT, 0.436f }, { STATS_TYPE_SPELL_PENETRATION, 0.02f }, { STATS_TYPE_ATTACK_POWER, -1.0f } };
// Pyromancer / Flameweaving (heilung)
static constexpr CoaStatWeight CoaStats_24_37[] = { { STATS_TYPE_SPIRIT, 4.364f }, { STATS_TYPE_SPELL_POWER, 2.0f }, { STATS_TYPE_CRIT, 1.8f }, { STATS_TYPE_HASTE, 1.3f }, { STATS_TYPE_INTELLECT, 0.258f }, { STATS_TYPE_ATTACK_POWER, -1.0f } };
// Pyromancer / Incineration (zauber)
static constexpr CoaStatWeight CoaStats_24_38[] = { { STATS_TYPE_CRIT, 2.484f }, { STATS_TYPE_SPELL_POWER, 2.0f }, { STATS_TYPE_HASTE, 1.2f }, { STATS_TYPE_HIT, 1.0f }, { STATS_TYPE_INTELLECT, 0.69f }, { STATS_TYPE_SPELL_PENETRATION, 0.02f }, { STATS_TYPE_ATTACK_POWER, -1.0f } };
// Cultist / Godblade (physisch_nah)
static constexpr CoaStatWeight CoaStats_25_42[] = { { STATS_TYPE_MELEE_DPS, 28.0f }, { STATS_TYPE_STRENGTH, 4.536f }, { STATS_TYPE_INTELLECT, 2.542f }, { STATS_TYPE_ATTACK_POWER, 2.0f }, { STATS_TYPE_CRIT, 1.594f }, { STATS_TYPE_HASTE, 1.2f }, { STATS_TYPE_HIT, 1.0f }, { STATS_TYPE_EXPERTISE, 1.0f }, { STATS_TYPE_ARMOR_PENETRATION, 0.6f }, { STATS_TYPE_SPELL_POWER, 0.5f } };
// Cultist / Corruption (zauber)
static constexpr CoaStatWeight CoaStats_25_41[] = { { STATS_TYPE_SPELL_POWER, 2.0f }, { STATS_TYPE_CRIT, 1.8f }, { STATS_TYPE_HASTE, 1.2f }, { STATS_TYPE_INTELLECT, 1.014f }, { STATS_TYPE_SPELL_PENETRATION, 0.02f }, { STATS_TYPE_ATTACK_POWER, -1.0f } };
// Cultist / Dreadnought (tank)
static constexpr CoaStatWeight CoaStats_25_96[] = { { STATS_TYPE_MELEE_DPS, 28.0f }, { STATS_TYPE_STRENGTH, 5.062f }, { STATS_TYPE_BLOCK_RATING, 3.6f }, { STATS_TYPE_STAMINA, 2.24f }, { STATS_TYPE_DEFENSE, 2.1f }, { STATS_TYPE_ATTACK_POWER, 2.0f }, { STATS_TYPE_DODGE, 1.8f }, { STATS_TYPE_PARRY, 1.8f }, { STATS_TYPE_SPELL_POWER, 1.0f }, { STATS_TYPE_HIT, 1.0f }, { STATS_TYPE_EXPERTISE, 1.0f }, { STATS_TYPE_CRIT, 0.7f }, { STATS_TYPE_HASTE, 0.7f }, { STATS_TYPE_ARMOR, 0.51f }, { STATS_TYPE_BLOCK_VALUE, 0.48f }, { STATS_TYPE_ARMOR_PENETRATION, 0.3f }, { STATS_TYPE_INTELLECT, 0.2f } };
// Cultist / Heretic (heilung)
static constexpr CoaStatWeight CoaStats_25_40[] = { { STATS_TYPE_MELEE_DPS, 28.0f }, { STATS_TYPE_CRIT, 6.0f }, { STATS_TYPE_STRENGTH, 2.0f }, { STATS_TYPE_ATTACK_POWER, 2.0f }, { STATS_TYPE_SPELL_POWER, 0.8f }, { STATS_TYPE_HASTE, 0.8f }, { STATS_TYPE_ARMOR_PENETRATION, 0.4f }, { STATS_TYPE_INTELLECT, 0.208f } };
// Starcaller / Sentinel (zauber)
static constexpr CoaStatWeight CoaStats_26_44[] = { { STATS_TYPE_RANGED_DPS, 28.0f }, { STATS_TYPE_INTELLECT, 4.2f }, { STATS_TYPE_AGILITY, 2.732f }, { STATS_TYPE_ATTACK_POWER, 2.0f }, { STATS_TYPE_CRIT, 1.556f }, { STATS_TYPE_ARMOR_PENETRATION, 0.6f }, { STATS_TYPE_SPELL_POWER, 0.5f }, { STATS_TYPE_HASTE, 0.2f } };
// Starcaller / Moon Guard (tank)
static constexpr CoaStatWeight CoaStats_26_100[] = { { STATS_TYPE_MELEE_DPS, 28.0f }, { STATS_TYPE_AGILITY, 4.294f }, { STATS_TYPE_STRENGTH, 4.082f }, { STATS_TYPE_STAMINA, 2.2f }, { STATS_TYPE_DEFENSE, 2.1f }, { STATS_TYPE_SPELL_POWER, 2.0f }, { STATS_TYPE_DODGE, 1.8f }, { STATS_TYPE_PARRY, 1.8f }, { STATS_TYPE_ATTACK_POWER, 1.5f }, { STATS_TYPE_BLOCK_RATING, 1.5f }, { STATS_TYPE_INTELLECT, 1.442f }, { STATS_TYPE_HIT, 1.0f }, { STATS_TYPE_EXPERTISE, 1.0f }, { STATS_TYPE_CRIT, 0.708f }, { STATS_TYPE_HASTE, 0.7f }, { STATS_TYPE_BLOCK_VALUE, 0.52f }, { STATS_TYPE_ARMOR, 0.4f }, { STATS_TYPE_ARMOR_PENETRATION, 0.3f } };
// Starcaller / Moon Priest (heilung)
static constexpr CoaStatWeight CoaStats_26_43[] = { { STATS_TYPE_INTELLECT, 2.0f }, { STATS_TYPE_SPELL_POWER, 2.0f }, { STATS_TYPE_HASTE, 1.3f }, { STATS_TYPE_CRIT, 1.01f }, { STATS_TYPE_ATTACK_POWER, -1.0f } };
// Starcaller / Warden (physisch_nah)
static constexpr CoaStatWeight CoaStats_26_45[] = { { STATS_TYPE_MELEE_DPS, 28.0f }, { STATS_TYPE_INTELLECT, 4.0f }, { STATS_TYPE_SPELL_POWER, 2.0f }, { STATS_TYPE_AGILITY, 1.658f }, { STATS_TYPE_CRIT, 1.4f }, { STATS_TYPE_HASTE, 1.2f }, { STATS_TYPE_STRENGTH, 1.1f }, { STATS_TYPE_ATTACK_POWER, 1.0f }, { STATS_TYPE_ARMOR_PENETRATION, 0.6f }, { STATS_TYPE_SPELL_PENETRATION, 0.6f } };
// Sun Cleric / Piety (zauber)
static constexpr CoaStatWeight CoaStats_27_46[] = { { STATS_TYPE_SPELL_POWER, 2.0f }, { STATS_TYPE_CRIT, 2.0f }, { STATS_TYPE_HASTE, 1.2f }, { STATS_TYPE_INTELLECT, 0.42f }, { STATS_TYPE_SPELL_PENETRATION, 0.02f }, { STATS_TYPE_ATTACK_POWER, -1.0f } };
// Sun Cleric / Blessings (heilung)
static constexpr CoaStatWeight CoaStats_27_98[] = { { STATS_TYPE_SPELL_POWER, 2.0f }, { STATS_TYPE_HASTE, 1.3f }, { STATS_TYPE_CRIT, 1.0f }, { STATS_TYPE_INTELLECT, 0.872f }, { STATS_TYPE_ATTACK_POWER, -1.0f } };
// Sun Cleric / Seraphim (tank)
static constexpr CoaStatWeight CoaStats_27_48[] = { { STATS_TYPE_MELEE_DPS, 28.0f }, { STATS_TYPE_STRENGTH, 6.106f }, { STATS_TYPE_AGILITY, 2.64f }, { STATS_TYPE_STAMINA, 2.3f }, { STATS_TYPE_INTELLECT, 2.178f }, { STATS_TYPE_DEFENSE, 2.1f }, { STATS_TYPE_SPELL_POWER, 2.0f }, { STATS_TYPE_DODGE, 1.8f }, { STATS_TYPE_PARRY, 1.8f }, { STATS_TYPE_BLOCK_RATING, 1.5f }, { STATS_TYPE_ATTACK_POWER, 1.0f }, { STATS_TYPE_HIT, 1.0f }, { STATS_TYPE_EXPERTISE, 1.0f }, { STATS_TYPE_CRIT, 0.726f }, { STATS_TYPE_HASTE, 0.7f }, { STATS_TYPE_ARMOR, 0.512f }, { STATS_TYPE_BLOCK_VALUE, 0.4f }, { STATS_TYPE_ARMOR_PENETRATION, 0.3f } };
// Sun Cleric / Valkyrie (physisch_nah)
static constexpr CoaStatWeight CoaStats_27_47[] = { { STATS_TYPE_MELEE_DPS, 28.0f }, { STATS_TYPE_STRENGTH, 6.72f }, { STATS_TYPE_ATTACK_POWER, 2.5f }, { STATS_TYPE_CRIT, 1.25f }, { STATS_TYPE_HASTE, 1.2f }, { STATS_TYPE_EXPERTISE, 1.0f }, { STATS_TYPE_AGILITY, 0.89f }, { STATS_TYPE_ARMOR_PENETRATION, 0.6f }, { STATS_TYPE_SPELL_POWER, 0.5f }, { STATS_TYPE_INTELLECT, 0.176f } };
// Tinker / Demolition (physisch_fern)
static constexpr CoaStatWeight CoaStats_28_49[] = { { STATS_TYPE_RANGED_DPS, 28.0f }, { STATS_TYPE_AGILITY, 3.756f }, { STATS_TYPE_INTELLECT, 2.41f }, { STATS_TYPE_ATTACK_POWER, 2.0f }, { STATS_TYPE_SPELL_POWER, 2.0f }, { STATS_TYPE_CRIT, 1.288f }, { STATS_TYPE_HASTE, 1.2f }, { STATS_TYPE_ARMOR_PENETRATION, 0.6f } };
// Tinker / Invention (heilung)
static constexpr CoaStatWeight CoaStats_28_51[] = { { STATS_TYPE_RANGED_DPS, 28.0f }, { STATS_TYPE_SPELL_POWER, 2.0f }, { STATS_TYPE_HASTE, 1.3f }, { STATS_TYPE_CRIT, 1.0f }, { STATS_TYPE_INTELLECT, 0.884f }, { STATS_TYPE_ATTACK_POWER, -1.0f } };
// Tinker / Mechanics (physisch_fern)
static constexpr CoaStatWeight CoaStats_28_50[] = { { STATS_TYPE_RANGED_DPS, 28.0f }, { STATS_TYPE_AGILITY, 2.75f }, { STATS_TYPE_INTELLECT, 2.438f }, { STATS_TYPE_ATTACK_POWER, 2.0f }, { STATS_TYPE_CRIT, 1.41f }, { STATS_TYPE_HASTE, 1.2f }, { STATS_TYPE_ARMOR_PENETRATION, 0.6f }, { STATS_TYPE_SPELL_POWER, 0.3f } };
// Venomancer / Rot (zauber)
static constexpr CoaStatWeight CoaStats_29_54[] = { { STATS_TYPE_CRIT, 2.4f }, { STATS_TYPE_SPELL_POWER, 2.0f }, { STATS_TYPE_HASTE, 1.2f }, { STATS_TYPE_INTELLECT, 1.15f }, { STATS_TYPE_HIT, 1.0f }, { STATS_TYPE_SPELL_PENETRATION, 0.02f }, { STATS_TYPE_ATTACK_POWER, -1.0f } };
// Venomancer / Fortitude (tank)
static constexpr CoaStatWeight CoaStats_29_52[] = { { STATS_TYPE_MELEE_DPS, 28.0f }, { STATS_TYPE_AGILITY, 5.058f }, { STATS_TYPE_DEFENSE, 2.1f }, { STATS_TYPE_STAMINA, 2.0f }, { STATS_TYPE_SPELL_POWER, 2.0f }, { STATS_TYPE_DODGE, 1.8f }, { STATS_TYPE_PARRY, 1.8f }, { STATS_TYPE_STRENGTH, 1.6f }, { STATS_TYPE_ATTACK_POWER, 1.0f }, { STATS_TYPE_CRIT, 0.714f }, { STATS_TYPE_HASTE, 0.7f }, { STATS_TYPE_ARMOR, 0.68f }, { STATS_TYPE_ARMOR_PENETRATION, 0.3f }, { STATS_TYPE_INTELLECT, 0.1f } };
// Venomancer / Stalking (zauber)
static constexpr CoaStatWeight CoaStats_29_53[] = { { STATS_TYPE_INTELLECT, 2.0f }, { STATS_TYPE_SPELL_POWER, 2.0f }, { STATS_TYPE_CRIT, 2.0f }, { STATS_TYPE_HASTE, 1.6f }, { STATS_TYPE_EXPERTISE, 1.0f }, { STATS_TYPE_SPELL_PENETRATION, 1.0f }, { STATS_TYPE_AGILITY, 0.8f }, { STATS_TYPE_ATTACK_POWER, 0.4f } };
// Venomancer / Vizier (heilung)
static constexpr CoaStatWeight CoaStats_29_101[] = { { STATS_TYPE_SPELL_POWER, 2.0f }, { STATS_TYPE_HASTE, 1.3f }, { STATS_TYPE_CRIT, 1.0f }, { STATS_TYPE_INTELLECT, 0.8f }, { STATS_TYPE_ATTACK_POWER, -1.0f } };
// Reaper / Harvest (physisch_nah)
static constexpr CoaStatWeight CoaStats_30_56[] = { { STATS_TYPE_MELEE_DPS, 28.0f }, { STATS_TYPE_STRENGTH, 4.694f }, { STATS_TYPE_ATTACK_POWER, 2.0f }, { STATS_TYPE_CRIT, 1.34f }, { STATS_TYPE_HASTE, 1.2f }, { STATS_TYPE_HIT, 1.0f }, { STATS_TYPE_EXPERTISE, 1.0f }, { STATS_TYPE_AGILITY, 0.942f }, { STATS_TYPE_ARMOR_PENETRATION, 0.9f } };
// Reaper / Domination (tank)
static constexpr CoaStatWeight CoaStats_30_57[] = { { STATS_TYPE_MELEE_DPS, 28.0f }, { STATS_TYPE_STRENGTH, 5.192f }, { STATS_TYPE_AGILITY, 3.06f }, { STATS_TYPE_ARMOR_PENETRATION, 2.7f }, { STATS_TYPE_STAMINA, 2.4f }, { STATS_TYPE_DEFENSE, 2.1f }, { STATS_TYPE_ATTACK_POWER, 2.0f }, { STATS_TYPE_DODGE, 1.8f }, { STATS_TYPE_PARRY, 1.8f }, { STATS_TYPE_HIT, 1.0f }, { STATS_TYPE_EXPERTISE, 1.0f }, { STATS_TYPE_ARMOR, 0.74f }, { STATS_TYPE_CRIT, 0.7f }, { STATS_TYPE_HASTE, 0.7f } };
// Reaper / Soul (physisch_nah)
static constexpr CoaStatWeight CoaStats_30_55[] = { { STATS_TYPE_MELEE_DPS, 28.0f }, { STATS_TYPE_STRENGTH, 4.4f }, { STATS_TYPE_ATTACK_POWER, 2.0f }, { STATS_TYPE_CRIT, 1.378f }, { STATS_TYPE_HASTE, 1.2f }, { STATS_TYPE_EXPERTISE, 1.0f }, { STATS_TYPE_AGILITY, 0.97f }, { STATS_TYPE_ARMOR_PENETRATION, 0.9f } };
// Primalist / Wildwalker (physisch_nah)
static constexpr CoaStatWeight CoaStats_31_59[] = { { STATS_TYPE_MELEE_DPS, 28.0f }, { STATS_TYPE_STRENGTH, 4.4f }, { STATS_TYPE_ATTACK_POWER, 2.0f }, { STATS_TYPE_CRIT, 1.3f }, { STATS_TYPE_HASTE, 1.2f }, { STATS_TYPE_EXPERTISE, 1.0f }, { STATS_TYPE_AGILITY, 0.972f }, { STATS_TYPE_ARMOR_PENETRATION, 0.9f } };
// Primalist / Geomancy (zauber)
static constexpr CoaStatWeight CoaStats_31_95[] = { { STATS_TYPE_SPELL_POWER, 2.0f }, { STATS_TYPE_CRIT, 1.308f }, { STATS_TYPE_HASTE, 1.2f }, { STATS_TYPE_HIT, 1.0f }, { STATS_TYPE_ARMOR_PENETRATION, 1.0f }, { STATS_TYPE_INTELLECT, 0.794f }, { STATS_TYPE_SPELL_PENETRATION, 0.02f }, { STATS_TYPE_ATTACK_POWER, -1.0f } };
// Primalist / Grovekeeper (physisch_fern)
static constexpr CoaStatWeight CoaStats_31_58[] = { { STATS_TYPE_MELEE_DPS, 28.0f }, { STATS_TYPE_STRENGTH, 4.4f }, { STATS_TYPE_ATTACK_POWER, 2.0f }, { STATS_TYPE_HASTE, 1.2f }, { STATS_TYPE_CRIT, 1.0f }, { STATS_TYPE_AGILITY, 0.748f }, { STATS_TYPE_SPELL_POWER, 0.5f }, { STATS_TYPE_ARMOR_PENETRATION, 0.4f }, { STATS_TYPE_INTELLECT, 0.186f } };
// Primalist / Mountain King (tank)
static constexpr CoaStatWeight CoaStats_31_60[] = { { STATS_TYPE_MELEE_DPS, 20.0f }, { STATS_TYPE_STRENGTH, 8.0f }, { STATS_TYPE_STAMINA, 4.0f }, { STATS_TYPE_PARRY, 4.0f }, { STATS_TYPE_HASTE, 3.0f }, { STATS_TYPE_EXPERTISE, 3.0f }, { STATS_TYPE_DEFENSE, 3.0f }, { STATS_TYPE_DODGE, 2.4f }, { STATS_TYPE_HIT, 2.2f }, { STATS_TYPE_ATTACK_POWER, 2.0f }, { STATS_TYPE_AGILITY, 1.6f }, { STATS_TYPE_SPELL_POWER, 1.0f }, { STATS_TYPE_CRIT, 1.0f } };
// Runemaster / Glyphic (zauber)
static constexpr CoaStatWeight CoaStats_32_62[] = { { STATS_TYPE_SPELL_POWER, 2.0f }, { STATS_TYPE_CRIT, 1.38f }, { STATS_TYPE_HASTE, 1.2f }, { STATS_TYPE_SPIRIT, 0.66f }, { STATS_TYPE_INTELLECT, 0.478f }, { STATS_TYPE_SPELL_PENETRATION, 0.02f }, { STATS_TYPE_ATTACK_POWER, -1.0f } };
// Runemaster / Engravement (physisch_nah)
static constexpr CoaStatWeight CoaStats_32_61[] = { { STATS_TYPE_MELEE_DPS, 28.0f }, { STATS_TYPE_AGILITY, 2.858f }, { STATS_TYPE_STRENGTH, 2.25f }, { STATS_TYPE_ATTACK_POWER, 2.0f }, { STATS_TYPE_HASTE, 1.2f }, { STATS_TYPE_SPELL_POWER, 1.0f }, { STATS_TYPE_EXPERTISE, 1.0f }, { STATS_TYPE_CRIT, 0.4f }, { STATS_TYPE_INTELLECT, 0.218f } };
// Runemaster / Riftblade (physisch_nah)
static constexpr CoaStatWeight CoaStats_32_63[] = { { STATS_TYPE_MELEE_DPS, 28.0f }, { STATS_TYPE_AGILITY, 3.286f }, { STATS_TYPE_STRENGTH, 2.0f }, { STATS_TYPE_ATTACK_POWER, 2.0f }, { STATS_TYPE_HASTE, 1.2f }, { STATS_TYPE_EXPERTISE, 1.0f }, { STATS_TYPE_SPELL_POWER, 0.5f }, { STATS_TYPE_CRIT, 0.4f }, { STATS_TYPE_INTELLECT, 0.288f } };

static constexpr CoaSpecStats CoaSpecStatsTable[] =
{
    { 12,   2, "Brutality", "physisch_nah", "twoHand", true, CoaStats_12_2, 9 },
    { 12,   3, "Ancestry", "physisch_nah", "shield", true, CoaStats_12_3, 8 },
    { 12,   1, "Headhunting", "physisch_fern", "twoHand", true, CoaStats_12_1, 6 },
    { 13,   4, "Shadowhunting", "physisch_fern", "dualWield", true, CoaStats_13_4, 7 },
    { 13,   6, "Brewing", "heilung", "caster", true, CoaStats_13_6, 6 },
    { 13,   5, "Voodoo", "zauber", "caster", true, CoaStats_13_5, 7 },
    { 14,   8, "Slayer", "physisch_nah", "dualWield", true, CoaStats_14_8, 8 },
    { 14,   7, "Infernal", "zauber", "caster", true, CoaStats_14_7, 7 },
    { 14,   9, "Tyrant", "tank", "twoHand", true, CoaStats_14_9, 14 },
    { 15,  10, "Boltslinger", "physisch_fern", "any", false, CoaStats_15_10, 8 },
    { 15,  97, "Black Knight", "tank", "twoHand", false, CoaStats_15_97, 16 },
    { 15,  11, "Houndmaster", "physisch_fern", "ranged", false, CoaStats_15_11, 8 },
    { 15,  12, "Inquisition", "physisch_nah", "dualWield", false, CoaStats_15_12, 10 },
    { 16,  15, "Lightning", "zauber", "caster", false, CoaStats_16_15, 7 },
    { 16,  14, "Maelstrom", "zauber", "caster", false, CoaStats_16_14, 6 },
    { 16,  13, "Wind", "zauber", "caster", false, CoaStats_16_13, 6 },
    { 17,  18, "War", "physisch_nah", "twoHand", true, CoaStats_17_18, 8 },
    { 17,  17, "Defiance", "tank", "shield", true, CoaStats_17_17, 18 },
    { 17,  16, "Hellfire", "zauber", "twoHand", true, CoaStats_17_16, 11 },
    { 18,  19, "Gladiator", "physisch_nah", "shield", true, CoaStats_18_19, 8 },
    { 18,  20, "Inspiration", "physisch_nah", "shield", true, CoaStats_18_20, 8 },
    { 18,  21, "Vanguard", "tank", "shield", true, CoaStats_18_21, 16 },
    { 19,  24, "Crusader", "physisch_nah", "twoHand", true, CoaStats_19_24, 10 },
    { 19,  22, "Oathkeeper", "tank", "twoHand", true, CoaStats_19_22, 15 },
    { 19,  23, "Zealot", "physisch_nah", "dualWield", true, CoaStats_19_23, 10 },
    { 20,  26, "Sanguine", "zauber", "caster", true, CoaStats_20_26, 8 },
    { 20,  27, "Accursed", "physisch_nah", "twoHand", true, CoaStats_20_27, 11 },
    { 20,  99, "Eternal", "tank", "any", true, CoaStats_20_99, 16 },
    { 20,  25, "Fleshweaver", "heilung", "any", true, CoaStats_20_25, 6 },
    { 21,  28, "Archery", "physisch_fern", "ranged", false, CoaStats_21_28, 9 },
    { 21,  30, "Brigand", "physisch_nah", "any", true, CoaStats_21_30, 9 },
    { 21,  29, "Farstrider", "physisch_fern", "ranged", false, CoaStats_21_29, 6 },
    { 22,  32, "Infinite", "zauber", "caster", false, CoaStats_22_32, 7 },
    { 22,  33, "Artificer", "zauber", "caster", false, CoaStats_22_33, 9 },
    { 22,  31, "Time", "heilung", "caster", false, CoaStats_22_31, 6 },
    { 23,  35, "Animation", "zauber", "any", false, CoaStats_23_35, 7 },
    { 23,  34, "Death", "zauber", "caster", false, CoaStats_23_34, 6 },
    { 23,  36, "Rime", "zauber", "caster", false, CoaStats_23_36, 6 },
    { 24,  39, "Draconic", "zauber", "caster", false, CoaStats_24_39, 7 },
    { 24,  37, "Flameweaving", "heilung", "caster", false, CoaStats_24_37, 6 },
    { 24,  38, "Incineration", "zauber", "caster", false, CoaStats_24_38, 7 },
    { 25,  42, "Godblade", "physisch_nah", "twoHand", true, CoaStats_25_42, 10 },
    { 25,  41, "Corruption", "zauber", "caster", true, CoaStats_25_41, 6 },
    { 25,  96, "Dreadnought", "tank", "shield", true, CoaStats_25_96, 17 },
    { 25,  40, "Heretic", "heilung", "twoHand", true, CoaStats_25_40, 8 },
    { 26,  44, "Sentinel", "zauber", "ranged", false, CoaStats_26_44, 8 },
    { 26, 100, "Moon Guard", "tank", "shield", false, CoaStats_26_100, 18 },
    { 26,  43, "Moon Priest", "heilung", "twoHand", false, CoaStats_26_43, 5 },
    { 26,  45, "Warden", "physisch_nah", "caster", false, CoaStats_26_45, 10 },
    { 27,  46, "Piety", "zauber", "caster", false, CoaStats_27_46, 6 },
    { 27,  98, "Blessings", "heilung", "any", false, CoaStats_27_98, 5 },
    { 27,  48, "Seraphim", "tank", "shield", false, CoaStats_27_48, 18 },
    { 27,  47, "Valkyrie", "physisch_nah", "dualWield", false, CoaStats_27_47, 10 },
    { 28,  49, "Demolition", "physisch_fern", "ranged", false, CoaStats_28_49, 8 },
    { 28,  51, "Invention", "heilung", "ranged", false, CoaStats_28_51, 6 },
    { 28,  50, "Mechanics", "physisch_fern", "caster", false, CoaStats_28_50, 8 },
    { 29,  54, "Rot", "zauber", "caster", true, CoaStats_29_54, 7 },
    { 29,  52, "Fortitude", "tank", "caster", true, CoaStats_29_52, 14 },
    { 29,  53, "Stalking", "zauber", "caster", true, CoaStats_29_53, 8 },
    { 29, 101, "Vizier", "heilung", "caster", true, CoaStats_29_101, 5 },
    { 30,  56, "Harvest", "physisch_nah", "twoHand", true, CoaStats_30_56, 9 },
    { 30,  57, "Domination", "tank", "twoHand", true, CoaStats_30_57, 14 },
    { 30,  55, "Soul", "physisch_nah", "dualWield", true, CoaStats_30_55, 8 },
    { 31,  59, "Wildwalker", "physisch_nah", "twoHand", true, CoaStats_31_59, 8 },
    { 31,  95, "Geomancy", "zauber", "caster", true, CoaStats_31_95, 8 },
    { 31,  58, "Grovekeeper", "physisch_fern", "any", true, CoaStats_31_58, 9 },
    { 31,  60, "Mountain King", "tank", "dualWield", true, CoaStats_31_60, 13 },
    { 32,  62, "Glyphic", "zauber", "twoHand", true, CoaStats_32_62, 7 },
    { 32,  61, "Engravement", "physisch_nah", "dualWield", true, CoaStats_32_61, 9 },
    { 32,  63, "Riftblade", "physisch_nah", "twoHand", true, CoaStats_32_63, 9 },
};

// The weights for a spec, or nullptr when we have none.
static inline CoaSpecStats const* GetCoaSpecStats(uint8_t classId, uint16_t specId)
{
    for (CoaSpecStats const& e : CoaSpecStatsTable)
        if (e.classId == classId && e.specId == specId)
            return &e;

    return nullptr;
}

// The default of a class: its first row, for a bot without a specialization.
static inline CoaSpecStats const* GetCoaDefaultSpecStats(uint8_t classId)
{
    for (CoaSpecStats const& e : CoaSpecStatsTable)
        if (e.classId == classId)
            return &e;

    return nullptr;
}

#endif

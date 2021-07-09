#pragma once

#include "Types.h"

/*!
 * @brief Check if a bit is 1 inside a uint8_t for a specific position
 * @param value uint8_t that will be check
 * @param position Position of the bit that is going to be check
 */
inline bool isBitSet(const uint8_t value, const unsigned short position) {
	return (value & (1 << position)) != 0;
}

/*!
 * @brief Gets a bit inside a uint8_t for a specific position
 * @param value uint8_t value
 * @param position Position of the bit that is going to be get
 */
inline uint8_t getBitValue(const uint8_t inData, const unsigned short position) {
	return (inData >> position) & 1;
}

/*!
 * @brief Sets a bit in a uint8_t. If the bit is 1, it doesn't change.
 * @param *value pointer to the uint8_t that is going to be changed
 * @param position Position of the bit that is going to be set
 */
inline void setBit(uint8_t *value, const unsigned short position) {
    uint8_t mask = 1 << position;
    *value = *value | mask;
}

/*!
 * @brief Toggle a bit in a uint8_t. If the bit is 1, it changes to 0, if it is 0, then it changes to 1.
 * @param *value pointer to the uint8_t that is going to be changed
 * @param position Position of the bit that is going to be toggle
 */
inline void toggleBit(uint8_t *value, const unsigned short position) {
    *value = *value ^ (1 << position);
}

/*!
 * @brief Clear a bit in a uint8_t. The bit's value will be always 0 after this operation.
 * @param *value pointer to the uint8_t that is going to be changed
 * @param position Position of the bit that is going to be set
 */
inline void clearBit(uint8_t *value, const unsigned short position) {
    *value = *value & ~(1 << position);
}
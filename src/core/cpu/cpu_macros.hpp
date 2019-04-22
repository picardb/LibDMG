#ifndef LIBDMG_CPU_MACROS_HPP
#define LIBDMG_CPU_MACROS_HPP

// HELPER MACROS
#define IS_ZERO(a) ((a) == 0 ? (true) : (false))
#define IS_CARRY2(a, b) ((uint16_t)(a) + (uint16_t)(b) > 256 ? (true) : (false))
#define IS_CARRY3(a, b, c) ((uint16_t)(a) + (uint16_t)(b) + (uint16_t)(c) > 256 ? (true) : (false))
#define IS_HALF_CARRY2(a, b) (((a)&0x0F) + ((b)&0x0F) > 16 ? (true) : (false))
#define IS_HALF_CARRY3(a, b, c) (((a)&0x0F) + ((b)&0x0F) + ((c)&0x0F) > 16 ? (true) : (false))
#define IS_BORROW2(a, b) ((a) < (b) ? (true) : (false))
#define IS_BORROW3(a, b, c) ((uint16_t)(a) < (uint16_t)(b) + (uint16_t)(c) ? (true) : (false))
#define IS_HALF_BORROW2(a, b) (((a)&0x0F) < ((b)&0x0F) ? (true) : (false))
#define IS_HALF_BORROW3(a, b, c) (((a)&0x0F) < ((b)&0x0F) + ((c)&0x0F) ? (true) : (false))
#define FLAG_TO_UINT(a) ((a) ? (1) : (0))

#endif // LIBDMG_CPU_MACROS_HPP

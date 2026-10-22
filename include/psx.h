#include "psx.h"
#include <stdint.h>
#include <stddef.h>
#include <string.h>

/*
 * PS1 memory address translation helpers.
 * Preserve physical device addresses before checking RAM,
 * scratchpad, I/O registers, or BIOS.
 */

static inline uint32_t psx_normalize_addr(uint32_t addr)
{
    return addr & 0x1FFFFFFFU;
}

static inline uint32_t psx_translate_ram(
    const PsxState *psx, uint32_t addr)
{
    uint32_t phys = psx_normalize_addr(addr);

    if (psx == NULL || psx->ram == NULL ||
        psx->ram_size == 0)
        return UINT32_MAX;

    if (phys < 0x00800000U &&
        psx->ram_size >= 0x00200000U)
        return phys % (uint32_t)psx->ram_size;

    return UINT32_MAX;
}

static inline uint32_t psx_translate_scratchpad(uint32_t addr)
{
    uint32_t phys = psx_normalize_addr(addr);

    if (phys >= 0x1F800000U &&
        phys < 0x1F800000U + PSX_SCRATCHPAD_SIZE)
        return phys - 0x1F800000U;

    return UINT32_MAX;
}

static inline uint32_t psx_translate_io(uint32_t addr)
{
    uint32_t phys = psx_normalize_addr(addr);

    if (phys >= 0x1F801000U &&
        phys < 0x1F801000U + PSX_IO_SIZE)
        return phys - 0x1F801000U;

    return UINT32_MAX;
}

static inline uint32_t psx_translate_bios(uint32_t addr)
{
    uint32_t phys = psx_normalize_addr(addr);

    if (phys >= 0x1FC00000U &&
        phys < 0x1FC00000U + PSX_BIOS_SIZE)
        return phys - 0x1FC00000U;

    return UINT32_MAX;
}

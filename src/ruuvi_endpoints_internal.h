/**
 * Ruuvi Endpoints internal functions.
 *
 * License: BSD-3
 * Author: TheSomeMan
 */

#ifndef RUUVI_ENDPOINTS_INTERNAL_H
#define RUUVI_ENDPOINTS_INTERNAL_H

#include <stdint.h>
#include <math.h>
#include "ruuvi_endpoints.h"

static inline uint8_t
re_be16_get_high_byte (const uint16_t value)
{
    return (uint8_t) ( (value >> RE_BYTE_1_SHIFT) & RE_BYTE_MASK);
}

static inline uint8_t
re_be16_get_low_byte (const uint16_t value)
{
    return (uint8_t) (value & RE_BYTE_MASK);
}

static inline void
re_encode_i16 (
    uint8_t * const p_slot,
    const re_float val,
    const int16_t  invalid_val,
    const re_float max_abs_val,
    const re_float scale_factor)
{
    int16_t coded_val_i16 = invalid_val;

    if (!isnan (val))
    {
        const re_float val_clipped = RE_CLIP (val, -max_abs_val, max_abs_val);
        coded_val_i16              = (int16_t) lrintf (val_clipped * scale_factor);
    }

    const uint16_t coded_val_u16 = (uint16_t) coded_val_i16;
    p_slot[0] |= re_be16_get_high_byte (coded_val_u16);
    p_slot[1] |= re_be16_get_low_byte (coded_val_u16);
}

static inline re_float
re_decode_i16 (const uint8_t * const p_slot, const int16_t invalid_val,
               const re_float scale_factor)
{
    uint16_t coded_val_u16 = 0;
    coded_val_u16 |= ( (uint16_t) p_slot[0]) << RE_BYTE_1_SHIFT;
    coded_val_u16 |= ( (uint16_t) p_slot[1]) << RE_BYTE_0_SHIFT;
    const int16_t coded_val_i16 = (int16_t) coded_val_u16;

    if (invalid_val == coded_val_i16)
    {
        return NAN;
    }

    return (re_float) coded_val_i16 / scale_factor;
}

static inline void
re_encode_u16 (
    uint8_t * const p_slot,
    const re_float val,
    const uint16_t invalid_val,
    const re_float min_val,
    const re_float max_val,
    const re_float scale_factor)
{
    uint16_t coded_val = invalid_val;

    if (!isnan (val))
    {
        const re_float val_clipped = RE_CLIP (val, min_val, max_val);
        coded_val                  = (uint16_t) lrintf ( (val_clipped - min_val) * scale_factor);
    }

    p_slot[0] |= re_be16_get_high_byte (coded_val);
    p_slot[1] |= re_be16_get_low_byte (coded_val);
}

static inline re_float
re_decode_u16 (
    const uint8_t * const p_slot,
    const uint16_t       invalid_val,
    const re_float       min_val,
    const re_float       scale_factor)
{
    uint16_t coded_val = 0;
    coded_val |= ( (uint16_t) p_slot[0]) << RE_BYTE_1_SHIFT;
    coded_val |= ( (uint16_t) p_slot[1]) << RE_BYTE_0_SHIFT;

    if (invalid_val == coded_val)
    {
        return NAN;
    }

    return (re_float) coded_val / scale_factor + min_val;
}

static void
re_encode_u9 (
    uint8_t * const        p_slot,
    uint8_t * const        p_flags,
    const re_bit_offset_t bit9_offset,
    const re_float        val,
    const uint16_t        invalid_val,
    const re_float        min_val,
    const re_float        max_val,
    const re_float        scale_factor)
{
    uint16_t coded_val = invalid_val;

    if (!isnan (val))
    {
        const re_float val_clipped = RE_CLIP (val, min_val, max_val);
        coded_val                  = (uint16_t) lrintf ( (val_clipped - min_val) * scale_factor);
    }

    *p_slot |= re_be16_get_low_byte (coded_val);
    const uint8_t flags = (re_be16_get_high_byte (coded_val) & RE_BIT1_MASK) << bit9_offset;
    *p_flags |= flags;
}

static re_float
re_decode_u9 (
    const uint8_t * const  p_slot,
    const uint8_t * const  p_flags,
    const re_bit_offset_t bit9_offset,
    const uint16_t        invalid_val,
    const re_float        min_val,
    const re_float        scale_factor)
{
    uint16_t coded_val = 0;
    coded_val |= *p_slot & RE_BYTE_MASK;
    const uint16_t bit9_val = (*p_flags >> bit9_offset) & RE_BIT1_MASK;
    coded_val |= bit9_val << RE_BYTE_1_SHIFT;

    if (invalid_val == coded_val)
    {
        return NAN;
    }

    return (re_float) coded_val / scale_factor + min_val;
}

#endif /* RUUVI_ENDPOINTS_INTERNAL_H */

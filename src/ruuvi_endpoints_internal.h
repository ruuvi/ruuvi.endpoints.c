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

/**
 * @brief Coefficients for encoding/decoding signed 16-bit values.
 */
typedef struct re_i16_coeffs_t
{
    const int16_t invalid_val;  /*!< Invalid value, if a coded value is equal to this,
                                     return NAN. */
    const re_float max_abs_val; //!< Maximum absolute value, used for clipping.
    const re_float ratio;       //!< Scale factor, used for scaling the value.
} re_i16_coeffs_t;

/**
 * @brief Coefficients for encoding/decoding unsigned 16-bit values.
 */
typedef struct re_u16_coeffs_t
{
    const uint16_t invalid_val; /*!< Invalid value, if a coded value is equal to this,
                                     return NAN. */
    const re_float min_val;     //!< Minimum value, used for clipping.
    const re_float max_val;     //!< Minimum value, used for clipping.
    const re_float ratio;       //!< Scale factor, used for scaling the value.
} re_u16_coeffs_t;

/**
 * @brief Coefficients for encoding/decoding unsigned 9-bit values.
 */
typedef struct re_u9_coeffs_t
{
    const re_bit_offset_t bit9_offset; /*!< Bit offset for the 9th bit
                                            which is stored in a separate flags byte. */
    const uint16_t invalid_val;        /*!< Invalid value, if a coded value is equal to this,
                                            return NAN. */
    const re_float min_val;            //!< Minimum value, used for clipping.
    const re_float max_val;            //!< Minimum value, used for clipping.
    const re_float ratio;              //!< Scale factor, used for scaling the value.
} re_u9_coeffs_t;

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
re_encode_i16 (uint8_t * const p_slot, const re_i16_coeffs_t * const p_coeffs,
               const re_float val)
{
    int16_t coded_val_i16 = p_coeffs->invalid_val;

    if (!isnan (val))
    {
        const re_float val_clipped = RE_CLIP (val, -p_coeffs->max_abs_val, p_coeffs->max_abs_val);
        coded_val_i16              = (int16_t) lrintf (val_clipped * p_coeffs->ratio);
    }

    const uint16_t coded_val_u16 = (uint16_t) coded_val_i16;
    p_slot[0] |= re_be16_get_high_byte (coded_val_u16);
    p_slot[1] |= re_be16_get_low_byte (coded_val_u16);
}

static inline re_float
re_decode_i16 (const uint8_t * const p_slot, const re_i16_coeffs_t * const p_coeffs)
{
    uint16_t coded_val_u16 = 0;
    coded_val_u16 |= (uint16_t) ( ( (uint16_t) p_slot[0]) << RE_BYTE_1_SHIFT);
    coded_val_u16 |= (uint16_t) ( ( (uint16_t) p_slot[1]) << RE_BYTE_0_SHIFT);
    const int16_t coded_val_i16 = (int16_t) coded_val_u16;

    if (p_coeffs->invalid_val == coded_val_i16)
    {
        return NAN;
    }

    return (re_float) coded_val_i16 / p_coeffs->ratio;
}

static inline void
re_encode_u16 (uint8_t * const p_slot, const re_u16_coeffs_t * const p_coeffs,
               const re_float val)
{
    uint16_t coded_val = p_coeffs->invalid_val;

    if (!isnan (val))
    {
        const re_float val_clipped = RE_CLIP (val, p_coeffs->min_val, p_coeffs->max_val);
        coded_val                  = (uint16_t) lrintf ( (val_clipped - p_coeffs->min_val) *
                                     p_coeffs->ratio);
    }

    p_slot[0] |= re_be16_get_high_byte (coded_val);
    p_slot[1] |= re_be16_get_low_byte (coded_val);
}

static inline re_float
re_decode_u16 (const uint8_t * const p_slot, const re_u16_coeffs_t * const p_coeffs)
{
    uint16_t coded_val = 0;
    coded_val |= (uint16_t) ( ( (uint16_t) p_slot[0]) << RE_BYTE_1_SHIFT);
    coded_val |= (uint16_t) ( ( (uint16_t) p_slot[1]) << RE_BYTE_0_SHIFT);

    if (p_coeffs->invalid_val == coded_val)
    {
        return NAN;
    }

    return (re_float) coded_val / p_coeffs->ratio + p_coeffs->min_val;
}

static void
re_encode_u9 (uint8_t * const p_slot, uint8_t * const p_flags,
              const re_u9_coeffs_t * const p_coeffs, const re_float val)
{
    uint16_t coded_val = p_coeffs->invalid_val;

    if (!isnan (val))
    {
        const re_float val_clipped = RE_CLIP (val, p_coeffs->min_val, p_coeffs->max_val);
        coded_val                  = (uint16_t) lrintf ( (val_clipped - p_coeffs->min_val) *
                                     p_coeffs->ratio);
    }

    *p_slot |= (coded_val >> RE_BIT_1_SHIFT) & RE_BYTE_MASK;

    if (0 != (coded_val & RE_BIT1_MASK))
    {
        *p_flags |= (uint8_t) (1U << p_coeffs->bit9_offset);
    }
}

static re_float
re_decode_u9 (const uint8_t * const p_slot, const uint8_t * const p_flags,
              const re_u9_coeffs_t * const p_coeffs)
{
    uint16_t coded_val = (uint16_t) * p_slot;
    coded_val <<= RE_BIT_1_SHIFT;

    if (0 != ( (*p_flags >> p_coeffs->bit9_offset) & RE_BIT1_MASK))
    {
        coded_val |= 1U;
    }

    if (p_coeffs->invalid_val == coded_val)
    {
        return NAN;
    }

    return (re_float) coded_val / p_coeffs->ratio + p_coeffs->min_val;
}

#endif /* RUUVI_ENDPOINTS_INTERNAL_H */

/*=============================================================================

    This file is part of ARB.

    ARB is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    ARB is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with ARB; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

=============================================================================*/
/******************************************************************************

    Copyright (C) 2012, 2013 Fredrik Johansson

******************************************************************************/

#include "arb.h"

#define BINEXP_LIMIT 64

void
_arb_pow_exp(arb_t z, const arb_t x, int negx, const arb_t y, slong prec)
{
    arb_t t;
    arb_init(t);

    if (negx)
    {
        arb_neg(t, x);
        arb_log(t, t, prec);
    }
    else
        arb_log(t, x, prec);

    arb_mul(t, t, y, prec);
    arb_exp(z, t, prec);
    arb_clear(t);
}

void
arb_pow(arb_t z, const arb_t x, const arb_t y, slong prec)
{
    if (arb_is_zero(y))
    {
        arb_one(z);
        return;
    }

    if (arb_is_zero(x))
    {
        if (arb_is_positive(y))
            arb_zero(z);
        else
            arb_indeterminate(z);
        return;
    }

    if (arb_is_exact(y) && !arf_is_special(arb_midref(x)))
    {
        const arf_struct * ymid = arb_midref(y);

        /* small half-integer or integer */
        if (arf_cmpabs_2exp_si(ymid, BINEXP_LIMIT) < 0 &&
            arf_is_int_2exp_si(ymid, -1))
        {
            fmpz_t e;
            fmpz_init(e);            

            if (arf_is_int(ymid))
            {
                arf_get_fmpz_fixed_si(e, ymid, 0);
                arb_pow_fmpz_binexp(z, x, e, prec);
            }
            else
            {
                arf_get_fmpz_fixed_si(e, ymid, -1);
                arb_sqrt(z, x, prec + fmpz_bits(e));
                arb_pow_fmpz_binexp(z, z, e, prec);
            }

            fmpz_clear(e);
            return;
        }
        else if (arf_is_int(ymid) && arf_sgn(arb_midref(x)) < 0)
        {
            /* use (-x)^n = (-1)^n * x^n to avoid NaNs
               at least at high enough precision */
            int odd = !arf_is_int_2exp_si(ymid, 1);
            _arb_pow_exp(z, x, 1, y, prec);
            if (odd)
                arb_neg(z, z);
            return;
        }
    }

    _arb_pow_exp(z, x, 0, y, prec);
}


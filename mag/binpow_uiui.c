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

    Copyright (C) 2014 Fredrik Johansson

******************************************************************************/

#include "mag.h"

/* bound (1 + 1/m)^n, m > 0, n >= 0 */
void
mag_binpow_uiui(mag_t b, ulong m, ulong n)
{
    mag_t t;

    if (m == 0)
    {
        mag_inf(b);
        return;
    }

    mag_init(t);

    /* bound by exp(n/m) <= 1 + (n/m) + (n/m)^2 */
    if (m > n)
    {
        mag_set_ui(t, n);   /* x = n/m */
        mag_div_ui(t, t, m);

        mag_mul(b, t, t);   /* x^2 */
        mag_add(b, b, t);   /* x */
        mag_one(t);
        mag_add(b, b, t);   /* 1 */
    }
    else
    {
        mag_one(b);
        mag_div_ui(b, b, m);
        mag_one(t);
        mag_add(t, t, b);
        mag_pow_ui(b, t, n);
    }

    mag_clear(t);
}


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

    Copyright (C) 2015 Fredrik Johansson

******************************************************************************/

#include "arb.h"

void
arb_acosh(arb_t z, const arb_t x, slong prec)
{
    if (arb_is_one(x))
    {
        arb_zero(z);
    }
    else
    {
        arb_t t;
        arb_init(t);

        arb_mul(t, x, x, prec + 4);
        arb_sub_ui(t, t, 1, prec + 4);
        arb_sqrt(t, t, prec + 4);
        arb_add(t, t, x, prec + 4);
        arb_log(z, t, prec);

        arb_clear(t);
    }
}


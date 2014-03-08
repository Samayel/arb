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

#include "ufloat.h"

int main()
{
    long iter;
    flint_rand_t state;

    printf("add_2exp....");
    fflush(stdout);

    flint_randinit(state);

    for (iter = 0; iter < 100000; iter++)
    {
        fmpr_t x, y, z, z2, w;
        ufloat_t xb, zb;
        long e;

        fmpr_init(x);
        fmpr_init(y);
        fmpr_init(z);
        fmpr_init(z2);
        fmpr_init(w);

        fmpr_randtest(x, state, 2 + n_randint(state, 200), 15);
        fmpr_abs(x, x);

        e = n_randint(state, 10000) - n_randint(state, 10000);
        fmpr_set_ui_2exp_si(y, 1, e);

        fmpr_add(z, x, y, FMPR_PREC_EXACT, FMPR_RND_DOWN);
        fmpr_mul_ui(z2, z, 1025, UFLOAT_BITS, FMPR_RND_UP);
        fmpr_mul_2exp_si(z2, z2, -10);

        ufloat_set_fmpr(xb, x);
        ufloat_add_2exp(zb, xb, e);
        ufloat_get_fmpr(w, zb);

        UFLOAT_CHECK_BITS(xb)
        UFLOAT_CHECK_BITS(zb)

        if (!(fmpr_cmpabs(z, w) <= 0 && fmpr_cmpabs(w, z2) <= 0))
        {
            printf("FAIL\n\n");
            printf("x = "); fmpr_printd(x, 15); printf("\n\n");
            printf("y = "); fmpr_printd(y, 15); printf("\n\n");
            printf("z = "); fmpr_printd(z, 15); printf("\n\n");
            printf("w = "); fmpr_printd(w, 15); printf("\n\n");
            abort();
        }

        fmpr_clear(x);
        fmpr_clear(y);
        fmpr_clear(z);
        fmpr_clear(z2);
        fmpr_clear(w);
    }

    flint_randclear(state);
    flint_cleanup();
    printf("PASS\n");
    return EXIT_SUCCESS;
}

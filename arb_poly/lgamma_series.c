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

    Copyright (C) 2013 Fredrik Johansson

******************************************************************************/

#include "arb_poly.h"

slong arf_get_si(const arf_t x, arf_rnd_t rnd);

void _arb_poly_lgamma_series_at_one(arb_ptr u, slong len, slong prec);

void arb_gamma_stirling_choose_param(int * reflect, slong * r, slong * n,
    const arb_t x, int use_reflect, int digamma, slong prec);

void _arb_poly_gamma_stirling_eval(arb_ptr res, const arb_t z, slong n, slong num, slong prec);

static __inline__ void
_log_rising_ui_series(arb_ptr t, const arb_t x, slong r, slong len, slong prec)
{
    arb_struct f[2];
    slong rflen;

    arb_init(f);
    arb_init(f + 1);
    arb_set(f, x);
    arb_one(f + 1);

    rflen = FLINT_MIN(len, r + 1);
    _arb_poly_rising_ui_series(t, f, FLINT_MIN(2, len), r, rflen, prec);
    _arb_poly_log_series(t, t, rflen, len, prec);

    arb_clear(f);
    arb_clear(f + 1);
}

void
_arb_poly_lgamma_series(arb_ptr res, arb_srcptr h, slong hlen, slong len, slong prec)
{
    int reflect;
    slong r, n, wp;
    arb_t zr;
    arb_ptr t, u;

    if (!arb_is_positive(h))
    {
        _arb_vec_indeterminate(res, len);
        return;
    }

    hlen = FLINT_MIN(hlen, len);
    wp = prec + FLINT_BIT_COUNT(prec);

    t = _arb_vec_init(len);
    u = _arb_vec_init(len);
    arb_init(zr);

    /* use zeta values at small integers */
    if (arb_is_int(h) && (arf_cmpabs_ui(arb_midref(h), prec / 2) < 0))
    {
        r = arf_get_si(arb_midref(h), ARF_RND_DOWN);

        if (r <= 0)
        {
            _arb_vec_indeterminate(res, len);
            goto cleanup;
        }
        else
        {
            _arb_poly_lgamma_series_at_one(u, len, wp);

            if (r != 1)
            {
                arb_one(zr);
                _log_rising_ui_series(t, zr, r - 1, len, wp);
                _arb_vec_add(u, u, t, len, wp);
            }
        }
    }
    else if (len <= 2)
    {
        arb_lgamma(u, h, wp);
        if (len == 2)
            arb_digamma(u + 1, h, wp);
    }
    else
    {
        /* otherwise use Stirling series */
        arb_gamma_stirling_choose_param(&reflect, &r, &n, h, 0, 0, wp);
        arb_add_ui(zr, h, r, wp);
        _arb_poly_gamma_stirling_eval(u, zr, n, len, wp);

        if (r != 0)
        {
            _log_rising_ui_series(t, h, r, len, wp);
            _arb_vec_sub(u, u, t, len, wp);
        }
    }

    /* compose with nonconstant part */
    arb_zero(t);
    _arb_vec_set(t + 1, h + 1, hlen - 1);
    _arb_poly_compose_series(res, u, len, t, hlen, len, prec);

cleanup:
    arb_clear(zr);
    _arb_vec_clear(t, len);
    _arb_vec_clear(u, len);
}

void
arb_poly_lgamma_series(arb_poly_t res, const arb_poly_t f, slong n, slong prec)
{
    arb_poly_fit_length(res, n);

    if (f->length == 0 || n == 0)
        _arb_vec_indeterminate(res->coeffs, n);
    else
        _arb_poly_lgamma_series(res->coeffs, f->coeffs, f->length, n, prec);

    _arb_poly_set_length(res, n);
    _arb_poly_normalise(res);
}


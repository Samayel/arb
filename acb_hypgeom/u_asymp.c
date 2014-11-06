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

#include "acb_hypgeom.h"

/* computes the factors that are independent of n (all are upper bounds) */
void
acb_hypgeom_u_asymp_bound_factors(int * R, mag_t alpha,
    mag_t nu, mag_t sigma, mag_t rho, mag_t zinv,
    const acb_t a, const acb_t b, const acb_t z)
{
    mag_t r, u, zre, zim, zlo, sigma_prime;
    acb_t t;

    mag_init(r);
    mag_init(u);
    mag_init(zre);
    mag_init(zim);
    mag_init(zlo);
    mag_init(sigma_prime);
    acb_init(t);

    /* lower bounds for |re(z)|, |im(z)|, |z| */
    arb_get_mag_lower(zre, acb_realref(z));
    arb_get_mag_lower(zim, acb_imagref(z));
    acb_get_mag_lower(zlo, z); /* todo: hypot */

    /* upper bound for 1/|z| */
    mag_one(u);
    mag_div(zinv, u, zlo);

    /* upper bound for r = |b - 2a| */
    acb_mul_2exp_si(t, a, 1);
    acb_sub(t, b, t, MAG_BITS);
    acb_get_mag(r, t);

    /* determine region */
    *R = 0;

    if (mag_cmp(zlo, r) >= 0)
    {
        int znonneg = arb_is_nonnegative(acb_realref(z));

        if (znonneg && mag_cmp(zre, r) >= 0)
        {
            *R = 1;
        }
        else if (mag_cmp(zim, r) >= 0 || znonneg)
        {
            *R = 2;
        }
        else
        {
            mag_mul_2exp_si(u, r, 1);
            if (mag_cmp(zlo, u) >= 0)
                *R = 3;
        }
    }

    if (R == 0)
    {
        mag_inf(alpha);
        mag_inf(nu);
        mag_inf(sigma);
        mag_inf(rho);
    }
    else
    {
        /* sigma = |(b-2a)/z| */
        mag_mul(sigma, r, zinv);

        /* nu = (1/2 + 1/2 sqrt(1-4 sigma^2))^(-1/2) <= 1 + 2 sigma^2 */
        if (mag_cmp_2exp_si(sigma, -1) <= 0)
        {
            mag_mul(nu, sigma, sigma);
            mag_mul_2exp_si(nu, nu, 1);
            mag_one(u);
            mag_add(nu, nu, u);
        }
        else
        {
            mag_inf(nu);
        }

        /* modified sigma for alpha, beta, rho when in R3 */
        if (*R == 3)
            mag_mul(sigma_prime, sigma, nu);
        else
            mag_set(sigma_prime, sigma);

        /* alpha = 1/(1-sigma') */
        mag_one(alpha);
        mag_sub_lower(alpha, alpha, sigma_prime);
        mag_one(u);
        mag_div(alpha, u, alpha);

        /* rho = |2a^2-2ab+b|/2 + sigma'*(1+sigma'/4)/(1-sigma')^2 */
        mag_mul_2exp_si(rho, sigma_prime, -2);
        mag_one(u);
        mag_add(rho, rho, u);
        mag_mul(rho, rho, sigma_prime);
        mag_mul(rho, rho, alpha);
        mag_mul(rho, rho, alpha);
        acb_sub(t, a, b, MAG_BITS);
        acb_mul(t, t, a, MAG_BITS);
        acb_mul_2exp_si(t, t, 1);
        acb_add(t, t, b, MAG_BITS);
        acb_get_mag(u, t);
        mag_mul_2exp_si(u, u, -1);
        mag_add(rho, rho, u);
    }

    mag_clear(r);
    mag_clear(u);
    mag_clear(zre);
    mag_clear(zim);
    mag_clear(zlo);
    mag_clear(sigma_prime);
    acb_clear(t);
}

void
acb_hypgeom_mag_chi(mag_t chi, ulong n)
{
    mag_t p, q;
    ulong k;

    mag_init(p);
    mag_init(q);

    if (n % 2 == 0)
    {
        mag_one(p);
        mag_one(q);
    }
    else
    {
        /* upper bound for pi/2 */
        mag_set_ui_2exp_si(p, 843314857, -28);
        mag_one(q);
    }

    for (k = n; k >= 2; k -= 2)
    {
        mag_mul_ui(p, p, k);
        mag_mul_ui_lower(q, q, k - 1);
    }

    mag_div(chi, p, q);

    mag_clear(p);
    mag_clear(q);
}

void acb_hypgeom_u_asymp(acb_t res, const acb_t a, const acb_t b,
    const acb_t z, long n, long prec)
{
    mag_t C1, Cn, alpha, nu, sigma, rho, zinv, tmp, err;
    int R;

    if (!acb_is_finite(a) || !acb_is_finite(b) || !acb_is_finite(z))
    {
        acb_indeterminate(res);
        return;
    }

    mag_init(C1);
    mag_init(Cn);
    mag_init(alpha);
    mag_init(nu);
    mag_init(sigma);
    mag_init(rho);
    mag_init(zinv);
    mag_init(tmp);
    mag_init(err);

    acb_hypgeom_u_asymp_bound_factors(&R, alpha, nu,
        sigma, rho, zinv, a, b, z);

    if (R == 0)
    {
        acb_indeterminate(res);
    }
    else
    {
        if (R == 1)
        {
            mag_one(C1);
            mag_one(Cn);
        }
        else
        {
            acb_hypgeom_mag_chi(C1, 1);
            acb_hypgeom_mag_chi(Cn, n);

            if (R == 3)
            {
                mag_mul(tmp, nu, nu);
                mag_mul(tmp, tmp, sigma);

                mag_add(C1, C1, tmp);
                mag_mul(C1, C1, nu);

                mag_mul_ui(tmp, tmp, n);
                mag_add(Cn, Cn, tmp);
                mag_pow_ui(tmp, nu, n);
                mag_mul(Cn, Cn, tmp);
            }
        }

        mag_mul(tmp, C1, rho);
        mag_mul(tmp, tmp, alpha);
        mag_mul(tmp, tmp, zinv);
        mag_mul_2exp_si(tmp, tmp, 1);
        mag_exp(err, tmp);
        mag_mul(err, err, Cn);
        mag_mul(err, err, alpha);
        mag_mul_2exp_si(err, err, 1);

        /* evaluate the sum, naively for now */
        {
            acb_t s, t, u, v, w, ab1;
            long k;

            acb_init(s);
            acb_init(t);
            acb_init(u);
            acb_init(v);
            acb_init(w);
            acb_init(ab1);

            acb_one(t);
            acb_sub(ab1, a, b, prec);
            acb_add_ui(ab1, ab1, 1, prec);

            if (n != 0)
            {
                acb_neg(w, z);
                acb_inv(w, w, prec);
            }

            for (k = 0; k < n && !acb_is_zero(t); k++)
            {
                acb_add(s, s, t, prec);

                acb_add_ui(u, a, k, prec);
                acb_add_ui(v, ab1, k, prec);
                acb_mul(u, u, v, prec);
                acb_mul(t, t, u, prec);
                acb_mul(t, t, w, prec);
                acb_div_ui(t, t, k + 1, prec);
            }

            /* nth term */
            acb_get_mag(tmp, t);
            mag_mul(err, err, tmp);
            acb_add_error_mag(s, err);

            acb_set(res, s);

            acb_clear(s);
            acb_clear(t);
            acb_clear(u);
            acb_clear(v);
            acb_clear(w);
            acb_clear(ab1);
        }
    }

    mag_clear(C1);
    mag_clear(Cn);
    mag_clear(alpha);
    mag_clear(nu);
    mag_clear(sigma);
    mag_clear(rho);
    mag_clear(zinv);
    mag_clear(tmp);
    mag_clear(err);
}


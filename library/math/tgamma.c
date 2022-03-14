/*
 * $Id: math_tgamma.c,v 1.4 2022-03-11 20:04:24 obarthel Exp $
 *
 * :ts=4
 *
 * Portable ISO 'C' (1994) runtime library for the Amiga computer
 * Copyright (c) 2002-2015 by Olaf Barthel <obarthel (at) gmx.net>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   - Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *   - Neither the name of Olaf Barthel nor the names of contributors
 *     may be used to endorse or promote products derived from this
 *     software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 *
 * PowerPC math library based in part on work by Sun Microsystems
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice
 * is preserved.
 */

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

static double neg_gam(double);
static double small_gam(double);
static double smaller_gam(double);
static struct Double large_gam(double);
static struct Double ratfun_gam(double, double);

#define LEFT -.3955078125    /* left boundary for rat. approx */
#define x0 .461632144968362356785    /* xmin - 1 */

#define a0_hi 0.88560319441088874992
#define a0_lo -.00000000000000004996427036469019695
#define P0     6.21389571821820863029017800727e-01
#define P1     2.65757198651533466104979197553e-01
#define P2     5.53859446429917461063308081748e-03
#define P3     1.38456698304096573887145282811e-03
#define P4     2.40659950032711365819348969808e-03
#define Q0     1.45019531250000000000000000000e+00
#define Q1     1.06258521948016171343454061571e+00
#define Q2    -2.07474561943859936441469926649e-01
#define Q3    -1.46734131782005422506287573015e-01
#define Q4     3.07878176156175520361557573779e-02
#define Q5     5.12449347980666221336054633184e-03
#define Q6    -1.76012741431666995019222898833e-03
#define Q7     9.35021023573788935372153030556e-05
#define Q8     6.13275507472443958924745652239e-06
/*
 * Constants for large x approximation (x in [6, Inf])
 * (Accurate to 2.8*10^-19 absolute)
 */
#define lns2pi_hi 0.418945312500000
#define lns2pi_lo -.000006779295327258219670263595
#define Pa0     8.33333333333333148296162562474e-02
#define Pa1    -2.77777777774548123579378966497e-03
#define Pa2     7.93650778754435631476282786423e-04
#define Pa3    -5.95235082566672847950717262222e-04
#define Pa4     8.41428560346653702135821806252e-04
#define Pa5    -1.89773526463879200348872089421e-03
#define Pa6     5.69394463439411649408050664078e-03
#define Pa7    -1.44705562421428915453880392761e-02

static const double zero = 0., one = 1.0, tiny = 1e-300;

static const double p1 = 0x1.555555555553ep-3;
static const double p2 = -0x1.6c16c16bebd93p-9;
static const double p3 = 0x1.1566aaf25de2cp-14;
static const double p4 = -0x1.bbd41c5d26bf1p-20;
static const double p5 = 0x1.6376972bea4d0p-25;
static const double ln2hi = 0x1.62e42fee00000p-1;
static const double ln2lo = 0x1.a39ef35793c76p-33;
static const double lnhuge = 0x1.6602b15b7ecf2p9;
static const double lntiny = -0x1.77af8ebeae354p9;
static const double invln2 = 0x1.71547652b82fep0;

#define N 128

static double	A1 = 	  .08333333333333178827;
static double	A2 = 	  .01250000000377174923;
static double	A3 =	 .002232139987919447809;
static double	A4 =	.0004348877777076145742;

static double logF_head[N+1] = {
        0.,
        .007782140442060381246,
        .015504186535963526694,
        .023167059281547608406,
        .030771658666765233647,
        .038318864302141264488,
        .045809536031242714670,
        .053244514518837604555,
        .060624621816486978786,
        .067950661908525944454,
        .075223421237524235039,
        .082443669210988446138,
        .089612158689760690322,
        .096729626458454731618,
        .103796793681567578460,
        .110814366340264314203,
        .117783035656430001836,
        .124703478501032805070,
        .131576357788617315236,
        .138402322859292326029,
        .145182009844575077295,
        .151916042025732167530,
        .158605030176659056451,
        .165249572895390883786,
        .171850256926518341060,
        .178407657472689606947,
        .184922338493834104156,
        .191394852999565046047,
        .197825743329758552135,
        .204215541428766300668,
        .210564769107350002741,
        .216873938300523150246,
        .223143551314024080056,
        .229374101064877322642,
        .235566071312860003672,
        .241719936886966024758,
        .247836163904594286577,
        .253915209980732470285,
        .259957524436686071567,
        .265963548496984003577,
        .271933715484010463114,
        .277868451003087102435,
        .283768173130738432519,
        .289633292582948342896,
        .295464212893421063199,
        .301261330578199704177,
        .307025035294827830512,
        .312755710004239517729,
        .318453731118097493890,
        .324119468654316733591,
        .329753286372579168528,
        .335355541920762334484,
        .340926586970454081892,
        .346466767346100823488,
        .351976423156884266063,
        .357455888922231679316,
        .362905493689140712376,
        .368325561158599157352,
        .373716409793814818840,
        .379078352934811846353,
        .384411698910298582632,
        .389716751140440464951,
        .394993808240542421117,
        .400243164127459749579,
        .405465108107819105498,
        .410659924985338875558,
        .415827895143593195825,
        .420969294644237379543,
        .426084395310681429691,
        .431173464818130014464,
        .436236766774527495726,
        .441274560805140936281,
        .446287102628048160113,
        .451274644139630254358,
        .456237433481874177232,
        .461175715122408291790,
        .466089729924533457960,
        .470979715219073113985,
        .475845904869856894947,
        .480688529345570714212,
        .485507815781602403149,
        .490303988045525329653,
        .495077266798034543171,
        .499827869556611403822,
        .504556010751912253908,
        .509261901790523552335,
        .513945751101346104405,
        .518607764208354637958,
        .523248143765158602036,
        .527867089620485785417,
        .532464798869114019908,
        .537041465897345915436,
        .541597282432121573947,
        .546132437597407260909,
        .550647117952394182793,
        .555141507540611200965,
        .559615787935399566777,
        .564070138285387656651,
        .568504735352689749561,
        .572919753562018740922,
        .577315365035246941260,
        .581691739635061821900,
        .586049045003164792433,
        .590387446602107957005,
        .594707107746216934174,
        .599008189645246602594,
        .603290851438941899687,
        .607555250224322662688,
        .611801541106615331955,
        .616029877215623855590,
        .620240409751204424537,
        .624433288012369303032,
        .628608659422752680256,
        .632766669570628437213,
        .636907462236194987781,
        .641031179420679109171,
        .645137961373620782978,
        .649227946625615004450,
        .653301272011958644725,
        .657358072709030238911,
        .661398482245203922502,
        .665422632544505177065,
        .669430653942981734871,
        .673422675212350441142,
        .677398823590920073911,
        .681359224807238206267,
        .685304003098281100392,
        .689233281238557538017,
        .693147180560117703862
};

static double logF_tail[N+1] = {
        0.,
        -.00000000000000543229938420049,
        .00000000000000172745674997061,
        -.00000000000001323017818229233,
        -.00000000000001154527628289872,
        -.00000000000000466529469958300,
        .00000000000005148849572685810,
        -.00000000000002532168943117445,
        -.00000000000005213620639136504,
        -.00000000000001819506003016881,
        .00000000000006329065958724544,
        .00000000000008614512936087814,
        -.00000000000007355770219435028,
        .00000000000009638067658552277,
        .00000000000007598636597194141,
        .00000000000002579999128306990,
        -.00000000000004654729747598444,
        -.00000000000007556920687451336,
        .00000000000010195735223708472,
        -.00000000000017319034406422306,
        -.00000000000007718001336828098,
        .00000000000010980754099855238,
        -.00000000000002047235780046195,
        -.00000000000008372091099235912,
        .00000000000014088127937111135,
        .00000000000012869017157588257,
        .00000000000017788850778198106,
        .00000000000006440856150696891,
        .00000000000016132822667240822,
        -.00000000000007540916511956188,
        -.00000000000000036507188831790,
        .00000000000009120937249914984,
        .00000000000018567570959796010,
        -.00000000000003149265065191483,
        -.00000000000009309459495196889,
        .00000000000017914338601329117,
        -.00000000000001302979717330866,
        .00000000000023097385217586939,
        .00000000000023999540484211737,
        .00000000000015393776174455408,
        -.00000000000036870428315837678,
        .00000000000036920375082080089,
        -.00000000000009383417223663699,
        .00000000000009433398189512690,
        .00000000000041481318704258568,
        -.00000000000003792316480209314,
        .00000000000008403156304792424,
        -.00000000000034262934348285429,
        .00000000000043712191957429145,
        -.00000000000010475750058776541,
        -.00000000000011118671389559323,
        .00000000000037549577257259853,
        .00000000000013912841212197565,
        .00000000000010775743037572640,
        .00000000000029391859187648000,
        -.00000000000042790509060060774,
        .00000000000022774076114039555,
        .00000000000010849569622967912,
        -.00000000000023073801945705758,
        .00000000000015761203773969435,
        .00000000000003345710269544082,
        -.00000000000041525158063436123,
        .00000000000032655698896907146,
        -.00000000000044704265010452446,
        .00000000000034527647952039772,
        -.00000000000007048962392109746,
        .00000000000011776978751369214,
        -.00000000000010774341461609578,
        .00000000000021863343293215910,
        .00000000000024132639491333131,
        .00000000000039057462209830700,
        -.00000000000026570679203560751,
        .00000000000037135141919592021,
        -.00000000000017166921336082431,
        -.00000000000028658285157914353,
        -.00000000000023812542263446809,
        .00000000000006576659768580062,
        -.00000000000028210143846181267,
        .00000000000010701931762114254,
        .00000000000018119346366441110,
        .00000000000009840465278232627,
        -.00000000000033149150282752542,
        -.00000000000018302857356041668,
        -.00000000000016207400156744949,
        .00000000000048303314949553201,
        -.00000000000071560553172382115,
        .00000000000088821239518571855,
        -.00000000000030900580513238244,
        -.00000000000061076551972851496,
        .00000000000035659969663347830,
        .00000000000035782396591276383,
        -.00000000000046226087001544578,
        .00000000000062279762917225156,
        .00000000000072838947272065741,
        .00000000000026809646615211673,
        -.00000000000010960825046059278,
        .00000000000002311949383800537,
        -.00000000000058469058005299247,
        -.00000000000002103748251144494,
        -.00000000000023323182945587408,
        -.00000000000042333694288141916,
        -.00000000000043933937969737844,
        .00000000000041341647073835565,
        .00000000000006841763641591466,
        .00000000000047585534004430641,
        .00000000000083679678674757695,
        -.00000000000085763734646658640,
        .00000000000021913281229340092,
        -.00000000000062242842536431148,
        -.00000000000010983594325438430,
        .00000000000065310431377633651,
        -.00000000000047580199021710769,
        -.00000000000037854251265457040,
        .00000000000040939233218678664,
        .00000000000087424383914858291,
        .00000000000025218188456842882,
        -.00000000000003608131360422557,
        -.00000000000050518555924280902,
        .00000000000078699403323355317,
        -.00000000000067020876961949060,
        .00000000000016108575753932458,
        .00000000000058527188436251509,
        -.00000000000035246757297904791,
        -.00000000000018372084495629058,
        .00000000000088606689813494916,
        .00000000000066486268071468700,
        .00000000000063831615170646519,
        .00000000000025144230728376072,
        -.00000000000017239444525614834
};

static double
__exp__D(double x, double c) {
    double z, hi, lo;
    int k;

    if (x != x)    /* x is NaN */
        return (x);
    if (x <= lnhuge) {
        if (x >= lntiny) {

            /* argument reduction : x --> x - k*ln2 */
            z = invln2 * x;
            k = z + copysign(.5, x);

            /* express (x+c)-k*ln2 as hi-lo and let x=hi-lo rounded */

            hi = (x - k * ln2hi);            /* Exact. */
            x = hi - (lo = k * ln2lo - c);
            /* return 2^k*[1+x+x*c/(2+c)]  */
            z = x * x;
            c = x - z * (p1 + z * (p2 + z * (p3 + z * (p4 + z * p5))));
            c = (x * c) / (2.0 - c);

            return scalbn(1. + (hi - (lo - c)), k);
        }
            /* end of x > lntiny */

        else
            /* exp(-big#) underflows to zero */
        if (isfinite(x)) return (scalbn(1.0, -5000));

            /* exp(-INF) is zero */
        else return (0.0);
    }
        /* end of x < lnhuge */

    else
        /* exp(INF) is INF, exp(+big#) overflows to INF */
        return (isfinite(x) ? scalbn(1.0, 5000) : x);
}

static struct Double
__log__D(double x) {
    int m, j;
    double F, f, g, q, u, v, u2;
    volatile double u1;
    struct Double r;

    /* Argument reduction: 1 <= g < 2; x/2^m = g;	*/
    /* y = F*(1 + f/F) for |f| <= 2^-8		*/

    m = logb(x);
    g = ldexp(x, -m);
    if (m == -1022) {
        j = logb(g), m += j;
        g = ldexp(g, -j);
    }
    j = N * (g - 1) + .5;
    F = (1.0 / N) * j + 1;
    f = g - F;

    g = 1 / (2 * F + f);
    u = 2 * f * g;
    v = u * u;
    q = u * v * (A1 + v * (A2 + v * (A3 + v * A4)));
    if (m | j)
        u1 = u + 513, u1 -= 513;
    else
        u1 = u, TRUNC(u1);
    u2 = (2.0 * (f - F * u1) - u1 * f) * g;

    u1 += m * logF_head[N] + logF_head[j];

    u2 += logF_tail[j];
    u2 += q;
    u2 += logF_tail[N] * m;
    r.a = u1 + u2;            /* Only difference is here */
    TRUNC(r.a);
    r.b = (u1 - r.a) + u2;
    return (r);
}


static struct Double
large_gam(double x)
{
    double z, p;
    struct Double t, u, v;

    z = one / (x * x);
    p = Pa0 + z * (Pa1 + z * (Pa2 + z * (Pa3 + z * (Pa4 + z * (Pa5 + z * (Pa6 + z * Pa7))))));
    p = p / x;

    u = __log__D(x);
    u.a -= one;
    v.a = (x -= .5);
    TRUNC(v.a);
    v.b = x - v.a;
    t.a = v.a * u.a;            /* t = (x-.5)*(log(x)-1) */
    t.b = v.b * u.a + x * u.b;
    /* return t.a + t.b + lns2pi_hi + lns2pi_lo + p */
    t.b += lns2pi_lo;
    t.b += p;
    u.a = lns2pi_hi + t.b;
    u.a += t.a;
    u.b = t.a - u.a;
    u.b += lns2pi_hi;
    u.b += t.b;
    return (u);
}
/*
 * Good to < 1 ulp.  (provably .90 ulp; .87 ulp on 1,000,000 runs.)
 * It also has correct monotonicity.
 */
static double
small_gam(double x)
{
    double y, ym1, t;
    struct Double yy, r;
    y = x - one;
    ym1 = y - one;
    if (y <= 1.0 + (LEFT + x0)) {
        yy = ratfun_gam(y - x0, 0);
        return (yy.a + yy.b);
    }
    r.a = y;
    TRUNC(r.a);
    yy.a = r.a - one;
    y = ym1;
    yy.b = r.b = y - yy.a;
    /* Argument reduction: G(x+1) = x*G(x) */
    for (ym1 = y - one; ym1 > LEFT + x0; y = ym1--, yy.a--) {
        t = r.a * yy.a;
        r.b = r.a * yy.b + y * r.b;
        r.a = t;
        TRUNC(r.a);
        r.b += (t - r.a);
    }
    /* Return r*tgamma(y). */
    yy = ratfun_gam(y - x0, 0);
    y = r.b * (yy.a + yy.b) + r.a * yy.b;
    y += yy.a * r.a;
    return (y);
}
/*
 * Good on (0, 1+x0+LEFT].  Accurate to 1ulp.
 */
static double
smaller_gam(double x)
{
    double t, d;
    struct Double r, xx;
    if (x < x0 + LEFT) {
        t = x, TRUNC(t);
        d = (t + x) * (x - t);
        t *= t;
        xx.a = (t + x), TRUNC(xx.a);
        xx.b = x - xx.a;
        xx.b += t;
        xx.b += d;
        t = (one - x0);
        t += x;
        d = (one - x0);
        d -= t;
        d += x;
        x = xx.a + xx.b;
    } else {
        xx.a = x, TRUNC(xx.a);
        xx.b = x - xx.a;
        t = x - x0;
        d = (-x0 - t);
        d += x;
    }
    r = ratfun_gam(t, d);
    d = r.a / x, TRUNC(d);
    r.a -= d * xx.a;
    r.a -= d * xx.b;
    r.a += r.b;
    return (d + r.a / x);
}
/*
 * returns (z+c)^2 * P(z)/Q(z) + a0
 */
static struct Double
ratfun_gam(double z, double c)
{
    double p, q;
    struct Double r, t;

    q = Q0 + z * (Q1 + z * (Q2 + z * (Q3 + z * (Q4 + z * (Q5 + z * (Q6 + z * (Q7 + z * Q8)))))));
    p = P0 + z * (P1 + z * (P2 + z * (P3 + z * P4)));

    /* return r.a + r.b = a0 + (z+c)^2*p/q, with r.a truncated to 26 bits. */
    p = p / q;
    t.a = z, TRUNC(t.a);        /* t ~= z + c */
    t.b = (z - t.a) + c;
    t.b *= (t.a + z);
    q = (t.a *= t.a);        /* t = (z+c)^2 */
    TRUNC(t.a);
    t.b += (q - t.a);
    r.a = p, TRUNC(r.a);        /* r = P/Q */
    r.b = p - r.a;
    t.b = t.b * p + t.a * r.b + a0_lo;
    t.a *= r.a;            /* t = (z+c)^2*(P/Q) */
    r.a = t.a + a0_hi, TRUNC(r.a);
    r.b = ((a0_hi - r.a) + t.a) + t.b;
    return (r);            /* r = a0 + t */
}

static double
neg_gam(double x)
{
    int sgn = 1;
    struct Double lg, lsine;
    double y, z;

    y = ceil(x);
    if (y == x)        /* Negative integer. */
        return ((x - x) / zero);
    z = y - x;
    if (z > 0.5)
        z = one - z;
    y = 0.5 * y;
    if (y == ceil(y))
        sgn = -1;
    if (z < .25)
        z = sin(M_PI * z);
    else
        z = cos(M_PI * (0.5 - z));
    /* Special case: G(1-x) = Inf; G(x) may be nonzero. */
    if (x < -170) {
        if (x < -190)
            return ((double) sgn * tiny * tiny);
        y = one - x;        /* exact: 128 < |x| < 255 */
        lg = large_gam(y);
        lsine = __log__D(M_PI / z);    /* = TRUNC(log(u)) + small */
        lg.a -= lsine.a;        /* exact (opposite signs) */
        lg.b -= lsine.b;
        y = -(lg.a + lg.b);
        z = (y + lg.a) + lg.b;
        y = __exp__D(y, z);
        if (sgn < 0) y = -y;
        return (y);
    }
    y = one - x;
    if (one - y == x)
        y = tgamma(y);
    else        /* 1-x is inexact */
        y = -x * tgamma(-x);
    if (sgn < 0) y = -y;
    return (M_PI / (y * z));
}

double
tgamma(double x) {
    struct Double u;

    if (isgreaterequal(x, 6)) {
        if (x > 171.63)
            return (x / zero);
        u = large_gam(x);
        return (__exp__D(u.a, u.b));
    } else if (isgreaterequal(x, 1.0 + LEFT + x0))
        return (small_gam(x));
    else if (isgreater(x, 1.e-17))
        return (smaller_gam(x));
    else if (isgreater(x, -1.e-17)) {
        if (x != 0.0)
            u.a = one - tiny;    /* raise inexact */
        return (one / x);
    } else if (!isfinite(x))
        return (x - x);        /* x is NaN or -Inf */
    else
        return (neg_gam(x));
}

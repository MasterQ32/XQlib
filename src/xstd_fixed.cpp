#include "../include/xstd/fixed"

using var = xstd::fixed<22, 10>;

static_assert(sizeof(var) == 4);
static_assert(std::is_same_v<var::word_t, int32_t>);

static_assert(var(10.0)  == var(10));
static_assert(var(10U)   == var(10));
static_assert(var(10.0f) == var(10));

static_assert(-var(10) == var(-10));

static_assert(var(10) + var(7.5) == var(17.5));
static_assert(var(10) - var(7.5) == var(2.5));

static_assert(var(10) / var(3) == var(3.3333333333333));
static_assert(var(10) * var(3) == var(30.0));

static_assert(var(10) % var(4) == var(2));
static_assert(var(10.25) % var(0.5) == var(0.25));
static_assert(var(-10) % var(4) == var(-2));
static_assert(var(-10.25) % var(0.5) == var(-0.25));

static_assert((var(2)  & var(8)) == var(0));
static_assert((var(10) & var(8)) == var(8));
static_assert((var(2)  | var(8)) == var(10));
static_assert((var(10) | var(8)) == var(10));
static_assert((var(2)  ^ var(8)) == var(10));
static_assert((var(10) ^ var(8)) == var(2));
static_assert((~var(10)) == var::raw(int32_t(0xFFFFD7FF)));

// one day, we can bitcast here!
// static_assert((var(10) >> 2) == var(2.5));

static_assert(xstd::floor(var(3.0)) == var(3));
static_assert(xstd::floor(var(3.4)) == var(3));
static_assert(xstd::floor(var(-3.4)) == var(-4));

static_assert(xstd::floor(var(4) - var::epsilon()) == var(3));
static_assert(xstd::floor(var(3.6)) == var(3));
static_assert(xstd::floor(var(-3.6)) == var(-4));

static_assert(xstd::ceil(var(3.0)) == var(3));
static_assert(xstd::ceil(var(3.0) + var::epsilon()) == var(4));
static_assert(xstd::ceil(var(3.4)) == var(4));
static_assert(xstd::ceil(var(-3.4)) == var(-3));

static_assert(xstd::ceil(var(3.6)) == var(4));
static_assert(xstd::ceil(var(-3.6)) == var(-3));

static_assert(var::epsilon() == var(1.0 / (1 << var::decimal_bits)));

static_assert(xstd::fract(var(3.0)) == var(0.0));
static_assert(xstd::fract(var(4.0)) == var(0.0));
static_assert(xstd::fract(var(3.4)) == var(0.4));
static_assert(xstd::fract(var(3.6)) == var(0.6));
static_assert(xstd::fract(var(4.0) + var::epsilon()) == var::epsilon());


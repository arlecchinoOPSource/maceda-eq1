#include "filter.hpp"
#include <algorithm>
#include <cmath>
#include <numbers>

namespace maceda {
std::complex<double> Section::response(double omega) const noexcept {
    const auto z = std::polar(1.0, -omega);
    return (b0 + z * (b1 + z * b2)) / (1.0 + z * (a1 + z * a2));
}

Section design(const Band& band, double rate) noexcept {
    if (!band.enabled || !std::isfinite(rate) || rate < 8000 || rate > 384000 ||
        !std::isfinite(band.frequency) || !std::isfinite(band.q) || !std::isfinite(band.gainDb))
        return {};

    const double hz = std::clamp(band.frequency, 20.0, std::min(20000.0, 0.45 * rate));
    const double w = 2 * std::numbers::pi * hz / rate;
    const double c = std::cos(w);
    const double s = std::sin(w);
    const double alpha = s / (2 * std::clamp(band.q, 0.1, 18.0));
    const double gain = std::clamp(band.gainDb, -24.0, 24.0);
    const double a = std::pow(10.0, gain / 40);
    const auto normalize = [](double b0, double b1, double b2, double a0, double a1, double a2) {
        return Section{b0/a0, b1/a0, b2/a0, a1/a0, a2/a0};
    };
    switch (band.shape) {
        case Shape::bell:
            if (gain == 0) return {};
            return normalize(1 + alpha*a, -2*c, 1 - alpha*a,
                             1 + alpha/a, -2*c, 1 - alpha/a);
        case Shape::lowPass:
            return normalize((1-c)/2, 1-c, (1-c)/2, 1+alpha, -2*c, 1-alpha);
        case Shape::highPass:
            return normalize((1+c)/2, -(1+c), (1+c)/2, 1+alpha, -2*c, 1-alpha);
        case Shape::lowShelf:
        case Shape::highShelf: {
            if (gain == 0) return {};
            // Shelf slope S=1. The sign selects complementary high/low forms.
            const double sign = band.shape == Shape::lowShelf ? -1 : 1;
            const double p = a+1, m = a-1;
            const double t = std::sqrt(2*a)*s;
            return normalize(a*(p + sign*m*c + t), -2*a*(sign*m + p*c),
                             a*(p + sign*m*c - t), p - sign*m*c + t,
                             2*(sign*m - p*c), p - sign*m*c - t);
        }
    }
    return {};
}

void Cascade::configure(const Bands& bands, double rate) noexcept {
    active = 0;
    for (const auto& band : bands)
        if (band.enabled)
            stages[active++] = {design(band, rate), 0, 0};
}

double Cascade::process(double x) noexcept {
    for (unsigned i = 0; i < active; ++i) {
        auto& stage = stages[i];
        const auto& k = stage.section;
        const double y = k.b0*x + stage.z1;
        stage.z1 = k.b1*x - k.a1*y + stage.z2;
        stage.z2 = k.b2*x - k.a2*y;
        x = y;
    }
    return x;
}

std::complex<double> Cascade::response(double w) const noexcept {
    std::complex<double> result = 1;
    for (unsigned i = 0; i < active; ++i)
        result *= stages[i].section.response(w);
    return result;
}
}

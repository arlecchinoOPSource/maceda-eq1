#include "filter.hpp"
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <numbers>

using namespace maceda;

void check(bool success, const char* description) {
    if (!success) {
        std::cerr << description << '\n';
        std::exit(EXIT_FAILURE);
    }
}

int main() {
    for (double rate : {44100., 48000., 88200., 96000., 176400., 192000.}) {
        const double omega = 2 * std::numbers::pi * 1000 / rate;
        for (auto shape : {Shape::bell, Shape::lowShelf, Shape::highShelf, Shape::lowPass, Shape::highPass}) {
            Bands bands{};
            bands.back() = {shape, 1000, 12, std::sqrt(0.5), true};
            Cascade left, right;
            left.configure(bands, rate);
            right.configure(bands, rate);
            const double centerDb = shape == Shape::bell ? 12 :
                (shape == Shape::lowShelf || shape == Shape::highShelf ? 6 : -10*std::log10(2));
            check(std::abs(20*std::log10(std::abs(left.response(omega))) - centerDb) < 1e-6, "center gain");
            auto expectedDC = shape == Shape::lowShelf ? std::pow(10., 12./20) : (shape == Shape::highPass ? 0. : 1.);
            auto expectedHF = shape == Shape::highShelf ? std::pow(10., 12./20) : (shape == Shape::lowPass ? 0. : 1.);
            check(std::abs(std::abs(left.response(0)) - expectedDC) < 1e-8, "DC gain");
            check(std::abs(std::abs(left.response(std::numbers::pi)) - expectedHF) < 1e-8, "Nyquist gain");
            std::complex<double> measured{};
            for (int n = 0; n < 16384; ++n) {
                measured += left.process(n == 0 ? 1.0 : 0.0) * std::polar(1.0, -omega*n);
                check(right.process(0) == 0, "channel isolation");
            }
            check(std::abs(measured-left.response(omega)) < 1e-8, "impulse vs graph");
        }
        Bands full{};
        full.fill({Shape::bell, 1000, 1, 1, true});
        Cascade engine;
        engine.configure(full, rate);
        check(std::abs(20*std::log10(std::abs(engine.response(omega))) - 12) < 1e-6, "12-band cascade");
        engine.configure({}, rate);
        check(engine.process(-0.37) == -0.37, "default dry null");
    }
}

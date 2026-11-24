#pragma once

#include <array>
#include <complex>

namespace maceda {
enum class Shape { bell, lowShelf, highShelf, lowPass, highPass };

struct Band {
    Shape shape = Shape::bell;
    double frequency = 1000.0;
    double gainDb = 0.0;
    double q = 0.7071067811865476;
    bool enabled = false;
};

using Bands = std::array<Band, 12>;

struct Section {
    double b0 = 1, b1 = 0, b2 = 0, a1 = 0, a2 = 0;
    std::complex<double> response(double radiansPerSample) const noexcept;
};

Section design(const Band&, double sampleRate) noexcept;

// Static configuration only. Configure while stopped. Each channel owns a Cascade.
class Cascade {
public:
    void configure(const Bands&, double sampleRate) noexcept;
    double process(double input) noexcept;
    std::complex<double> response(double radiansPerSample) const noexcept;
private:
    struct Stage {
        Section section;
        double z1 = 0, z2 = 0;
    };
    std::array<Stage, 12> stages{};
    unsigned active = 0;
};
}

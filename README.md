# Maceda’s EQ-1

A precise 12-band digital parametric EQ for mixing. Version 1.0 targets VST3 on Apple Silicon macOS and Windows x64, plus AUv2 on macOS. Ableton Live 12 is the primary listening and session-recall host.

**Development status** — The initial C++ library implements static bell, low/high shelf and low/high pass sections, a 12-slot cascade and its complex response. The first executable is a numerical regression test. An installable plugin is scheduled for M2.

## Build the foundation

Use CMake 3.22+ and a C++20 compiler (Xcode Command Line Tools on Mac, Visual Studio 2022 Desktop C++ on Windows). Run from the repository root.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel 2
ctest --test-dir build -C Release --output-on-failure
```

The core build runs offline and has no third-party dependencies. Two compile jobs suit a 16 GB laptop. Future JUCE builds will use a separate optional target and a cached, pinned source checkout.

## Product and development

- [Product vision](docs/vision.md) — the mixing workflow and future physical instrument.
- [Architecture decisions](docs/architecture.md) — DSP, host contract, state and threading.
- [Development and release gates](docs/roadmap.md) — evidence required to ship.
- [GitHub milestones](https://github.com/arlecchinoOPSource/maceda-eq1/milestones) and [issues](https://github.com/arlecchinoOPSource/maceda-eq1/issues) track current work.

## Contributing

Start with an open issue whose prerequisites are satisfied. Keep the change scoped to its milestone, run the commands above, and link the issue and verification evidence in the pull request. Include the sample rate, buffer size, host version and reproducible input for audio bugs. Document parameter identity, format or DSP behavior changes alongside their compatibility fixtures.

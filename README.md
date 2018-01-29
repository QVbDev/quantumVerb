# QuantumReverb

An open-source convolution reverb audio plugin developed for Quantum Music by 4th-year engineering students at Polytechnique Montréal.

# Getting started using Visual Studio 2017

## Getting the code

1. Clone repository
2. Download [VST3 SDK](https://www.steinberg.net/en/company/developers.html)
2. Unzip SDK files to C:\SDKs\ (or modify build solution to point to chosen SDK location)
3. Open Builds\VisualStudio2017\QuantumReverb.sln

## Working with the project

* Plugin classes and methods are located under the QuantumReverb_SharedCode project
  * To run plugin in standalone config: build and execute QuantumReverb_StandalonePlugin project
  * To build plugin in VST3 format: build and execute QuantumReverb_VST3 project
* Unit tests are located under the Tests project
  * To run unit tests: build and execute Tests project

# Style guide

See the [*Style Guide* page](StyleGuide.md)
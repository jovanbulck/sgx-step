# SGX-Step: A Practical Attack Framework for Precise Enclave Execution Control
[![License: GPL](https://img.shields.io/badge/License-GPL-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

This repository contains the source code of the SGX interrupt attack framework
accompanying our SysTEX 2017 [paper](systex17.pdf).

> Jo Van Bulck, Frank Piessens, and Raoul Strackx. 2017. SGX-Step: A Practical
> Attack Framework for Precise Enclave Execution Control. In Proceedings of the
> 2nd Workshop on System Software for Trusted Execution (SysTEX '17). 

## Abstract

Protected module architectures such as Intel SGX hold the promise of protecting
sensitive computations from a potentially compromised operating system. Recent
research convincingly demonstrated, however, that SGX's strengthened adversary
model also gives rise to to a new class of powerful, low-noise side-channel
attacks leveraging first-rate control over hardware. These attacks commonly
rely on frequent enclave preemptions to obtain fine-grained side-channel
observations. A maximal temporal resolution is achieved when the victim state
is measured after every instruction. Current state-of-the-art enclave
execution control schemes, however, do not generally achieve such
instruction-level granularity.

This paper presents SGX-Step, an open-source Linux kernel framework that allows
an untrusted host process to configure APIC timer interrupts and track page
table entries directly from user space. We contribute and evaluate an improved
approach to single-step enclaved execution at instruction-level granularity,
and we show how SGX-Step enables several new or improved attacks. Finally, we
discuss its implications for the design of effective defense mechanisms.

## Building and Running

Source code and building instructions coming soon!

## License

SGX-Step is free software, licensed under [GPLv3](https://www.gnu.org/licenses/gpl-3.0).

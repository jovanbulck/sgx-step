# SGX-Step "Selftest" Applications

This directory contains bare-metal applications designed to test various functionalities provided by `libsgxstep` without the need for SGX enclaves.

## Directory Overview

| Directory  | Description                                                                                               |
|------------|-----------------------------------------------------------------------------------------------------------|
| `apic`     | Microbenchmarks for timer interrupts via various Advanced Programmable Interrupt Controller (APIC) modes. |
| `apic-ipi` | Microbenchmarks for APIC inter-processor interrupts.                                                      |
| `cpl`      | Changing the Current Privilege Level (CPL) through calland interrupt GDT/IDT gates.                       |
| `idt`      | Custom handlers for software and timer interrupts via the Interrupt Descriptor Table (IDT).               |
| `idt_isr_map` | Stresstest to demonstrate custom IDT handlers can be called across processes.                          |

## Usage

To run the selftest applications:

1. Navigate to the desired directory.
2. Compile the application:
   ```bash
   make
   ```
3. Run the application with:
   ```bash
   sudo ./app
   ```

Reference output is available in the respective README files within each subdirectory.

**Note:** Any crashes during execution may indicate that your machine is not set up correctly for using SGX-Step. 
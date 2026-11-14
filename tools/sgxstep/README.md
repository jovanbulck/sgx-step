# sgxstep

A dynamic SGX enclave execution tracer that instruments enclave entry points at runtime via `LD_PRELOAD` — no SDK patching or app modification required.

## Setup

Add `sgxstep` to your PATH:

```bash
export PATH=/path/to/sgx-step-tracer/tools:$PATH
```

Add to `~/.bashrc` or `~/.zshrc` to make permanent.

## Usage

```bash
sgxstep [OPTIONS] ./app [app args]
```

## Options

| Flag | Description |
|------|-------------|
| `--skip N` | Skip the first N enclave entries before tracing |
| `--target N` | Only trace the Nth enclave entry |
| `--tf` | Enable hardware single-stepping via RFLAGS.TF |

## Examples

```bash
# Basic tracing
sgxstep ./app

# Skip first 3 enclave entries
sgxstep --skip 3 ./app

# Trace only the 4th enclave entry
sgxstep --target 4 ./app

# Single-step entire enclave execution
sgxstep --tf ./app

# Skip 2 entries then single-step
sgxstep --tf --skip 2 ./app
```

## Requirements

- `libsgx-step.so` must be built first: `make SHARED=1` in `libsgxstep/`
- Root privileges required (sudo is invoked automatically)

use std::{env, path::PathBuf};

fn main() {
    let sgxsdk = env::var("SGX_SDK").unwrap_or_else(|_| "/opt/intel/sgxsdk".into());
    println!("cargo:rerun-if-changed={sgxsdk}");

    println!("cargo:rustc-link-search=native={sgxsdk}/lib64");
    if cfg!(feature = "sim") {
        println!("cargo:rustc-link-lib=dylib=sgx_uae_service_sim");
        println!("cargo:rustc-link-lib=dylib=sgx_urts_sim");
    } else {
        println!("cargo:rustc-link-lib=dylib=sgx_uae_service");
        println!("cargo:rustc-link-lib=dylib=sgx_urts");
    }

    bindgen::Builder::default()
        .header("wrapper.h")
        .clang_arg(format!("-I{sgxsdk}/include"))
        .parse_callbacks(Box::new(bindgen::CargoCallbacks::new()))
        .generate()
        .expect("Unable to generate bindings")
        .write_to_file(PathBuf::from(env::var("OUT_DIR").unwrap()).join("bindings.rs"))
        .expect("Couldn't write bindings!");
}

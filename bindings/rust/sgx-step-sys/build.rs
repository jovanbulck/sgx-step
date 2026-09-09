use std::{env, path::PathBuf};

fn main() {
    let libsgxstep = env::var("LIBSGXSTEP").unwrap_or_else(|_| "../../../libsgxstep".into());

    println!("cargo:rerun-if-changed={libsgxstep}");

    #[cfg(feature = "build")]
    {
        cc::Build::new()
            .files(
                glob::glob(&format!("{libsgxstep}/*.c"))
                    .unwrap()
                    .filter_map(|e| e.ok()),
            )
            .cargo_metadata(true)
            .compile("sgxstep");
        println!("cargo:rustc-link-lib=elf");
    }

    bindgen::Builder::default()
        .header("wrapper.h")
        .clang_arg(format!("-I{libsgxstep}",))
        .parse_callbacks(Box::new(bindgen::CargoCallbacks::new()))
        .generate()
        .expect("Unable to generate bindings")
        .write_to_file(PathBuf::from(env::var("OUT_DIR").unwrap()).join("bindings.rs"))
        .expect("Couldn't write bindings!");
}

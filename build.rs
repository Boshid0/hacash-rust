extern crate cc;


/*


build release software

Ubuntu: 
RUSTFLAGS='-C target-feature=+crt-static' RUST_BACKTRACE=1 cargo build --release --target x86_64-unknown-linux-gnu
cp target/x86_64-unknown-linux-gnu/release/hacash ../../download_rust/hacash_ubuntu


Windows:
set RUSTFLAGS='-C target-feature=+crt-static' ; set RUST_BACKTRACE=1 ; cargo build --release --target x86_64-pc-windows-msvc
cp target/x86_64-pc-windows-msvc/release/hacash.exe ./hacash_windows.exe

MacOS:
RUSTFLAGS='-C target-feature=+crt-static' RUST_BACKTRACE=1 cargo build --release --target x86_64-apple-darwin
cp target/x86_64-apple-darwin/release/hacash ./hacash_macos


*/



fn main() {
    if cfg!(target_os = "linux") {
        cc::Build::new()
            .include("src/x16rs/simd-utils")
            .define("USE_NEW_ALGO", "1")
            .file("src/x16rs/algo/whirlpool/whirlpool_x86_64.S")
            .flag("-maes")
            .flag("-msse2")
            .flag("-mssse3")
            .flag("-msse4.1")
            .flag("-msse4.2")
            .include("src/x16rs/algo")
            .file("src/x16rs/x16rs.c")
            .flag("-O3")
            .flag("-march=native")
            .compile("x16rs");
    } else {
        cc::Build::new()
            .include("src/x16rs/algo")
            .file("src/x16rs/x16rs.c")
            .flag("-O3")
            .flag("-march=native")
            .compile("x16rs");
    }
    println!("cargo:rerun-if-changed=src/x16rs/x16rs.c");
}













/*

RUST update:
export RUSTUP_DIST_SERVER=https://mirrors.ustc.edu.cn/rust-static
export RUSTUP_UPDATE_ROOT=https://mirrors.ustc.edu.cn/rust-static/rustup


*/




/*

# Step 1: create libx16rs.a
# Step 2: build and run

gcc -c src/x16rs/x16rs.c && ar rcs libx16rs.a x16rs.o && mv *.a ./src/x16rs && rm -f *.o

RUSTFLAGS="$RUSTFLAGS -Awarnings -L ./src/x16rs/" cargo run

# Build static release software
cargo build --release --target=x86_64-unknown-linux-musl
ldd target/x86_64-unknown-linux-musl/release/hacash


RUSTFLAGS="$RUSTFLAGS -Awarnings -L ./src/x16rs/" cargo build && 
cp ./target/debug/hacash ./test/hacash_dev1 &&
./test/hacash_dev1 hacash.dev1.config.ini


RUSTFLAGS="$RUSTFLAGS -Awarnings" RUST_BACKTRACE=1 cargo run -- --reptblk


*/ 


/*

use std::process::Command;
use std::env;
use std::path::Path;

fn main() {
    let out_dir = env::var("OUT_DIR").unwrap();

    panic!("{}", out_dir);

    Command::new("gcc").args(&["src/x16rs/x16rs.c", "-c", "-fPIC", "-o"])
                       .arg(&format!("{}/x16rs.o", out_dir))
                       .status().unwrap();
    Command::new("ar").args(&["crus", "libx16rs.a", "x16rs.o"])
                      .current_dir(&Path::new(&out_dir))
                      .status().unwrap();

    println!("cargo:rustc-link-search=native={}", out_dir);
    println!("cargo:rustc-link-lib=static=x16rs");
    println!("cargo:rerun-if-changed=src/x16rs/x16rs.c");
}

*/
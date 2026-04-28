extern crate capstone;

use capstone::prelude::*;
use goblin::{error, Object};
use std::path::Path;
use std::env;
use std::fs;

fn main() {
    
}

//straceの動作テストを行うmain()
// use std::env;
// use strace_counter::strace::runner::run_strace;

// fn main() {
//     let args: Vec<String> = env::args().collect();
    
//     let syscall_counts = run_strace(&args);

//     for (syscall, count) in syscall_counts.iter() {
//         println!("{}: {}", syscall, count);
//     }
// }
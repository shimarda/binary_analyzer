use std::env;
use strace_counter::strace::runner::run_strace;

fn main() {
    let args: Vec<String> = env::args().collect();
    
    let syscall_counts = run_strace(&args);

    for (syscall, count) in syscall_counts.iter() {
        println!("{}: {}", syscall, count);
    }
}
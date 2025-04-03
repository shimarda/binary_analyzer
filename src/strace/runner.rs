use std::process::{Command, Stdio};
use std::io::{BufRead, BufReader};
use std::collections::HashMap;
use crate::strace::parser::parse_syscall;
use crate::strace::counter::count_syscalls;

pub fn run_strace(command: &[String]) -> HashMap<String, u32> {
    let mut counter_map: HashMap<String, u32> = HashMap::new();

    let mut output = Command::new("strace")
        .args(&command[1..])
        .stderr(Stdio::piped())
        .spawn()
        .expect("failed");
    
    let stderr = output.stderr.take().unwrap();
    let buf = BufReader::new(stderr);

    for line in buf.lines() {
        if let Ok(line) = line {
            if let Some(syscall) = parse_syscall(&line) {
                count_syscalls(&mut counter_map, &syscall);
            }
        }
    }

    counter_map
}
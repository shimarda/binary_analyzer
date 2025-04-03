use std::process::{Command, Stdio};
use std::env;
use std::io::{BufRead, BufReader};
use std::collections::HashMap;

extern crate regex;
use regex::Regex;

fn main() {
    let args: Vec<String> = env::args().collect();

    let regex_pattern = Regex::new(r"^([a-zA-Z0-9]+)").unwrap();

    let mut counter_map: HashMap<String, u32> = HashMap::new();

    match std::env::current_dir() {
        Ok(_x) =>
            {
                let mut output = Command::new("strace")
                    .args(&args[1..])
                    .stderr(Stdio::piped())
                    .spawn()
                    .expect("failed to execute process");
                let stderr = output.stderr.take().unwrap();
                let buf = BufReader::new(stderr);
                
                for line in buf.lines() {
                    if let Ok(line) = line {
                        if let Some(caps) = regex_pattern.captures(&line) {
                            let syscall = caps[1].to_string();

                            *counter_map.entry(syscall.clone()).or_insert(0) += 1;
                        }
                    }
                }

                for (syscall, count) in counter_map.iter() {
                    println!("{}: {}", syscall, count);
                }
            },
        Err(err) => {
            eprintln!("{:?}", err);
        },
    }
}
extern crate regex;
use regex::Regex;

pub fn parse_syscall(line: &str) -> Option<String> {
    let regex_pattern = Regex::new(r"^([a-zA-Z0-9_]+)").unwrap();
    if let Some(caps) = regex_pattern.captures(line) {
        return Some(caps[1].to_string());
    }
    None
}
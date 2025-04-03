use std::collections::HashMap;

pub fn count_syscalls(counter_map: &mut HashMap<String, u32>, syscall: &str) {
    *counter_map.entry(syscall.to_string()).or_insert(0) += 1;
}
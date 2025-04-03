use std::process::{Command, Stdio};
use std::env;
use std::io::Read;

fn main() {
    let args: Vec<String> = env::args().collect();

    match std::env::current_dir() {
        Ok(_x) =>
            {
                let output = Command::new("strace")
                    .args(&args[1..])
                    .stderr(Stdio::piped())
                    .spawn()
                    .expect("failed to execute process");
                let mut stderr = output.stderr.unwrap();
                let mut buf = String::new();
                stderr.read_to_string(&mut buf).unwrap();
                println!("{}", buf);
            },
        Err(err) => {
            eprintln!("{:?}", err);
        },
    }
}

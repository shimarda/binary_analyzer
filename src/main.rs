use std::process::Command;

fn main() {
    // let args: Vec<String> = env::args().collect();
    match std::env::current_dir() {
        Ok(x) =>
            {
                let cmd: &str = "ls ";
                let output = Command::new(cmd)
                    .current_dir(x)
                    .output()
                    .expect("failed to execute process");
                let res = output.stdout;
                println!("{}", std::str::from_utf8(&res).unwrap());
            },
        Err(err) => {
            eprintln!("{:?}", err);
        },
    }
}

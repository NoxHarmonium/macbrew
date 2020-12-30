use reqwest;
use tokio;

use std::{env, str};
use tokio::stream::StreamExt;
use tokio_serial::{Serial, SerialPortSettings};
use tokio_util::codec::Framed;

mod codecs;
mod data;

#[cfg(unix)]
const DEFAULT_TTY: &str = "/dev/pts/1";
#[cfg(windows)]
const DEFAULT_TTY: &str = "COM1";

use codecs::line_codec::LineCodec;

#[tokio::main]
async fn main() -> Result<(), reqwest::Error> {
    println!("Fetching sessions...");
    let session_data = data::sessions::load_sessions().await?;

    println!("Fetched [{:#?}] sessions", session_data.brewsessions.len());
    println!("Opening serial interface...");

    let mut args = env::args();
    let tty_path = args.nth(1).unwrap_or_else(|| DEFAULT_TTY.into());

    let settings = SerialPortSettings::default();
    let mut port = Serial::from_path(tty_path, &settings).unwrap();

    #[cfg(unix)]
    port.set_exclusive(false)
        .expect("Unable to set serial port exclusive to false");

    let mut reader = Framed::new(port, LineCodec);

    while let Some(line_result) = reader.next().await {
        let line = line_result.expect("Failed to read line");
        println!("{}", line);
    }

    Ok(())
}

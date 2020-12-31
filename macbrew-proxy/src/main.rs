use futures::sink::SinkExt;
use std::{env, error, str};
use tokio;
use tokio::stream::StreamExt;
use tokio_serial::{Serial, SerialPortSettings};
use tokio_util::codec::Framed;

mod codecs;
mod commands;
mod data;

#[cfg(unix)]
const DEFAULT_TTY: &str = "/dev/pts/1";
#[cfg(windows)]
const DEFAULT_TTY: &str = "COM1";

use codecs::line_codec::LineCodec;
use commands::command::Command;

async fn send_help(framed: &mut Framed<Serial, LineCodec>) -> Result<(), Box<dyn error::Error>> {
    framed
        .send("Invalid command. Type HELP to get a list of valid commands.\n".to_string())
        .await?;
    Ok(())
}

#[tokio::main]
async fn main() -> Result<(), Box<dyn error::Error>> {
    let mut args = env::args();
    let tty_path = args.nth(1).unwrap_or_else(|| DEFAULT_TTY.into());

    let settings = SerialPortSettings::default();
    let mut port = Serial::from_path(tty_path, &settings).unwrap();

    #[cfg(unix)]
    port.set_exclusive(false)
        .expect("Unable to set serial port exclusive to false");

    let mut framed = Framed::new(port, LineCodec);

    let commands = [commands::help::HelpCommand::new("HELP")];

    while let Some(line_result) = framed.next().await {
        let line = line_result.expect("Failed to read line");
        let components = line.trim().split("|").collect::<Vec<_>>();
        match components.as_slice() {
            [] => send_help(&mut framed).await?,
            [command_tag, args @ ..] => {
                let matching_command = commands
                    .iter()
                    .find(|&command| command.name() == *command_tag);
                if let Some(command) = matching_command {
                    let result = command
                        .handle(
                            args.to_vec()
                                .iter()
                                .map(|c| c.to_string())
                                .collect::<Vec<_>>(),
                        )
                        .await?;
                    framed.send(result.to_string()).await?;
                } else {
                    send_help(&mut framed).await?;
                }
            }
        };
    }

    Ok(())
}

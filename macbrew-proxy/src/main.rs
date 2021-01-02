use error::{Error, Result};
use futures::sink::SinkExt;
use std::{env, str};
use tokio;
use tokio::stream::StreamExt;
use tokio_serial::{Serial, SerialPortSettings};
use tokio_util::codec::Framed;

mod codecs;
mod commands;
mod data;
mod error;
mod serializers;

#[cfg(unix)]
const DEFAULT_TTY: &str = "/dev/pts/1";
#[cfg(windows)]
const DEFAULT_TTY: &str = "COM1";

use codecs::line_codec::LineCodec;
use commands::command::{prepare_response, Command};
use data::api_data_manager::ApiDataManager;

async fn handle_line(line: &str) -> Result<Vec<u8>> {
    let components = line.split(" ").collect::<Vec<_>>();
    let response = match components.as_slice() {
        [] => Err(Error::InvalidCommandInput {
            message: String::from(line),
        }),
        [rid, "LIST", "SESSIONS", args @ ..] => {
            commands::list_sessions::ListSessionsCommand::<ApiDataManager>::handle(rid, args).await
        }
        [rid, "GET", "SESSIONS", args @ ..] => {
            commands::get_sessions::GetSessionsCommand::<ApiDataManager>::handle(rid, args).await
        }
        [_, _args @ ..] => Err(Error::InvalidCommandInput {
            message: String::from(line),
        }),
    };
    match response {
        Ok(response) => Ok(response),
        Err(error) => prepare_response("0", false, &error),
    }
}

#[tokio::main]
async fn main() -> Result<()> {
    let mut args = env::args();
    let tty_path = args.nth(1).unwrap_or_else(|| DEFAULT_TTY.into());

    let settings = SerialPortSettings::default();
    let mut port = Serial::from_path(tty_path, &settings).unwrap();

    #[cfg(unix)]
    port.set_exclusive(false)
        .expect("Unable to set serial port exclusive to false");

    let mut framed = Framed::new(port, LineCodec);

    while let Some(line_result) = framed.next().await {
        let line = line_result.expect("Failed to read line");
        let response = handle_line(line.trim()).await?;
        framed.send(response).await?;
    }

    Ok(())
}

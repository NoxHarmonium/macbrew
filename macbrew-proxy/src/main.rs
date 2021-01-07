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
use data::brewers_friend::bf_api_data_manager::BFApiDataManager;

async fn handle_line(line: &str) -> Result<Vec<u8>> {
    let components = line.split(" ").collect::<Vec<_>>();
    let response = match components.as_slice() {
        [] => Err(Error::InvalidCommandInput {
            message: String::from(line),
        }),
        [rid, "LIST", "SESSION", args @ ..] => {
            commands::list_sessions::ListSessionsCommand::<BFApiDataManager>::handle(rid, args)
                .await
        }
        [rid, "GET", "SESSION", args @ ..] => {
            commands::get_sessions::GetSessionsCommand::<BFApiDataManager>::handle(rid, args).await
        }
        [rid, "GET", "RECIPE", args @ ..] => {
            commands::get_recipes::GetRecipesCommand::<BFApiDataManager>::handle(rid, args).await
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

#[cfg(test)]
mod tests {

    use hexplay::HexViewBuilder;
    use mockito::mock;

    use super::*;

    fn format_binary_for_snap(binary: Vec<u8>) -> Vec<String> {
        let view = HexViewBuilder::new(&binary.as_slice())
            .row_width(16)
            .finish();
        // Split on new lines to make the snapshots easier to read
        view.to_string().split("\n").map(String::from).collect()
    }

    #[tokio::test]
    async fn test_get_recipe() {
        let recipe_xml = include_str!("../resources/sample_recipe.xml");
        let _m = mock("GET", "/v1/recipes/123456.xml")
            .with_status(200)
            .with_header("content-type", "application/xml")
            .with_header("x-api-key", "1234")
            .with_body(recipe_xml)
            .create();

        let result = handle_line("99 GET RECIPE 123456").await.unwrap();

        insta::assert_debug_snapshot!(format_binary_for_snap(result));
    }
}

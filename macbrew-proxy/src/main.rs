#![warn(clippy::all)]

use clap::Clap;
use error::{Error, Result};
use futures::sink::SinkExt;
use std::str;
use tokio::stream::StreamExt;
use tokio_serial::{Serial, SerialPortSettings};
use tokio_util::codec::Framed;

mod codecs;
mod commands;
mod data;
mod error;
mod serializers;
mod utils;

#[cfg(unix)]
const DEFAULT_TTY: &str = "/dev/ttyUSB0";
#[cfg(windows)]
const DEFAULT_TTY: &str = "COM1";

use codecs::line_codec::LineCodec;
use commands::command::{prepare_response, Command};
use data::brewers_friend::bf_api_data_manager::BFApiDataManager;

#[derive(Clap)]
#[clap(version = "0.1.4", author = "Sean Dawson <contact@seandawson.info>")]
struct Opts {
    #[clap(default_value = DEFAULT_TTY)]
    serial_device: String,
}

async fn handle_line(line: &str) -> Result<Vec<u8>> {
    println!("Received command");
    let components = line.split_terminator(' ').collect::<Vec<_>>();
    if let Some((request_id, rest)) = components.split_first() {
        let response = match rest {
            [] => Err(Error::InvalidCommandInput {
                message: String::from(line),
            }),
            ["LIST", "SESSION", args @ ..] => {
                commands::list_sessions::ListSessionsCommand::<BFApiDataManager>::handle(
                    request_id, args,
                )
                .await
            }
            ["GET", "SESSION", args @ ..] => {
                commands::get_sessions::GetSessionsCommand::<BFApiDataManager>::handle(
                    request_id, args,
                )
                .await
            }
            ["GET", "RECIPE", args @ ..] => {
                commands::get_recipes::GetRecipesCommand::<BFApiDataManager>::handle(
                    request_id, args,
                )
                .await
            }
            [_args @ ..] => Err(Error::InvalidCommandInput {
                message: String::from(line),
            }),
        };
        match response {
            Ok(response) => Ok(response),
            Err(error) => prepare_response(request_id, false, &error.to_string()),
        }
    } else {
        prepare_response("ERROR", false, &"Empty Request")
    }
}

#[tokio::main]
async fn main() -> Result<()> {
    let opts: Opts = Opts::parse();

    println!("Starting MacBrew on port {}", opts.serial_device);

    let settings = SerialPortSettings::default();
    let mut port = Serial::from_path(opts.serial_device, &settings).unwrap();

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

    use crate::utils::xor_checksum_from_bytes;
    use encoding::all::MAC_ROMAN;
    use encoding::{EncoderTrap, Encoding};
    use hexplay::HexViewBuilder;
    use mockito::mock;
    use std::convert::TryInto;

    use super::*;

    fn assert_request_id(binary: &Vec<u8>, request_id: &'static str) {
        // The first two bytes after the message length are the 16 bit integer that indicates the length of the session ID string
        let string_len = u16::from_be_bytes(
            binary[2..4]
                .try_into()
                .expect("expected 2 byte (u16) slice"),
        );
        println!("string_len: {}", string_len);
        assert_eq!(
            &binary[4..4 + string_len as usize],
            MAC_ROMAN
                .encode(request_id, EncoderTrap::Replace)
                .unwrap()
                .as_slice()
        );
    }

    fn assert_length(binary: &Vec<u8>) {
        // First 2 bytes are the 16 bit integer that indicates the length of the entire message (minus checksum)
        let length_size = 2;
        // Checksum is 32 bit
        let checksum_size = 4;
        let expected_length = u16::from_be_bytes(
            binary[0..2]
                .try_into()
                .expect("expected 2 byte (u16) slice"),
        );
        assert_eq!(
            expected_length,
            // Test only the length of the payload, not the envelope
            (binary.len() - length_size - checksum_size) as u16
        );
    }

    fn assert_checksum(binary: &Vec<u8>) {
        // First 2 bytes are the 16 bit integer that indicates the length of the entire message. Discard that.
        let mut payload_without_length = binary.clone().split_off(2);
        let checksum_bytes = payload_without_length.split_off(payload_without_length.len() - 4);
        let checksum_value = u32::from_be_bytes(
            checksum_bytes
                .try_into()
                .expect("expected 4 byte (u32) slice"),
        );

        let payload_checksum = xor_checksum_from_bytes(&payload_without_length);
        let result = checksum_value ^ payload_checksum;

        // XOR of all bytes and checksum should be zero
        assert_eq!(result, 0);
    }

    fn format_binary_for_snap(binary: &Vec<u8>) -> Vec<String> {
        let view = HexViewBuilder::new(binary.as_slice())
            .row_width(16)
            .finish();
        // Split on new lines to make the snapshots easier to read
        view.to_string().split('\n').map(String::from).collect()
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

        let result = handle_line("11 GET RECIPE 123456").await.unwrap();

        assert_length(&result);
        assert_checksum(&result);
        assert_request_id(&result, "11");
        insta::assert_debug_snapshot!("get_recipe", format_binary_for_snap(&result));
    }

    #[tokio::test]
    async fn test_list_sessions() {
        let sessions_json = include_str!("../resources/sample_sessions.json");
        let _m = mock("GET", "/v1/brewsessions")
            .with_status(200)
            .with_header("content-type", "application/xml")
            .with_header("x-api-key", "1234")
            .with_body(sessions_json)
            .create();

        let result = handle_line("22 LIST SESSION").await.unwrap();

        assert_length(&result);
        assert_checksum(&result);
        assert_request_id(&result, "22");
        insta::assert_debug_snapshot!("list_sessions", format_binary_for_snap(&result));
    }

    #[tokio::test]
    async fn test_get_sessions() {
        let session_json = include_str!("../resources/sample_session.json");
        let _m = mock("GET", "/v1/brewsessions/363597")
            .with_status(200)
            .with_header("content-type", "application/xml")
            .with_header("x-api-key", "1234")
            .with_body(session_json)
            .create();

        let result = handle_line("33 GET SESSION 363597").await.unwrap();

        assert_length(&result);
        assert_checksum(&result);
        assert_request_id(&result, "33");
        insta::assert_debug_snapshot!("get_sessions", format_binary_for_snap(&result));
    }

    #[tokio::test]
    async fn test_invalid_command() {
        let result = handle_line("FOO BAR").await.unwrap();

        insta::assert_debug_snapshot!("invalid_command", format_binary_for_snap(&result));
        assert_length(&result);
        assert_checksum(&result);
        assert_request_id(&result, "FOO");
    }

    #[tokio::test]
    async fn test_command_error() {
        let _m = mock("GET", "/v1/brewsessions/363597")
            .with_status(500)
            .with_header("x-api-key", "1234")
            .with_body("something bad happened")
            .create();

        let result = handle_line("33 GET SESSION 363597").await.unwrap();

        assert_length(&result);
        assert_checksum(&result);
        assert_request_id(&result, "33");
        insta::assert_debug_snapshot!("command_error", format_binary_for_snap(&result));
    }

    #[tokio::test]
    async fn test_empty_command() {
        let result = handle_line("").await.unwrap();

        insta::assert_debug_snapshot!("empty_command", format_binary_for_snap(&result));
        assert_length(&result);
        assert_checksum(&result);
    }
}

use quick_xml::de::DeError;
use reqwest::Error as ReqwestError;
use serde::{ser, Serialize};
use snafu::Snafu;

// TODO: Clean these up to provide more context and better error messages
#[derive(Debug, Snafu, Serialize)]
pub enum Error {
    #[snafu(display("Could not serialize data"))]
    SerializationFailure {},

    #[snafu(display("Error calling API: {}", inner_error))]
    ApiError {
        #[serde(skip_serializing)]
        inner_error: ReqwestError,
    },

    #[snafu(display("Error derserializing XML: {}", inner_error))]
    XMLError {
        #[serde(skip_serializing)]
        inner_error: DeError,
    },

    #[snafu(display("Error derserializing JSON: {}", inner_error))]
    JsonError {
        #[serde(skip_serializing)]
        inner_error: serde_json::Error,
    },

    #[snafu(display("API response failed validation: {}", message))]
    ApiResponseValidationError { message: String },

    #[snafu(display("Invalid command input: {}", message))]
    InvalidCommandInput { message: String },

    #[snafu(display("Serial IO error: {}", inner_error))]
    SerialIoError {
        #[serde(skip_serializing)]
        inner_error: std::io::Error,
    },

    #[snafu(display("Unknown error: {}", message))]
    UnknownError { message: String },

    #[snafu(display("Could not allocate serialization buffer. (Not enough RAM?)"))]
    SerializeBufferFull {},

    #[snafu(display("Could not work out length of sequence"))]
    SerializeSeqLengthUnknown {},
}

pub type Result<T, E = Error> = std::result::Result<T, E>;

impl From<ReqwestError> for Error {
    fn from(inner_error: ReqwestError) -> Self {
        Error::ApiError { inner_error }
    }
}

impl From<std::io::Error> for Error {
    fn from(inner_error: std::io::Error) -> Self {
        Error::SerialIoError { inner_error }
    }
}

impl From<quick_xml::de::DeError> for Error {
    fn from(inner_error: DeError) -> Self {
        Error::XMLError { inner_error }
    }
}

impl From<serde_json::Error> for Error {
    fn from(inner_error: serde_json::Error) -> Self {
        Error::JsonError { inner_error }
    }
}

impl ser::Error for Error {
    #[cold]
    fn custom<T: std::fmt::Display>(msg: T) -> Error {
        // TODO: What should this actually be doing?
        // Hint: https://github.com/serde-rs/json/blob/master/src/error.rs#L373
        Error::UnknownError {
            message: msg.to_string(),
        }
    }
}

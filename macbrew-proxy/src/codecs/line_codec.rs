use std::io;
use tokio_util::codec::{Decoder, Encoder};

use bytes::BytesMut;

use encoding::all::MAC_ROMAN;
use encoding::{DecoderTrap, EncoderTrap, Encoding};

///
/// A codec that just encodes/decodes text over a serial device
/// with a Classic Mac encoding.
///
pub struct LineCodec;

impl Decoder for LineCodec {
    type Item = String;
    type Error = io::Error;

    fn decode(&mut self, src: &mut BytesMut) -> Result<Option<Self::Item>, Self::Error> {
        // Classic Macs use a single carriage return for a new line
        let newline = src.as_ref().iter().position(|b| *b == b'\r' || *b == b'\n');
        if let Some(n) = newline {
            let line = src.split_to(n + 1);
            return match MAC_ROMAN.decode(line.as_ref(), DecoderTrap::Replace) {
                Ok(string) => Ok(Some(string)),
                Err(error) => Err(io::Error::new(io::ErrorKind::Other, error)),
            };
        }
        Ok(None)
    }
}

impl Encoder<String> for LineCodec {
    type Error = io::Error;

    fn encode(&mut self, item: String, dst: &mut BytesMut) -> Result<(), Self::Error> {
        return match MAC_ROMAN.encode(&item, EncoderTrap::Replace) {
            Ok(bytes) => {
                dst.extend_from_slice(&bytes);
                Ok(())
            }
            Err(error) => Err(io::Error::new(io::ErrorKind::Other, error)),
        };
    }
}

impl Encoder<Vec<u8>> for LineCodec {
    type Error = io::Error;

    fn encode(&mut self, item: Vec<u8>, dst: &mut BytesMut) -> Result<(), Self::Error> {
        dst.extend(item);
        // Finish the line
        dst.extend(MAC_ROMAN.encode("\r\n", EncoderTrap::Replace).unwrap());
        Ok(())
    }
}

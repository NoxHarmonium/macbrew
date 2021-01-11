use crate::data::brewers_friend::bf_data_manager::BFDataManager;
use crate::error::Result;
use crate::serializers::mac::serialize;
use crate::utils::xor_checksum_from_bytes;
use async_trait::async_trait;
use serde::Serialize;

#[async_trait]
pub trait Command<DataManagerType: BFDataManager> {
    async fn handle(rid: &str, args: &[&str]) -> Result<Vec<u8>>;
}

fn checksum(payload: &[u8]) -> Result<Vec<u8>> {
    let checksum_value = xor_checksum_from_bytes(payload);
    serialize(&checksum_value)
}

pub fn prepare_response<T>(rid: &str, success: bool, response_object: &T) -> Result<Vec<u8>>
where
    T: Serialize + ?Sized,
{
    let preable_bytes = serialize(&rid)?;
    let success_bytes = serialize(&success)?;
    let response_bytes = serialize(&response_object)?;
    let payload = [preable_bytes, success_bytes, response_bytes].concat();
    // TODO: Again check that these casts are not unsafe
    let payload_length = payload.len() as u16;
    let payload_length_bytes = serialize(&payload_length)?;
    let payload_checksum = checksum(&payload)?;
    let envelope = [payload_length_bytes, payload, payload_checksum].concat();
    Ok(envelope)
}

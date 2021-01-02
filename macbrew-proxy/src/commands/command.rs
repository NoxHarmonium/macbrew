use crate::data::data_manager::DataManager;
use crate::error::Result;
use crate::serializers::mac::serialize;
use async_trait::async_trait;
use serde::Serialize;

#[async_trait]
pub trait Command<DataManagerType: DataManager> {
    async fn handle(rid: &str, args: &[&str]) -> Result<Vec<u8>>;
}

pub fn prepare_response<T>(rid: &str, success: bool, response_object: &T) -> Result<Vec<u8>>
where
    T: Serialize + ?Sized,
{
    let preable_bytes = serialize(&rid)?;
    let success_bytes = serialize(&success)?;
    let response_bytes = serialize(&response_object)?;
    Ok([preable_bytes, success_bytes, response_bytes].concat())
}

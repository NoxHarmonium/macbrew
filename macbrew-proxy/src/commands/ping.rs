use crate::commands::command::{prepare_response, Command};
use crate::data::brewers_friend::bf_data_manager::BFDataManager;
use crate::error::Result;
use async_trait::async_trait;
use std::marker::PhantomData;

// TODO: Work out why rustc thinks T is unused. Is PhantomData needed? Can I restructure this?
// Clue: https://github.com/rust-lang/rust/issues/23246
pub struct PingCommand<DataManager: BFDataManager>(PhantomData<DataManager>);

#[async_trait]
impl<DataManager: BFDataManager + 'static> Command<DataManager> for PingCommand<DataManager> {
    async fn handle(rid: &str, _: &[&str]) -> Result<Vec<u8>> {
        prepare_response(rid, true, "Pong!")
    }
}

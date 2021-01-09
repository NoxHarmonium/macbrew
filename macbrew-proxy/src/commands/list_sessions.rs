use crate::commands::command::{prepare_response, Command};
use crate::data::brewers_friend::bf_data_manager::BFDataManager;
use crate::data::macbrew::sessions::BrewSessionReference;
use crate::error::Result;
use async_trait::async_trait;
use std::marker::PhantomData;

// TODO: Work out why rustc thinks T is unused. Is PhantomData needed? Can I restructure this?
// Clue: https://github.com/rust-lang/rust/issues/23246
pub struct ListSessionsCommand<T: BFDataManager>(PhantomData<T>);

#[async_trait]
impl<T: BFDataManager + 'static> Command<T> for ListSessionsCommand<T> {
    async fn handle(rid: &str, _args: &[&str]) -> Result<Vec<u8>> {
        let sessions = T::sessions().await?;
        let session_list = sessions
            .iter()
            .map(|session| BrewSessionReference::from_bf_session(session))
            .collect::<Vec<_>>();

        prepare_response(rid, true, &session_list)
    }
}

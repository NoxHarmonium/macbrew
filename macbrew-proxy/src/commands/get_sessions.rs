use crate::commands::command::{prepare_response, Command};
use crate::data::brewers_friend::bf_data_manager::BFDataManager;
use crate::data::macbrew::sessions::BrewSession;
use crate::error::Error::InvalidCommandInput;
use crate::error::Result;
use async_trait::async_trait;
use std::marker::PhantomData;

// TODO: Work out why rustc thinks T is unused. Is PhantomData needed? Can I restructure this?
// Clue: https://github.com/rust-lang/rust/issues/23246
pub struct GetSessionsCommand<T: BFDataManager>(PhantomData<T>);

#[async_trait]
impl<T: BFDataManager + 'static> Command<T> for GetSessionsCommand<T> {
    async fn handle(rid: &str, args: &[&str]) -> Result<Vec<u8>> {
        match args {
            [session_id] => {
                let bf_session = T::session(session_id).await?;
                let session = BrewSession::from_bf_session(&bf_session);
                prepare_response(rid, true, &session)
            }
            [_args @ ..] => Err(InvalidCommandInput {
                message: String::from("Expected single parameter (Session ID)"),
            }),
        }
    }
}

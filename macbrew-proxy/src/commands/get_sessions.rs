use crate::commands::command::{prepare_response, Command};
use crate::data::brewers_friend::bf_data_manager::BFDataManager;
use crate::data::macbrew::sessions::BrewSession;
use crate::error::Error::InvalidCommandInput;
use crate::error::Result;
use async_trait::async_trait;
use std::marker::PhantomData;

// TODO: Work out why rustc thinks T is unused. Is PhantomData needed? Can I restructure this?
// Clue: https://github.com/rust-lang/rust/issues/23246
pub struct GetSessionsCommand<DataManager: BFDataManager>(PhantomData<DataManager>);

#[async_trait]
impl<DataManager: BFDataManager + 'static> Command<DataManager>
    for GetSessionsCommand<DataManager>
{
    async fn handle(rid: &str, args: &[&str]) -> Result<Vec<u8>> {
        match args {
            [session_id] => {
                let bf_session = DataManager::session(session_id).await?;
                let session = BrewSession::from_bf_session(&bf_session);
                prepare_response(rid, true, &session)
            }
            [_args @ ..] => Err(InvalidCommandInput {
                message: String::from("Expected single parameter (Session ID)"),
            }),
        }
    }
}

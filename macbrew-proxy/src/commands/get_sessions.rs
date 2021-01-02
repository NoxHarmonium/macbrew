use crate::commands::command::{prepare_response, Command};
use crate::data::data_manager::DataManager;
use crate::error::Error::InvalidCommandInput;
use crate::error::Result;
use async_trait::async_trait;
use std::marker::PhantomData;

// TODO: Work out why rustc thinks T is unused. Is PhantomData needed? Can I restructure this?
// Clue: https://github.com/rust-lang/rust/issues/23246
pub struct GetSessionsCommand<T: DataManager>(PhantomData<T>);

#[async_trait]
impl<T: DataManager + 'static> Command<T> for GetSessionsCommand<T> {
    async fn handle(rid: &str, args: &[&str]) -> Result<Vec<u8>> {
        match args {
            [session_id] => {
                let session = T::session(session_id).await?;
                prepare_response(rid, true, &session)
            }
            [_args @ ..] => Err(InvalidCommandInput {
                message: String::from("Expected single parameter (Session ID)"),
            }),
        }
    }
}

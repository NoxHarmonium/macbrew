use crate::commands::command::{prepare_response, Command};
use crate::data::data_manager::DataManager;
use crate::error::Result;
use async_trait::async_trait;
use serde::Serialize;
use std::marker::PhantomData;

/// Enough information to decide which session to fetch more info on
#[derive(Serialize, Debug, Clone)]
struct SessionReference {
    id: String,
    batch_code: String,
    name: String,
}

// TODO: Work out why rustc thinks T is unused. Is PhantomData needed? Can I restructure this?
// Clue: https://github.com/rust-lang/rust/issues/23246
pub struct ListSessionsCommand<T: DataManager>(PhantomData<T>);

#[async_trait]
impl<T: DataManager + 'static> Command<T> for ListSessionsCommand<T> {
    async fn handle(rid: &str, _args: &[&str]) -> Result<Vec<u8>> {
        let sessions = T::sessions().await?;
        let session_list = sessions
            .iter()
            .map(|session| SessionReference {
                id: session.id.clone(),
                batch_code: session.batchcode.clone(),
                name: session.recipe_title.clone(),
            })
            .collect::<Vec<_>>();

        prepare_response(rid, true, &session_list)
    }
}

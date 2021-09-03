use crate::commands::command::{prepare_response, Command};
use crate::data::brewers_friend::bf_data_manager::BfDataManager;
use crate::data::macbrew::fermentation::FermentationData;
use crate::error::Error::InvalidCommandInput;
use crate::error::Result;
use async_trait::async_trait;
use std::marker::PhantomData;

// TODO: Work out why rustc thinks T is unused. Is PhantomData needed? Can I restructure this?
// Clue: https://github.com/rust-lang/rust/issues/23246
pub struct GetFermentationDataCommand<DataManager: BfDataManager>(PhantomData<DataManager>);

#[async_trait]
impl<DataManager: BfDataManager + 'static> Command<DataManager>
    for GetFermentationDataCommand<DataManager>
{
    async fn handle(rid: &str, args: &[&str]) -> Result<Vec<u8>> {
        match args {
            [session_id] => {
                let bf_fermentation = DataManager::fermentation(session_id).await?;
                let fermentation_data = FermentationData::from_bf_fermentation(&bf_fermentation);
                prepare_response(rid, true, &fermentation_data)
            }
            [_args @ ..] => Err(InvalidCommandInput {
                message: String::from("Expected single parameter (Session ID)"),
            }),
        }
    }
}

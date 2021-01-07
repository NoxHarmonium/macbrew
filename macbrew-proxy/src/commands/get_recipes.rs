use crate::commands::command::{prepare_response, Command};
use crate::data::brewers_friend::bf_data_manager::BFDataManager;
use crate::error::Error::InvalidCommandInput;
use crate::error::Result;
use async_trait::async_trait;
use std::marker::PhantomData;

// TODO: Work out why rustc thinks T is unused. Is PhantomData needed? Can I restructure this?
// Clue: https://github.com/rust-lang/rust/issues/23246
pub struct GetRecipesCommand<T: BFDataManager>(PhantomData<T>);

#[async_trait]
impl<T: BFDataManager + 'static> Command<T> for GetRecipesCommand<T> {
    async fn handle(rid: &str, args: &[&str]) -> Result<Vec<u8>> {
        match args {
            [recipe_id] => {
                let recipe = T::recipe(recipe_id).await?;
                prepare_response(rid, true, &recipe)
            }
            [_args @ ..] => Err(InvalidCommandInput {
                message: String::from("Expected single parameter (Recipe ID)"),
            }),
        }
    }
}

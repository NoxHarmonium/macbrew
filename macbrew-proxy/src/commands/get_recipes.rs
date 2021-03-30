use crate::commands::command::{prepare_response, Command};
use crate::data::brewers_friend::bf_data_manager::BfDataManager;
use crate::data::macbrew::recipes::Recipe;
use crate::error::Error::InvalidCommandInput;
use crate::error::Result;
use async_trait::async_trait;
use std::marker::PhantomData;

// TODO: Work out why rustc thinks T is unused. Is PhantomData needed? Can I restructure this?
// Clue: https://github.com/rust-lang/rust/issues/23246
pub struct GetRecipesCommand<DataManager: BfDataManager>(PhantomData<DataManager>);

#[async_trait]
impl<DataManager: BfDataManager + 'static> Command<DataManager> for GetRecipesCommand<DataManager> {
    async fn handle(rid: &str, args: &[&str]) -> Result<Vec<u8>> {
        match args {
            [recipe_id] => {
                let beer_xml_recipe = DataManager::recipe(recipe_id).await?;
                let recipe = Recipe::from_beer_xml(&beer_xml_recipe);
                prepare_response(rid, true, &recipe)
            }
            [_args @ ..] => Err(InvalidCommandInput {
                message: String::from("Expected single parameter (Recipe ID)"),
            }),
        }
    }
}

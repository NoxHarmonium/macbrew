use crate::data::brewers_friend::fermentation::BfFermentationResponse;
use crate::data::brewers_friend::recipes::BeerXml;
use crate::data::brewers_friend::sessions::BfBrewSession;
use crate::data::brewers_friend::sessions::BfBrewSessionFull;
use crate::Result;
use async_trait::async_trait;

#[async_trait]
pub trait BfDataManager {
    async fn sessions() -> Result<Vec<BfBrewSession>>;
    async fn session(session_id: &str) -> Result<BfBrewSessionFull>;
    async fn fermentation(session_id: &str) -> Result<BfFermentationResponse>;
    async fn recipe(recipe_id: &str) -> Result<BeerXml>;
}

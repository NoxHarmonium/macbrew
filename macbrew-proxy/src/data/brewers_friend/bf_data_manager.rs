use crate::data::brewers_friend::recipes::BeerXml;
use crate::data::brewers_friend::sessions::BfBrewSession;
use crate::Result;
use async_trait::async_trait;

#[async_trait]
pub trait BfDataManager {
    async fn sessions() -> Result<Vec<BfBrewSession>>;
    async fn session(session_id: &str) -> Result<BfBrewSession>;
    async fn recipe(recipe_id: &str) -> Result<BeerXml>;
}

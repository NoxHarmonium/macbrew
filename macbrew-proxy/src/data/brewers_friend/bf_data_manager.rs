use crate::data::brewers_friend::recipes::BeerXml;
use crate::data::brewers_friend::sessions::BrewSession;
use crate::Result;
use async_trait::async_trait;

#[async_trait]
pub trait BFDataManager {
    async fn sessions() -> Result<Vec<BrewSession>>;
    async fn session(session_id: &str) -> Result<BrewSession>;
    async fn recipe(recipe_id: &str) -> Result<BeerXml>;
}

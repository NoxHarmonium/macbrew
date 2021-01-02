use crate::data::sessions::BrewSession;
use crate::Result;
use async_trait::async_trait;

#[async_trait]
pub trait DataManager {
    async fn sessions() -> Result<Vec<BrewSession>>;
    async fn session(session_id: &str) -> Result<BrewSession>;
}

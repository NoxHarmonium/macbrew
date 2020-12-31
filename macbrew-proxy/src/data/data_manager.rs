use super::sessions::BrewSession;
use async_trait::async_trait;
use std::error;

#[async_trait]
pub trait DataManager {
    async fn sessions() -> Result<Vec<BrewSession>, Box<dyn error::Error>>;
}

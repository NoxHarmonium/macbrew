use super::data_manager::DataManager;
use super::sessions::load_sessions;
use super::sessions::BrewSession;
use async_trait::async_trait;
use std::error;

pub struct ApiDataManager;

#[async_trait]
impl DataManager for ApiDataManager {
    async fn sessions() -> Result<Vec<BrewSession>, Box<dyn error::Error>> {
        println!("Fetching sessions...");
        let response = load_sessions().await?;

        println!("Fetched [{:#?}] sessions", response.brewsessions.len());

        Ok(response.brewsessions)
    }
}

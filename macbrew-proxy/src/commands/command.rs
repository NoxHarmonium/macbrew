use super::super::data::data_manager::DataManager;
use async_trait::async_trait;
use std::error;

#[async_trait]
pub trait Command<DataManagerType: DataManager> {
    fn new(name: &'static str) -> Self;

    fn name(&self) -> &'static str;
    async fn handle(&self, args: Vec<String>) -> Result<&str, Box<dyn error::Error>>;
}

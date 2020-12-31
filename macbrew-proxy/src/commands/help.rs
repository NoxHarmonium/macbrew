use super::super::data::api_data_manager::ApiDataManager;
use super::command::Command;
use async_trait::async_trait;
use std::error;

pub struct HelpCommand {
    name: &'static str,
}

#[async_trait]
impl Command<ApiDataManager> for HelpCommand {
    fn new(name: &'static str) -> Self {
        HelpCommand { name: name }
    }

    fn name(&self) -> &'static str {
        self.name
    }
    async fn handle(&self, _args: Vec<String>) -> Result<&str, Box<dyn error::Error>> {
        Ok(r#"
            macbrew-proxy expects a single command at a time, delimeted by new line characters.
            A command begins with a single keyword which specifies the command and then pipe delimitered parameters (if applicable)

            Accepted commands:
            ls - List Sessions
        "#)
    }
}

use crate::data::data_manager::DataManager;
use crate::data::sessions::{BrewSession, BrewSessionsResponse};
use crate::error::{Error, Result};
use async_trait::async_trait;
use reqwest;
use reqwest::header::HeaderMap;
use serde::Deserialize;

#[derive(Deserialize, Debug)]
struct Config {
    brewers_friend_api_key: String,
}

pub struct ApiDataManager;

fn create_client() -> Result<reqwest::Client> {
    let mut headers = HeaderMap::new();

    let config = match envy::from_env::<Config>() {
        Ok(config) => config,
        Err(error) => panic!("Missing environment variables: {:#?}", error),
    };

    headers.insert("X-API-KEY", config.brewers_friend_api_key.parse().unwrap());

    let client = reqwest::Client::builder()
        .default_headers(headers)
        .build()?;

    Ok(client)
}

#[async_trait]
impl DataManager for ApiDataManager {
    async fn sessions() -> Result<Vec<BrewSession>> {
        println!("Fetching sessions...");
        let client = create_client()?;

        let brew_sessions_url = "https://api.brewersfriend.com/v1/brewsessions";

        let res = client.get(brew_sessions_url).send().await?;

        let body = res.text().await?;

        let response: BrewSessionsResponse = match serde_json::from_str(&body) {
            Ok(response) => response,
            Err(error) => panic!(
                "Could not decode response from Brewers Friend API: {:#?}",
                error
            ),
        };

        println!("Fetched [{:#?}] sessions", response.brewsessions.len());

        Ok(response.brewsessions)
    }

    async fn session(id: &str) -> Result<BrewSession> {
        println!("Fetching session with id [{}]...", id);
        let client = create_client()?;

        let brew_session_url = format!("https://api.brewersfriend.com/v1/brewsessions/{}", id);

        let res = client.get(&brew_session_url).send().await?;

        let body = res.text().await?;

        let response: BrewSessionsResponse = match serde_json::from_str(&body) {
            Ok(response) => response,
            Err(error) => panic!(
                "Could not decode response from Brewers Friend API: {:#?}",
                error
            ),
        };

        let session = match response.brewsessions.as_slice() {
            [] => Err(Error::ApiResponseValidationError {
                message: String::from("No session with ID found"),
            }),
            [session] => Ok(session.to_owned()),
            [_args @ ..] => Err(Error::ApiResponseValidationError {
                message: String::from("Multiple sessions for ID found. This should not happen"),
            }),
        };

        println!("Fetched session");

        session
    }
}

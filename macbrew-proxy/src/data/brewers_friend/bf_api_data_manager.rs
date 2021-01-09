use crate::data::brewers_friend::bf_data_manager::BFDataManager;
use crate::data::brewers_friend::recipes::BeerXml;
use crate::data::brewers_friend::sessions::{BFBrewSession, BFBrewSessionsResponse};
use crate::error::{Error, Result};
use async_trait::async_trait;
use reqwest;
use reqwest::header::HeaderMap;
use serde::Deserialize;

#[cfg(test)]
use mockito;

#[derive(Deserialize, Debug)]
struct Config {
    brewers_friend_api_key: String,
}

pub struct BFApiDataManager;

fn get_base_url() -> String {
    #[cfg(test)]
    return mockito::server_url();
    #[cfg(not(test))]
    return String::from("https://api.brewersfriend.com");
}

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
impl BFDataManager for BFApiDataManager {
    async fn sessions() -> Result<Vec<BFBrewSession>> {
        println!("Fetching sessions...");
        let client = create_client()?;

        let brew_sessions_url = format!("{}/v1/brewsessions", get_base_url());

        let res = client.get(&brew_sessions_url).send().await?;

        let body = res.text().await?;

        let response: BFBrewSessionsResponse = match serde_json::from_str(&body) {
            Ok(response) => response,
            Err(error) => panic!(
                "Could not decode response from Brewers Friend API: {:#?}",
                error
            ),
        };

        println!("Fetched [{:#?}] sessions", response.brewsessions.len());

        Ok(response.brewsessions)
    }

    async fn session(id: &str) -> Result<BFBrewSession> {
        println!("Fetching session with id [{}]...", id);
        let client = create_client()?;

        let brew_session_url = format!("{}/v1/brewsessions/{}", get_base_url(), id);

        let res = client.get(&brew_session_url).send().await?;

        let body = res.text().await?;

        let response: BFBrewSessionsResponse = match serde_json::from_str(&body) {
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
        println!("Fetched session with id [{}]...", id);

        session
    }

    async fn recipe(recipe_id: &str) -> Result<BeerXml> {
        println!("Fetching recipe with id [{}]...", recipe_id);
        let client = create_client()?;

        let brew_session_url = format!("{}/v1/recipes/{}.xml", get_base_url(), recipe_id);

        let res = client.get(&brew_session_url).send().await?;

        let body = res.text().await?;

        let response = match quick_xml::de::from_str(&body) {
            Ok(response) => response,
            Err(error) => panic!(
                "Could not decode response from Brewers Friend API: {:#?}",
                error
            ),
        };

        println!("Fetched recipe with id [{}]...", recipe_id);

        Ok(response)
    }
}

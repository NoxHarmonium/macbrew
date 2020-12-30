use reqwest;
use reqwest::header::HeaderMap;
use serde::Deserialize;

#[derive(Deserialize, Debug)]
struct Config {
    brewers_friend_api_key: String,
}

#[derive(Deserialize, Debug)]
pub struct BrewSession {
    pub id: String,
    pub phase: String,
    pub batchcode: String,
    pub recipe_title: String,
}

#[derive(Deserialize, Debug)]
pub struct BrewSessionsResponse {
    pub message: String,
    pub count: String,
    pub brewsessions: Vec<BrewSession>,
}

pub async fn load_sessions() -> Result<BrewSessionsResponse, reqwest::Error> {
    let mut headers = HeaderMap::new();

    let config = match envy::from_env::<Config>() {
        Ok(config) => config,
        Err(error) => panic!("Missing environment variables: {:#?}", error),
    };

    headers.insert("X-API-KEY", config.brewers_friend_api_key.parse().unwrap());

    let client = reqwest::Client::new();

    let brew_sessions_url = "https://api.brewersfriend.com/v1/brewsessions";

    let res = client
        .get(brew_sessions_url)
        .headers(headers)
        .send()
        .await?;

    println!("Status: {}", res.status());

    let body = res.text().await?;

    let response: BrewSessionsResponse = match serde_json::from_str(&body) {
        Ok(response) => response,
        Err(error) => panic!(
            "Could not decode response from Brewers Friend API: {:#?}",
            error
        ),
    };

    Ok(response)
}

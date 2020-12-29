use reqwest;
use reqwest::header::HeaderMap;
use serde::Deserialize;

#[derive(Deserialize, Debug)]
struct Config {
    brewers_friend_api_key: String,
}

#[derive(Deserialize, Debug)]
struct BrewSession {
    id: String,
    phase: String,
    batchcode: String,
    recipe_title: String,
}

#[derive(Deserialize, Debug)]
struct BrewSessionsResponse {
    message: String,
    count: String,
    brewsessions: Vec<BrewSession>,
}

pub async fn load_sessions() -> Result<(), reqwest::Error> {
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

    println!("{:#?}", response);

    Ok(())
}

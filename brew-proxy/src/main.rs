

use reqwest;
use tokio;
use std::env;
use reqwest::header::HeaderMap;

#[tokio::main]
async fn main() -> Result<(), reqwest::Error> {
    let mut headers = HeaderMap::new();

    let api_key_env_var = "BREWERS_FRIEND_API_KEY";
    let api_key =  env::var(api_key_env_var).unwrap();

    headers.insert("X-API-KEY", api_key.parse().unwrap());

    let client = reqwest::Client::new();

    let brew_sessions_url = "https://api.brewersfriend.com/v1/brewsessions";

    let res = client.get(brew_sessions_url)
        .headers(headers)
        .send()
        .await?;

    println!("Status: {}", res.status());

    let body = res.text().await?;

    println!("Body:\n\n{}", body);

    Ok(())
}
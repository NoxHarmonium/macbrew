use reqwest;
use tokio;

mod data;

#[tokio::main]
async fn main() -> Result<(), reqwest::Error> {
    let session_data = data::sessions::load_sessions().await?;

    println!("{:#?}", session_data);

    Ok(())
}

use crate::error::Error;
use crate::error::Result;
use reqwest;
use reqwest::header::HeaderMap;
use serde::{Deserialize, Serialize};

#[derive(Serialize, Deserialize, Debug, Clone)]
pub struct BrewSession {
    pub id: String,
    pub phase: String,
    pub batchcode: String,
    pub recipe_title: String,
    pub recipeid: String,
}

#[derive(Deserialize, Debug)]
pub struct BrewSessionsResponse {
    pub message: String,
    pub count: String,
    pub brewsessions: Vec<BrewSession>,
}

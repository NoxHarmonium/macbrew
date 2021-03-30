use serde::{Deserialize, Serialize};

#[derive(Serialize, Deserialize, Debug, Clone)]
pub struct BfBrewSession {
    pub id: String,
    pub phase: String,
    pub batchcode: String,
    pub recipe_title: String,
    pub recipeid: String,
}

#[derive(Serialize, Deserialize, Debug)]
pub struct BfBrewSessionsResponse {
    pub message: String,
    pub count: String,
    pub brewsessions: Vec<BfBrewSession>,
}

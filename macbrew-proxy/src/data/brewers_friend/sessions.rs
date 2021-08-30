use serde::{Deserialize, Serialize};

#[derive(Serialize, Deserialize, Debug, Clone)]
pub struct BfBrewSessionRecipe {
    pub id: String,
    pub stylename: String,
    pub created_at: String,
}

#[derive(Serialize, Deserialize, Debug, Clone)]
pub struct BfBrewSession {
    pub id: String,
    pub phase: String,
    pub batchcode: String,
    pub recipe_title: String,
    pub recipeid: String,
}

#[derive(Serialize, Deserialize, Debug, Clone)]
pub struct BfBrewSessionFull {
    pub id: String,
    pub phase: String,
    pub batchcode: String,
    pub recipe_title: String,
    pub recipeid: String,
    pub recipe: BfBrewSessionRecipe,
    pub created_at: String,
}

#[derive(Serialize, Deserialize, Debug)]
pub struct BfBrewSessionsResponse {
    pub message: String,
    pub count: String,
    pub brewsessions: Vec<BfBrewSession>,
}

#[derive(Serialize, Deserialize, Debug)]
pub struct BfBrewSessionsFullResponse {
    pub message: String,
    pub count: String,
    pub brewsessions: Vec<BfBrewSessionFull>,
}

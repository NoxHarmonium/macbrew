use serde::{Deserialize, Serialize};

#[derive(Serialize, Deserialize, Debug, Clone)]
pub struct Recipe {
    pub name: String,
    pub version: String,
    pub type: String,
}


#[derive(Serialize, Deserialize, Debug, Clone)]
pub struct BrewSession {
    pub id: String,
    pub phase: String,
    pub batch_code: String,
    pub recipe_title: String,
}

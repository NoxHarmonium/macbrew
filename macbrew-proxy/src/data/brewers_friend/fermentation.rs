use serde::{Deserialize, Serialize};

#[derive(Serialize, Deserialize, Debug)]
pub struct BfFermentationReading {
    pub login_id: Option<u32>,
    pub brewevent_id: Option<u32>,
    pub recipe_id: Option<u32>,
    pub ip: Option<String>,
    pub name: String,
    pub gravity: String,
    pub gravity_unit: String,
    pub temp: String,
    pub temp_unit: String,
    pub comment: String,
    pub beer: Option<String>,
    pub interval: Option<String>,
    pub created_at: String,
    pub source: String,
}

#[derive(Serialize, Deserialize, Debug)]
pub struct BfFermentationResponse {
    pub message: String,
    pub readings: Vec<BfFermentationReading>,
}

use crate::error::Error;
use crate::error::Result;
use reqwest;
use reqwest::header::HeaderMap;
use serde::{Deserialize, Serialize};

#[derive(Serialize, Deserialize, Debug, Clone)]
pub struct Recipe {
    pub id: String,
    pub title: String,
    pub stylename: String,
    pub brewmethod: String,
    pub batchsize: String,
    pub batch_size_mode: String,
    pub boilsize: String,
    pub boiltime: String,
    pub efficiency: String,
    pub displayunits: String,
    pub og: String,
    pub fg: String,
}

#[derive(Deserialize, Debug)]
pub struct RecipeResponse {
    pub message: String,
    pub count: String,
    pub recipes: Vec<Recipe>,
}

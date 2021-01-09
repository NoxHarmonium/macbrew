use crate::data::brewers_friend::sessions;
use serde::{Deserialize, Serialize};

/// Enough information to decide which session to fetch more info on
#[derive(Serialize, Debug, Clone)]
pub struct BrewSessionReference {
    id: String,
    batch_code: String,
    name: String,
}

#[derive(Serialize, Deserialize, Debug, Clone)]
pub struct BrewSession {
    pub id: String,
    pub phase: String,
    pub batch_code: String,
    pub recipe_title: String,
    pub recipe_id: String,
}

impl BrewSessionReference {
    pub fn from_bf_session(session: &sessions::BrewSession) -> BrewSessionReference {
        BrewSessionReference {
            id: session.id.clone(),
            batch_code: session.batchcode.clone(),
            name: session.recipe_title.clone(),
        }
    }
}

impl BrewSession {
    pub fn from_bf_session(session: &sessions::BrewSession) -> BrewSession {
        BrewSession {
            id: session.id.clone(),
            phase: session.phase.clone(),
            batch_code: session.batchcode.clone(),
            recipe_title: session.recipe_title.clone(),
            recipe_id: session.recipeid.clone(),
        }
    }
}
